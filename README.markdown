## About ##
First of' : *thank you for dropping by!* I'll be short : this repo' contains some basic graphics jobs done for the EGIOC (Graphical and Human Interface Programming) course in Polytechnics University of Timisoara, Romania. 

This file contains the homework assignments as postulated by the class professor (*Sorin Babii*).
The rest of the file is written in Romanian (Babii's native language), but can be easily translated with a translator such as *Google Translate* ([the link](http://translate.google.com/)).

**Cheers**

## Lucrarea 1: Formatul grafic XPM ##
Tema acestui laborator presupune realizarea unei biblioteci ce va contine un set de functii necesare manipularii imaginilor in format XPM. Biblioteca implementata in cadrul acestei lucrari va fi refolosita in urmatorele lucrari.

Implementati functiile corespunzatore unei biblioteci XPM ce vor trata urmatoarele aspecte:

1.  Initializarea (in memorie) a unui element de tip structura ce contine datele corespunzatoare unei imagini XPM si este echivalent cu reprezentarea imaginii in memorie.
2.  Posibilitatea definirii unei culori (RGB) impreuna cu caracterul/'gruparea de caractere' asociata (vor fi afectate datele elementului de tip structura).
3.  Setarea unui pixel al imaginii din memorie , pornind de la coordonatele x si y si o culoare definita prin procedee specificate anterior.
4.  Exportul intr-un fisier (pe disc) cu formatul XPM pornind de la structura definita anterior.

Sa se realizeze un program ce utilizeaza functiile acestei biblioteci. Programul va genera fisierul XPM cu dimensiunile 50x50 ce corespunde imaginii urmatoare:
![Expected output](http://i41.tinypic.com/s266o9.gif)

Elementele de pe prima coloana au culoarea #000000(negru), elementele de pe ultima coloana au culoarea #FF0000(rosu 100%), elementele de pe coloana centrala #7F0000(rosu 50%), etc. In total vor fi 50 de culori, fiecareia corespunzandu-i cate un singur caracter.

## Lucrarea 2: Trasarea segmentelor de dreapta ##
#### Introducere in PostScript simplificat ####
In acest paragraf vor fi prezentate cateva aspecte necesare pentru rezolvarea temei propuse in acest laborator. Descrierea ce urmeaza trateaza doar o parte infima (suficienta insa pentru implementarea aplicatiei din paragraful C ) a structurii formatului PostScript.

Exemplu :

    /Line {moveto lineto stroke} bind def
    %%%BEGIN
    25 126 71 126 Line
    71 126 71 141 Line
    71 141 17 141 Line
    17 141 17 76 Line
    17 76 49 76 Line
    120 126 76 126 Line
    76 126 76 141 Line
    
    76 141 130 141 Line
    130 141 130 58 Line
    130 58 119 58 Line
    167 76 136 76 Line
    136 76 136 141 Line
    136 141 190 141 Line
    190 141 190 132 Line
    %%%END

Rezultat :

![PS Output](http://i43.tinypic.com/kebzid.gif)

Fisierele in format PostScript sunt fisiere text, avand in general extensia .ps. In exemplul anterior, structura PostScript din partea stanga are atasata ca rezultat imaginea din dreapta.

*  In formatul PS, coordonatelor (x,y)=(0,0) le corespunde coltul din partea stanga-jos a ferestrei / imaginii (!!! atentie la modul in care functia PutPixel din cadrul bibliotecii XPM considera sistemul de coordonate - (x,y)=(0,0)=stanga-sus).
*  Programul tema va trebui sa trateze o singura "comanda": X1 Y1 X2 Y2 Line Aceasta traseaza un segment de dreapta intre punctele de coordonate (X1,Y1) , respectiv (X2,Y2).
*  Comenzile sunt incadrate intre delimitatorii : %%%BEGIN si %%%END (in exemplul de sus sunt trasate 14 segmente de dreapta). Textul aflat in exteriorul delimitatorilor va fi ignorat.
*  In interiorul constructiei BEGIN-END pot exista linii vide (in exemplul precedent, intre "segmentele" 76-126-76-141 si 76-141-130-141 exista o linie vida).

#### Cerinte ####
1.  Realizati o functie **line(CoordonatePct1,CoordonatePct2,... alte_date_pe_care_le_considerati_utile...)** ce traseaza un segment de dreapta intre doua puncte de coordonate date folosind [algoritmul lui Bresenham](http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm). Functia va lucra pe structurile implementate in lucrarea anterioara (va folosi functii si structuri din biblioteca XPM - laboratorul 1).

2.  Implementati o aplicatie ce porneste de la un fisier PostScript simplificat si genereaza fisierul imagine XPM asociat. Programul va primi din linia de comanda urmatorii parametri:

  *  **-f nume_fisier_ps** (numele fisierului PostScript simplificat utilizat ca intrare)
  *  **-w width** (latimea impusa imaginii XPM)
  *  **-h height** (inaltimea impusa imaginii XPM)
  *  **-o nume_fisier_XPM** (numele fisierului de iesire XPM)

Exemplu de utilizare: **lineout -f in.ps -w 200 -h 200 -o out.xpm**
