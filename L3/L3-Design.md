# Matrixmultiplizierung

## Problemstellung
Es soll die Multiplikation von quadratischen und freiwillig auch die von
nicht-quadratischen Matrizen implementiert werden; es müssen dabei ein
oder mehrere Threads genutzt werden.
Das Generieren von Matrizen muss in Python geschehen, das Einlesen und
Multiplizieren in C.


## Einleitung
Das Generieren von Matzizen in Python war relativ einfach, da nur
zufällige Kommazahlen berechnet und entsprechend formatiert ausgegeben
werden müssen.

Für die Matrix-Mulitplikation mit einem Thread wurde der "naive" Anzatz
mit 3 verschachtelten Schleifen gewählt, bei der für jedes Element der
Ziel-Matrix das Skalar-Produkt der ensprechenden Zeile der ertsen mit
der entsprechenden Spalte der zweiten Matrix gebildet wird.

Bei der parallelen Multiplikation berechnet jeder Thread das
numThreads-te Element, mit je einem anderen Start-Element; siehe auch
"Weitere Themen" -> "Überblick über die parallele Multiplikation".


## Funktionen

### Matrix *readMatrix(char[] filename)
Diese Funktion liest eine Matrix aus einer Datei, die darin gespeicherte
Matrix muss quadratisch sein; die Funktion reserviert genügend Speicher
für die Matrix aus der Datei und liefert im Fehlerfall einen NULL-Zeiger
zurück. Es wird erwartet, dass der Benutzer der Bibliothek den
reservierten Speicher selbst wieder freigibt.

### Matrix *multiplyMatrix(Matrix *a, Matrix *b, int threads)
Diese Funktion erwartet 2 Zeiger auf korrekt initialisierte Matrizen
(a und b; rows * columns = AnzahlElemente) und eine Ganze Zahl für die
Anzahl der zu benutzenden Threads größer als 0.

Für die Ziel-Matrix wird entsprechend viel Speicher reserviert, den der
Benutzer der Bibliothek nach Benutzen wieder frei geben muss.

Diese Funktion dispatcht auf eine der Funktionen:
oneThreadedMultiplication() oder multiThreadedMultiplication().

### double multiplyRowColumn(Matrix *a, int row, Matrix *b, int column)
Diese Funktion erwartet 2 Zeiger auf korrekt initialisierte Matrizen
(a und b; rows * columns = AnzahlElemente). "row" muss kleiner sein als
die Anzahl der Zeilen von a; "colums" kleiner als die Anzahl der Spalten
von b.

Es wird das Skalarprodukt (Summe der elementweisen Produkte) der Reihe
"row" von a und der Spalte "column" von b.

### Matrix *createMatrix(int numRows, int numColumns)
Es wird eine Matrix mit der entsprechend vielen Zeilen und Spalten
erstellt. Tritt während der Speicheranfrage ein Fehler auf, wird die
Matrix mit cleanUpMatrix() aufgeräumt.

### void cleanUpMatrix(Matrix *m, int lastInitializedRow)
Diese Funktion habe ich der API hinzugefügt, um einfach eine Matrix
aufräumen zu können. Sie gibt den Speciherplatz von der ersten bis zur
lastInitializedRow-ten Reihe frei. Im Fehlerfall kann ich den Index der
bisher erfolgreich allokierten Reihen übergeben; die Funktion freet den
Speicher entsprechend.

### void oneThreadedMultiplication(Matrix *a, Matrix *b, Matrix *result)
Implementiert Matrix-Multiplizierung mit einem Thread (siehe 
"Einleitung").

### void multiThreadedMultiplication(Matrix *a, Matrix *b, Matrix *result, int numThreads)
Implementiert die parallele Matrix-Multiplizierung mit numThreads
Threads (siehe "Weitere Themen" -> "Überblick über die parallele
Multiplikation").

Diese Funktion erzeugt die Threads, seine Argumente und wartet auf jeden.

### struct MatThreadArg
```c
typedef struct MatThreadArg{
    Matrix *a;
    Matrix *b;
    Matrix *c;
    int threadNumber;
    int numberOfThreads;
} MatThreadArg;
```
Enthält die nötigen Werte damit jeder Thread (Argument) "seine" Elemente
multiplizieren kann.

### void *mulitplyMatrixMT(void *argAsVoid)
Diese Funktion ist die, die jeder Thread ausführt; sie "packt" das
Argument aus und berechnet die für den Thread entsprechenden Elemente.

### int elemsInLine(char *line)
Diese Funktion liefert die Anzahl der Kommazahlen in der gegeben Zeile
zurück. Das Argument muss unhleich NULL sein. Diese Funktion wird
benutzt, um die Anzahl der Spalten für die einzulesend Matrix zu
bestimmen, falls die Matrix nicht quadratisch ist.

### int countLines(const char filename[])
Diese Funktion bestimmt die Anzahl der Zeilen der Matrix in der Datei
mit dem Namen filename. Diese Funktion wird benutzt, um die Anzahl der
Zeilen für die einzulesend Matrix zu bestimmen, falls die Matrix nicht
quadratisch ist.


## Weitere Themen

### Dimensionen der Matrix in einer Datei
Die Datei, in der eine Matrix enthalten ist, muss am Ende eine Leerzeile
enthalten; falls sie keine hat, wird die letzte Zeile der Matrix nicht
eingelesen.

Die Matrizen für das Programm müssen nicht quadratisch sein.

Es muss aber beachtet werden, dass die Matrix in der Datei am Parameter
"a" so viele Zeilen hat, wie die Matrix in Datei für "b" Spalten hat.

### Überblick über die parallele Multiplikation
Die Vorgehensweise für den Multithreaded-Teil orientiert sich am Single-
Threaded-Ansatz. Für jedes Element der Zielmatrix wird wieder die
Produkt-Summe gebildet.

Die Berechnungen dieser sind auf die Threads aufgeteilt: Die Threads
berechnen versetzt je das N-te Element, wobei N die Anzahl der Threads ist.

Bei 4 Threads (und Inidzes ab 0) berechnet der erste Thread das 0-te,
4-te, 8-te, ... Element. Der zweite Thread berechnet das 1-te, 5-te,
9-te, ... Element und für die restlichen beiden Threads funktioniert das
analog.

Dieser elementweise Anzatz wurde gewählt, um bei kleinen Matrizen auch
mit einer großen Anzahl Threads zu skalieren. Bei einem Zeilen- oder
Spalten-orientierten Ansatz wären eventuell nicht genug Spalten für alle
Threads vergügbar.
Ein Besipeil wäre es zwei 10x10-Matrizen mit 16 Threads zu
multiplizieren. Die ersten 10 Threads könnten je eine Zeile/Spalte der
Ziel-Matrix berechnen, die restlichen 6 liefen leer.


## Kritik

### Algorithmus
Bekäme man mehr Zeit für die Aufgabe, hätte man sich in einen
Algorithmus mit einer besseren Komplexität einarbeiten können; davon
gibt es mehrere, wie zum Beispiel den Strassen- oder den
Coppersmith–Winograd-Algorithmus.

### Skalierung mit mehreren Threads
Unsere Implementierung der parallelen Matrixmultiplikation braucht wie
zu erwarten war weniger Zeit als die serielle Implementierung; leider
skaliert sie nicht ideal, braucht bei 2 Threads nicht halb so viel Zeit
wie mit einem Thread.

Zum Teil liegt der Grund dafür in der Berechnung der Indizes für die
noch zu berechnenden Elemente. Hier findet eine Division und eine
Modulo-Operation statt, die zum einen bei der Single-Threaded-Version
wegfallen und zum anderen im Vergleich zur Addition und Multiplikation
viel Ausführungszeit benötigen.

Ein Anderer Grund für das nicht-ideale Skalieren ist vielleicht die
Fehlende Optimierung auf Lokalität:
Bei der Single-Threaded-Variante werden nacheinander die Reihen der
Zielmatrix berechnet, innerhalb einer Zeile wird immer wieder auf die
Elemente derselben Reihe der ersten Quell-Matrix zugegriffen.
Bei der Milti-Threaded-Variante ist das nicht mehr unbedingt gegeben, da
die Threads in verschiedenen Reihen der Ziel-Matrix Elemente berechnen
können. Dabei können sich die Threads gegenseitig die zu lesenden Reihen
überschreiben.

