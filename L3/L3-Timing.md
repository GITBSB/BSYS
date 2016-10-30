# Matrix

Die folgenden Messungen wurden mit 2 2000x2000-Matrizen durchgeführt;
(wesentlich) größere Matrizen sind wegen des begrenzten Speicherplatz-
Kontingents nicht möglich.

| Threads | Zeit      |
|---------|-----------|
| 1       | 3m 11.09s |
| 2       | 2m 27.03s |
| 4       | 1m 19.08s |
| 8       | 0m 45.09s |
| 16      | 0m 34.65s |

Wie man an den Werten ablesen kann, skaliert das Programm nicht ideal
von 1 auf 2 Threads; aber verdoppelt man die Thread-Zahl weiter,
skaliert die Zeit (fast) ideal.

Das relativ schlechte Abschneiden der 2 Threads im Vergleich zu einem
liegt an den verschiedenen Implementierungen der Single- und
Multithreaded-Variante. Bei der letztgenannten sind für jedes Element
der Ziel-Matrix Indizes innerhalb der Wokrer-Threads zu berechenen.
ab 2 Threads skaliert die Implementierung relativ gut, weil die Index-
Berechnungen einmal pro Element in der Zielmatrix notwendig sind (je
eine Division und eine Modulo-Operation).


# mthread

## Wie viele Threads können maximal erstellt werden?
Da wir mit POSIX-Threads arbeiten wird die Anzahl der Threads durch den
Kernel limitiert. Es kann zum Beispiel in der Datei
"/proc/sys/kernel/threads-max" eine Maximalzahl für Threads eingetragen
werden.

Die Anzahl der Threads kann auch durch den Speicher limitiert werden,
z.B. wenn es noch freie Thread-IDs gibt, aber keinen Speicher für
weitere Stacks.

## Zeitverhalten

## Ohne pthread_join()-Aufrufe
Ließe man die Aufrufe von pthread_join() für jeden Thread weg, würde der
Hautp-Thread nach dem Erstellen der Warte-Threads beendet werden und
damit der ganze Prozess; also würden die neu erstellten Threads ohne zu
Warten auch beendet, ohne eine Ausgabe zu machen.


# Race-Condition

##Performance-Unterschied zwischen Mutex und Peterson Algorithmus

|Modifikationen:|    100     |    200     |    300     |   400      |
|               | t:10, l:10 | t:20, l:10 | t:30, l:10 | t:40, l:10 |
|---------------|------------|------------|------------|------------|
|Peterson       | 2.2157s    | 8.1978s    | 25.7730s   | 48.3500s   |
|---------------|------------|------------|------------|------------|
|Mutex          | 0.6138s    | 1.16524s   | 1.7955s    | 2.4597s    |


|Modifikationen:|    200     |    300     |    400     |   800      |
|               | t:10, l:20 | t:10, l:30 | t:10, l:40 | t:10, l:80 |
|---------------|------------|------------|------------|------------|
|Peterson       | 4.1847s    | 5.7080s    | 8.5740s    | 17.4653    |
|---------------|------------|------------|------------|------------|
|Mutex          | 1.0504s    | 1.6276s    | 2.0260s    | 4.1549     |

Den Messungen zu Folge, ist die Implementation des Mutex einiges
Performancestärker. 
Mit der Erhöhung der Threads wird der Zeit-Unterschied immer deutlicher.
Im Peterson Algorithmus gibt es mit mehr Threads, mehr Ebenen zum
durchlaufen und somit deutlich mehr Operationen und Abfragen. Bei einem
Mutex ist wie erwartet eher eine lineare Zeitsteigerung zu erkennen.Mit 
der Erhöhung der Loops, ist der Verlauf der Messungen bei Beiden linear.


# C in Python

## Performance-Unterschied der einzelnen Wurzel Funktionen
Anhand der Messungen sieht man die Performance-Unterschied zwischen den
jeweiligen Funktion und deren Implementierung. 
Die Zeitmessungen wurden nur mit einer Wiederholung aufgenommen 
und bilden daher keinen Mittelwert.

|Funktion               |Zeit          |
|-----------------------|--------------|
|cyforksqrt.sqrtm(5.0)  |1.66893 ms    |
|cyforksqrt.sqrt2(5.0)  |10.96725 ms   |
|forksqrt.sqrt2(5.0)    |209.09309 ms  |
|Math.sqrt(5.0)         |1.19209 ms    |

Die Zeiten der C Funktion in Cython sqrtm() ähneln sich jedoch mit denen
von Python Math.sqrt() sehr oder sind gleich. Die Funktion mit der
Methode von Heron in forksqrt.sqrt2 hat eine sehr lange Zeitmessung.
Begründen lässt sich das hauptsächlich durch die vielen if Abfragen, 
um die Default-Werte, wenn benötigt, einzusetzen und das parsen des
Toleranzwertes. Diese Operationen fallen in der Heron Funktion in Cython
sqrt2() weg.
Zusätzlich gibt es weitere Optimierungen, die Cython ermöglicht.
Suboptimal ist auch der Standartwert 100 des Loops, der bis zu einem
erreichten Wert keinen Unterschied mehr bringt.
