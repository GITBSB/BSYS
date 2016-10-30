# Generischer Stack

## New
Es wird für vier Elemente Speicher auf dem Heap reserviert, dafür muss
die korrekte Anzahl von belegten Bytes pro Element übergeben werden.

Für komplexe Datentypen, die eine eigene Speicherbereinigung brauchen,
muss eine freefn() übergeben werden, die die Speicherbereiche des
Objekts, die über Zeiger referenziert werden, freigibt. Nach dem Aufruf
dieser Funktion darf von dem Objekt nur ein zusammenhängender Speicher-
bereich von n Bytes zurückbleiben, wobei n die oben übergebene Größe des
Typs ist.

## Push
Bei Push werden so viele Bytes, wie bei New() angegeben, von der über-
gebenen Adresse in die nächste freie des Stacks kopiert. Falls nicht
genug Speicher für ein nächstes Element frei ist, wird für den Stack
doppelt so viel Speicher angefordert, wie er bereits groß ist.

Es wird deshalb kopiert, damit nicht ausserhalb des Stacks ein Element
verändern kann, das auf dem Stack liegt. Das gilt allerdings nur für die
Attribute des Elements, die direkt darunter leigen; ein Attribut, auf
das das element nur eine Referenz oder Zeiger hat, kann man trotzdem
noch über das ursprüngliche Element verändern.

Die Zeigerarithmetik wird mit Zeigern auf den Typ char realisiert, weil
das Makro "sizeof" ein Vielfaches der Größe des Typs "char" zurückgibt.
Diese ist zwar plattformabhängig, sollte aber funktionieren, da "sizeof"
das auch ist. Ich hatte zwar die Idee die Zeigerarithmetik mit Zeigern
auf den Typ int8_t zu realisieren, da size8_t auf allen Platformen 8 Bit
lang ist; das passt allerdings nicht zu der platfomspezifischen Rückgabe
vom Makro "sizeof".

## Pop
Bei Pop wird das neueste Element vom Stack an die Speicheradresse
kopiert, auf die das Argument zeigt; es muss schon vorher Speicher für
das Element allokiert worden sein.

Nach dem Kopieren wird innerhalb des Pop()-Aufrufs wie bei Dispose auch
die Funktion freefn() fur das aktuelle Element ausgeführt, da es nicht
mehr auf dem Stack liegt und somit aufgeräumt werden muss. Hierbei muss
beachtet werden, dass innerhalb der freefn() nicht Speicherbereiche
freigegeben werden, die nach dem Kopieren von dem Stack noch
referenziert werden. Die müssen von dem Benutzer der Bibliothek selbst
freigegeben werden.

## Dispose
Hier wird, falls der bei GenStackNew() übergebene Funktionszeiger nicht
NULL war, für jedes Element auf dem Stack diese Funktion, mit einem
Zeiger auf den Speicherbereich als Parameter, aufgerufen. Es wird
erwartet, dass die Funktion freefn() Objekte vom eigenen Typ korrekt
"aufräumt"; gemeint ist, dass Speicherbereiche im Heap, die durch das
Objekt per Zeiger genutzt werden mit free() aufgeräumt werden. Nach dem
Aufruf von freefn() darf also nur der Speicher auf dem Stack selbst
belegt bleiben.

Der Zeiger auf das nächte Objekt wird berechnet, indem ein Offset
berechnet wird, der ein Vielfaches der Elementgröße ist - mit einem
Faktor von 0 bis logLength -1. Dieser Offset wird auf s->elems addiert,
um auf das aktuelle Objekt zu zeigen.

Danach wird der Speicher für den eigentlichen Stack freigegeben.
