This repository contains some basic graphics tasks realized for the EGIOC (Graphical and Human Interface Programming) course at the Polytechnica University of Timisoara, Romania. In the following, we present the homework assignments given by Prof. Sorin Babii (in Romanian).

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

## Lucrarea 3: Operaţii de decupare (*Clipping*) ##
#### Clipping ####
Unul dintre algoritmii cei mai des utilizati pentru clipping-ul segmentelor de dreapta este [algoritmul Cohen-Sutherland](http://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm "Cohen-Sutherland algorithm").

Vom considera ca fereastra activa este definita de 4 valori reale: xmin, ymin, xmax, ymax. Un punct P(x,y) definit in coordonate utilizator este in interiorul ferestrei (deci vizibil) daca indeplineste relatiile :

1.  xmin <= x <= xmax
2.  ymin <= y <= ymax

Aceste relatii trebuie in principiu verificate pentru fiecare punct al segmentului, ceea ce evident, este imposibil. Algoritmul de clipping **Cohen-Sutherland** porneste de la determinarea pozitiei relative a segmentului fata de fereastra. Exista 4 posibilitati: 

1.  ambele capete ale segmentului sunt in interiorul ferestrei
2.  un capat al segmentului este in interior iar celelalt in exterior
3.  ambele capete ale segmentului sunt exterioare ferestrei, dar segmentul intersecteaza fereastra
4.  ambele capete sunt in exteriorul ferestrei dar segmentul nu intersecteaza fereastra
Planul utilizator este impartit in 9 regiuni carora li se asociaza cate un cod unic, format din 2 valori intregi (0 respectiv 1) conform figurii:

![region codes](http://i39.tinypic.com/war5te.gif "CS Regional Segmentation")

Descrierea algoritmului este urmatoarea:

1.  Determina codurile de regiune pentru punctele P1, P2
2.  Cele doua coduri sunt insumate logic (OR). Daca rezultatul este codul (0,0,0,0) segmentul este complet interior si este acceptat. EXIT. Altfel treci la pasul 3.
3.  Cele doua coduri sunt inmultite logic (AND). Daca rezultatul este diferit de codul (0,0,0,0) segmentul este complet in afara ferestrei si este eliminat. EXIT. Altfel treci la pasul 4.
4.  Se alege codul unui capat al segmentului. Daca acesta este codul (0,0,0,0) se alege celalalt cod.
5.  Se inspecteaza codul in ordinea b3, b2, b1, b0 pentru a determina primul element b i = 1 care precizeaza frontiera ferestrei fata de care punctul este exterior.
6.  Se calculeaza intersectia segmentului cu acea frontiera si se inlocuieste punctul cu intersectia determinata.
7.  Se reia cu pasul 1.

#### Cerinte ####

1.   Adaugati la biblioteca grafica o functie (ex: ClipLine) ce va realiza decuparea segmentelor utilizand **algoritmul Cohen-Sutherland**
2.   Modificati aplicatia realizata in laboratorul precedent adugand facilitati pentru decuparea segmentelor de dreapta. Programul va accepta din linia de comanda un nou set de parametri:
  *  -wl window_left - corespunde xmin
  *  -wt window_top - corespunde ymin
  *  -wr window_right - corespunde xmax
  *  -wb window_bottom - corespunde ymax


Exemplu de utilizare: **lineout -f in.ps -w 200 -h 200 -wl 50 -wt 50 -wr 150 -wb 150 -o out.xpm**
![conversion example](http://i41.tinypic.com/34q441i.gif)
