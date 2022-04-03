ISTRUZIONI PER LA COMPILAZIONE:

Per lanciare l'algoritmo, basta semplicemente compilare con g++ con la seguente riga di codice:
g++ -o main *.cpp

Per lanciare il codice invece:
.\main

ATTENZIONE!!!!!!
1) Per il corretto funzionamento del programma la cartella "inputdatabase" deve essere inserita così com'è nella cartella con i sorgente,
così come è stato fatto nella cartella contenete questo file.
2) Disabilitare l'antivirus. Con l'antivirus attivo la stampa del file non avviene correttamente.


MODIFICA DEL FILE DI INPUT:

Per modificare il numero del file di input, basta avviare il programma e digitare il numero del file desiderato a schermo quando richiesto.


MODIFICA DIMENSIONE PACCHETTO:

E' possibile inoltre modificare il numero di samples in ogni pacchetto modificando il valore di packet_size in VadProject.cpp (riga 10):
const int packet_size = <inserisci numero di samples per pacchetto>