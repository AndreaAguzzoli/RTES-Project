# FIRST OF ALL...
- Controllare la versione di sistema operativo.

		$ lsb_release -d

- Le versioni di Ubuntu dalla 20.04, Python3 è già incluso. Per le versioni precedenti è raccomandato un upgrade.  
Ora verifichiamo la presenza di aggiorniamenti.

		$ sudo apt update
	
- Effettuiamo gli aggiornamenti.

		$ sudo apt upgrade
		
- Ora installiamo pip per Python3.

		$ sudo apt install python3-pip
		
- Ora utilizziamo il comando pip per installare pybind11, un modulo di python che ci permetterà di wrappare classi e funzioni C++ in moduli python.

		$ pip install pybind11
		
Pybind11 è una libreria che permette appunto il wrapping di code C++ per Python e viceversa. La documentazione originale è possibile trovara [QUI](https://pybind11.readthedocs.io/en/stable/).  
Una volta installato pybind11 sarà possibile compilare la libreria in modo tale da poterla importare in un codice Python. Per eseguire quello presente nel repository:

- Spostarsi nella cartella contenente il codice.

		$ cd PATH/TO/REPO/code/src
		
- Dare il seguente comando per effettuare il wrapping.

		$ g++ -shared -fPIC -std=c++11 -I./pybind11/include/ `python3.8 -m pybind11 --includes` pywrap.cpp -o Queue_cpp.so `python3.8-config --ldflags`
	
- Se non si sono verificati errori, è possibile eseguire il codice python che importa la libreria.

		$ python3 Python_Testing.py
		
Per eseguire il codice C++ che testa la libreria è sufficiente:

- Spostarsi nella castella contenente il codice.

		$ cd PATH/TO/REPO/code/src
	
- Compilare il codice (è presente un makefile).
	
		$ make
	
- Eseguire il codice.

		$ ./Testing