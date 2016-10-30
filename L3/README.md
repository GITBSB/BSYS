## C Code files
* work.c
    * Aktives warten für eine gegebene Zeitspanne (sekunden millisekunden)

* mthread.c
    * Generiert eine gegebene Anzahl von Threads, die eine zufällige Zeit zwischen einem Minimum und Maximum warten.

* multiply_matrix.h
    * Definitionen der Struktur und Funktionen der Matrixmupltiplikation.

* multiply_matrix.c
    * Implemenetierung der Matrixmultiplikation.

* matrix.c
    * Hauptprogramm für die Matrixmultiplikation

* Makefile
    * Entält Regeln zum Kompilieren von work.c, mthread.c und matrix.c.

## Python files
* matr-gen.py
    * Generiert eine quadratische Matrix mit Werten zwischen einem gegebenem Minimum und Maximum.

* racecondition.py
    * Generiert Threads, die mit verschiedenen Implementierungen auf eine globale Variable zugreifen und diese verändern. 

* forksqrt.py
    * Berechnet Quadrat Wurzeln mit Hilfe von forks und pipes.

* cyforksqrt.pyx
    * Implementierungen mit Cython um Quadrat Wurzeln zu berechnen.

* measure.py
    * Auswertungsprogramm, welches verschiedene Ansätze zur Quadrat Wurzel Berechnung durch Zeitmessung vergleicht.

* setup.py
    * Baut den cython Code von cyforksqrt.pyx zu einem Python Modul

* gen_inputsqrt.py
    *  leitet die Ausgabe, die eine per Option angegebene Zahl von zufälligen Gleitkommazahlen ausgibt, in eine Datei um.
