# FIRST OF ALL...
- Controllare la versione di sistema operativo.

		lsb_release -d

- Le versioni di Ubuntu dalla 20.04, Python3 è già incluso. Per le versioni precedenti è raccomandato un upgrade.  
Ora verifichiamo la presenza di aggiorniamenti.

		sudo apt update
	
- Effettuiamo gli aggiornamenti.

		sudo apt upgrade
		
- Ora installiamo pip per Python3.

		sudo apt install python3-pip
		
- Ora utilizziamo il comando pip per installare pybind11, un modulo di python che ci permetterà di wrappare classi e funzioni C++ in moduli python.

		pip install pybind11
		
Pybind11 è una libreria che permette appunto il wrapping di code C++ per Python e viceversa. La documentazione originale è possibile trovara [QUI](https://pybind11.readthedocs.io/en/stable/).  
Una volta installato pybind11 sarà possibile compilare la libreria in modo tale da poterla importare in un codice Python. Per eseguire quello presente nel repository:

- Spostarsi nella directory contenente il codice.

		cd PATH/TO/REPO/code/src
		
- Per compilare i wrapper python è sufficiente utilizzare il Makefile già presente all'interno della directory.

		make Queue\_cpp\_int.so
	
	oppure:
	
		make Queue\_cpp\_double.so

- Se non si sono verificati errori, è possibile eseguire il codice python che importa la libreria sia con degli interi che con dei double.

		python3 Python_int.py
		
	oppure:
	
		python3 Python_double.py
		
Per eseguire il codice C++ che testa la libreria è sufficiente:

- Spostarsi nella directory contenente il codice.

		cd PATH/TO/REPO/code/src
	
- Compilare il codice utilizzando il Makefile già presente all'interno della directory.
	
		make
	
- Eseguire il codice.

		./Testing