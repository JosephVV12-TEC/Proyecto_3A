# Proyecto_3A
HOLA GENTE las fuentes por ahora son:
https://github.com/ekcbw/cxx-huffman-simple/blob/main/huffman.cpp
http://www.zator.com/Cpp/E4_9_3.htm 

COMPRESOR / DESCOMPRESOR DE ARCHIVOS (HUFFMAN)

Integrantes:Maria Luisa Lalyre Castillo/ Luis Josue Rosales Castillo / Joseph David Vargas Villalta


QUE ES ESTO
-----------
Es un programa de consola hecho en C++ que comprime y descomprime
archivos usando arboles de Huffman. Funciona con cualquier tipo de
archivo (txt, imagenes, etc.) porque trabaja byte por byte.


ARCHIVOS DEL PROYECTO
---------------------
- Compresor.cpp      -> el main, muestra el menu y pide la ruta.
- ArbolesHuffman.h   -> todo el codigo de comprimir y descomprimir.


COMO CORRERLO
-------------
1. Abrir el proyecto en Visual Studio (lleva fopen_s, asi que es para
   Visual Studio / Windows).
2. Compilar y ejecutar Compresor.cpp.


COMO USARLO
-----------
Al correr sale este menu:

    1. Comprimir
    2. Descomprimir

Para comprimir:
  - Escribir 1 y dar Enter.
  - Pegar la ruta del archivo (puede llevar comillas, el programa las
    quita solo).
  - Se crea ArchivoX.cmp en la misma carpeta del original.

Para descomprimir:
  - Escribir 2 y dar Enter.
  - Pegar la ruta del archivo .cmp.
  - Se crea ArchivoX-descomprimido.ext en la misma carpeta, con la
    extension original.
  - Al final dice si el checksum coincide (o sea, si quedo igual al
    original).


NOMBRES DE LOS ARCHIVOS
-----------------------
  Comprimir:     ArchivoX.ext  ->  ArchivoX.cmp
  Descomprimir:  ArchivoX.cmp  ->  ArchivoX-descomprimido.ext


COMO FUNCIONA 
-----------------------
Comprimir:
  1. Cuenta cuantas veces aparece cada byte.
  2. Arma el arbol de Huffman (se van uniendo los dos nodos de menor
     frecuencia hasta quedar uno solo).
  3. Cada byte recibe un codigo de 0s y 1s (izquierda = 0, derecha = 1).
     Los bytes mas comunes quedan con codigos mas cortos.
  4. Calcula un checksum (XOR de los bytes).
  5. Escribe el .cmp: header + arbol + datos codificados.

El .cmp guarda en orden:
  - 1 byte con el largo del nombre original
  - el nombre original
  - 4 bytes con el tamano original
  - 1 byte de checksum
  - el arbol (en bits)
  - los datos comprimidos (en bits)

Descomprimir:
  1. Lee el header (nombre, tamano y checksum).
  2. Reconstruye el arbol.
  3. Lee los bits y baja por el arbol (0 = izquierda, 1 = derecha)
     hasta llegar a una hoja, y ahi escribe ese byte.
  4. Compara el checksum para verificar que todo salio bien.


COSAS A TENER EN CUENTA
-----------------------
- Archivos muy chiquitos o ya comprimidos (zip, jpg, mp4) pueden
  quedar mas grandes, porque el header y el arbol ocupan espacio.
- Si el archivo de salida ya existe, se sobreescribe sin preguntar.
- El tamano maximo de archivo es de aprox. 2 GB.
- El nombre del archivo no puede pasar de 255 caracteres.
- Si el .cmp esta muy danado puede que no se detecte bien.
- Solo compila en Visual Studio por el uso de fopen_s.


## Imagen del Día

![SOS Dan](fotos/oa.jpg)
