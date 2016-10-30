# Design Aufgabe 4

## Aufgabenstellung 
-----
Um das Producer/Consumer Problem zu lösen, soll ein entsprechendes 
Multithreading Programm geschrieben werden. Wichtig ist die 
Synchronisation zwischen den Producer und Consumer Threads, 
sowie der gemeinsame Puffer-Zugriff.

## Einleitung
-----
Die Posix-Funktionen werden zur Verwaltung der Threads genutzt und
getopt um folgende Optionen zu parsen.
    * Optionen:
        - -v 	verbose Ausgabe
        - -h 	GIT Version + Hilfetext
        - -i INPUT 	INPUT Datei
        - -o OUTPUT 	OUTPUT Datei
        - -L NR   Anzahl der Zeilen des Buffers
        - -C NR   Anzahl der Spalten pro Zeile
        - -c NR   Anzahl der Consumer Threads
        - -p NR   Anzahl der Producer Threads
        - -t NR (NR in Millisekunden) 	NR: passives Schlafen der Producer
        - -r NR (NR in Sekunden!) 	NR: Untere 'random' Zahl Grenze
        - -R NR (NR in Sekunden!) 	NR: Obere 'random' Zahl Grenze
        - -a NR (NR in Millisekunden)   aktives warten
            - mit NR: zusätzliche Wartezeit
        
Der von Consumer und Producer genutzte Puffer wird über Mutex und CV's 
synchronisiert. Dieser Zugriff gilt als kritischer Bereich und er soll 
somit von nur einem Thread zu einem bestimmten Zeitpunkt genutzt werden. 
Andere zugreifende Threads müssen warten bis der Zugriff wieder
freigegeben ist.

### prodcon.c
prodcon.c ist das Hauptprogramm, in dem die Eingabeparameter geparsed
werden und die Producer und Consumer Threads initialisiert werden.
Es werden keine globalen Variablen benutzt. 
Die von Producer und Consumer benötigten Variablen werden 
in structs übergeben.

### producer.c
Die Aufgabe der Producers ist es, die gegebenen Daten in einem 
gemeinsamen genutzten Puffer zu schreiben. 
* Übergebene Parameter:
    - struct producer_arg
        - int thread_num: Thread-ID
        - FILE *inFile: Zeiger auf Inputfile
        - int bufferLines: Anzahl Zeilen des Puffers
        - int bufferColumns: Anzahl Spalten des Puffers
        - char **buffer: Doppelpointer auf den Puffer
        - volatile int *linesInBuffer: Zeiger auf Anzahl Zeilen im Puffer
        - pthread_mutex_t *mutex_buffer: Zeiger auf Mutex, blockt den Puffer
        - pthread_cond_t *buffer_notempty: Condition Variable, gibt an, dass Puffer nicht mehr leer ist
        - pthread_cond_t *buffer_empty: Condition Variable, gibt an, dass Puffer nicht mehr voll ist
        - sem_t *sem_read:  Zeiger auf binäres Semaphore, welches die Input-Datei lockt
        - int v_flag: Verbose Flag
        - int timeToSleep: Zeit des passiven Schlafens
        - int thread_lines: Anzahl der zu bearbeitenden Zeilen pro Producer
        - int *assignment: gibt pro Bufferzeile an, ob sie beschrieben (1) oder leer(0) ist

### consumer.c
Der Consumer ist dafür zuständig, die Daten, in Form von Strings,
in Grossbuchstaben umzuwandeln. Dazu nimmt jeweils ein Consumer eine 
Zeile aus dem gemeinsamen Puffer und wandelt diese um, bis der Puffer 
leer ist.
* Übergebene Parameter:
    - struct consumer_arg
        - unsigned int thread_num: s. struct producer_arg
        - FILE *outFile: s. struct producer_arg
        - pthread_mutex_t *mutex_outFile: Zeiger auf Mutex, lockt die Output-Datei 
        - int bufferLines: s. struct producer_arg
        - int bufferColumns: s. struct producer_arg
        - volatile int *linesInBuffer: s. struct producer_arg
        - char **buffer: s. struct producer_arg
        - volatile int *assignment: s. struct producer_arg
        - volatile int *termin_reading: Zeiger auf Integer. 
            - Gibt an ob Producer beendet(1) oder am laufen sind(0)
        - pthread_mutex_t *mutex_buffer: s. struct producer_arg
        - pthread_cond_t *buffer_notempty: s. struct producer_arg
        - pthread_cond_t *buffer_empty: s. struct producer_arg
        - int randLow: Untere Zufallsgrenze für das Aktive Warten
        - int randHigh: Obere Zufallsgrenze für das Aktive Warten
        - int v_flag: s. struct producer_arg
        - int w_flag: Active Offset Flag

## Funktionen
-----

### prodcon.c

#### int main(int argc, char *argv[])
- Parsen mit getopt
- Erstellung von Mutex und Condition Variblen
- Initialisierung der Threads
- Übergabe der Argumente in structs für Producer und Consumer

#### char **createBuffer(int numLines, int numColumns)
Für den Buffer wird je nach Größe der übergebenen Parameter, für Zeilen 
und Spalten, mit Malloc Speicher allokiert. Tritt dabei ein Fehler auf,
wird der Speicher mit der Funktion freeBuffer wieder freigegeben.

#### void freeBuffer(char **buffer, int lines)
Wenn der Speicher nicht mehr benötigt wird oder bei der 
Speicherreservierung ein Fehler auftritt, wird er mit der Funktion 
free() wieder freigegeben.

#### void printHelp(void)
Zeigt den Hilfetext und die Gitversion auf der Konsole.

#### int parse_value(const char *opt_arg, int default_val)
Benutzt strtol() um den übergebenen Argumentenstring in einen 
Integer Wert umzuwandeln. Wenn dies nicht möglich ist, gibt es eine
Fehlerausgabe.

### producer.c

#### void *produce(void *arg)
Die Producer-Threads führen diese Funktion aus.
Datenfluss:
* Eine  Schleife solange durchlaufen, bis alle Zeilen des Thread in den 
Buffer geschrieben worden sind.
- Ein Schleifendurchlauf umfasst:
    1. Mutex mutex_buffer blocken, um zu erfassen ob der Buffer voll ist.
       - wenn ja: warten auf die Condition Variable buffer_empty in 
          einer weiteren while-Schleife
    2. Binäre Semaphore sem_read blocken, damit nur ein Producer zur
       gleichen Zeit aus einer Datei lesen kann
    3. Gelesene Zeile zwischenspeichern und sem_read wieder freigeben
    4. Wenn die Zeile nicht gelesen werden konnte, wird abgebrochen
    5. Die Zeile wird je nach Spaltenlänge abgeschnitten
    6. Wartezeit wenn durch -t angegeben
    7. Mutex mutex_buffer blocken und nach leerer Zeile in Buffer suchen:
        - *assignment durchlaufen und prüfen auf 0(leer)
    8. Warten auf die Condition Variable buffer_notempty, wenn der Buffer
    voll ist
    9. Gelesene Zeile in den Buffer schreiben
        - *assignment der entsprechenden Zeile auf 1 setzen(voll)
        - linesInBuffer um 1 erhöhen
    10. Signaliere buffer_notempty, um wartende Consumer aufzuwecken
    11. Mutex mutex_buffer freigeben

### consumer.c

#### void *consume(void *arg)
Die Consumer-Threads führen diese Funktion aus.
Datenfluss:
* Mutex mutex_buffer blocken
resumeConsumer setzen, wenn es noch Zeilen im Puffer hat oder wenn die 
Producer noch am laufen sind.
Solange dies der Fall ist wird die Schleife durchlaufen.
- Ein Schleifendurchlauf umfasst:
    1. Mutex mutex_buffer blocken, wait Variable zu bestimmen
        - Die wait varibale bestimmt, ob es Zeilen im Puffer gibt
        - wenn es keine gibt: wartet der Consumer auf die Condition 
        Variable buffer_notempty, bis es wieder eine Zeile gibt.
    3. Mutex mutex_buffer blocken, sodass wieder nur ein Thread in den 
    kritischen Bereich kann
    4. Suche nach einer geschriebenen Zeile im Puffer. 
        - *assignment durchlaufen und prüfen auf 1(voll)
        - wenn nicht erfolgreich:
        resumeConsumer erneut setzen(s. oben), Mutex mutex_buffer
        freigeben und zum Schleifenbeginn springen.
    5. Für das lesen aus dem Puffer wird Speicher für ein temporären 
    "String" allokiert und dort die gefundene Zeile aus dem Puffer 
    abgespeichert.
        - *assignment der entsprechenden Zeile auf 0 setzen(leer)
        - linesInBuffer um 1 erniedrigen
    6. Mutex mutex_buffer freigeben
    7. Übergabe des Strings an stringToupper(char *string), um ihn in 
    Grossbuchstaben umzuwandeln.
    8. aktive, passive Wartezeit
    9. Mutex mutex_outFile blocken, solange in Outputfile das Ergebnis 
    geschrieben wird -o OUTPUT
    10. Ergebnis ausgeben -v
    11. Mutex mutex_buffer blocken, resumeConsumer erneut setzen(s.oben)
    wenn Producer noch nicht beendet sind, signalisiere buffer_empty,
    um wartende Consumer aufzuwecken

#### char *stringToupper(char *string)
Wandelt einen String in Großbuchstaben um.
Zuerst wird die Länge des übergebenen Strings bestimmt, indem eine 
Variable solange erhöht wird, bis das Stringende mit '\0' erreicht ist. 
Dann wird Speicherplatz für einen neuen String reserviert und dort 
solange die umgewandelten Großbuchstaben reingeschrieben, bis die
errechnete Stringlänge erreicht ist. Anschlißend wird mit '\0' wird das
Stringende gekennzeichnet.

#### void activeWait(int sec)
Ist für das aktive Warten zuständig, dass durch die Ausführungszeit der 
Schleifendurchläufe zustande kommt.

## Synchronisation
In jedem Producer und Consumer wird vor jedem Zugriff auf den
gemeinsamen Puffer der Mutex mutex_buffer gelockt und danach wieder
freigegeben. Dadurch wird garantiert, dass nur ein Thread zur einer Zeit,
die Daten im Puffer bearbeitet.
Im Producer gibt es außerdem ein binäres Semaphore sem_read, welches die
Input-Datei lockt. Ein Mutex hätte die gleiche Aufgabe erfüllt.
Im Consumer wird die Output-Datei mit einem Mutex mutex_outFile geblockt,
um gleichzeitiges schreiben zu verhindern. Außerdem werden zwei
Condition Variablen verwendet. 
* buffer_notempty: Der Puffer ist nicht mehr leer und der Consumer wird
signalisiert, dass er weitermachen kann.
* buffer_empty: Puffer ist nicht mehr voll und eventuell wartende 
Producer werden aufgeweckt.

## Zeitverhalten

### Skalierung mit Threads
Für diese Messung wurde eine Eingabe-Datei mit 200 Zeilen verändert und
die unten aufgeführten Werte verändert.

| Consumer-Threads | Producer-Threads | Zeit      |
|------------------|------------------|-----------|
| 1                |  1               | 0m 0.003s |
| 2                |    2             | 0m 0.016s |
| 4                |     4            | 1m 32.016s |
Anscheinend blockieren sich die Threads mit steigender Anzahl zunehmned
gegensitig, da wir viele Variablen mit Locks schützen
