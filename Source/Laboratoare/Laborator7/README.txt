Laborator 7 SPG - Nicolescu Radu-Catalin, 343C4

Am implementat filtrele cerute in laborator: medie,median,gaussian atat pe CPU, cat si pe GPU.

Cum se modifica filtrele:

tasta E -> comutare intre implementare pe CPU si implementare pe GPU
tasta 1 -> imaginea normala, fara filtre
tasta 2 -> imaginea greyscale
tasta 3 -> imaginea cu medie
tasta 4 -> imaginea cu gauss
tasta 5 -> imaginea cu median

Observatie: imaginea cu filtrul median va dura aproximativ 20 secunde la afisare pe CPU, din cauza sortarilor secventiale facute.

La toate cele 3 filtre am ignorat pixelii de pe frontiera, nemodificandu-i (cand X=0, X=WIDTH-1,Y=0,Y=HEIGHT-1)

Medie -> fac media celor 8 pixeli vecini si o pun in pixelul curent
Gaussian -> folosesc convolutia 3x3 cu (1,2,1,2,4,2,1,2,1)/16 si salvez in pixelul curent
Median -> gasesc valoarea mediana a celor 9 elemente (toti vecinii+pixel curent) introducand acesti pixeli intr-un vector si sortandu-l crescator
Mediana va fi mereu pixelul 4. Acesta va fi memorat in pixelul curent.