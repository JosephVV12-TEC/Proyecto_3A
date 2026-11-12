#pragma once
#include <iostream>
#include <cstdio>
using namespace std;



struct NodoA
{
    int  frecuencia; // cuántas veces aparece (o la suma de sus hijos)
    int   byte;
    NodoA* HijoIzquierdo;//El bit en 0
    NodoA* HijoDerecho;//El bit en 1
};

NodoA almacen[512]; //Se pone memoria solo para 512 nodos, ya que el árbol de Huffman para 256 bytes puede tener como máximo 511 nodos
int  cantidadNodos = 0;
char codigos[256][256];//aca van los 0 y 1 y se puso hasta 256 porque hay 256 posibilidades de posiciones en las que esten los bytes
int  largoCodigo[256];//cuantos bits tiene el codigo de cada byte

//Funciones que ya hemos visto
unsigned char encenderBit(unsigned char b, int pos)
{
    return b | (1 << pos);
}

unsigned char apagarBit(unsigned char b, int pos)
{
    return b & ~(1 << pos);
}


unsigned char invertirBit(unsigned char b, int pos)
{
    return b ^ (1 << pos);
}


int leerBit(unsigned char b, int pos)
{
    if ((b & (1 << pos)) != 0) return 1;
    return 0;
}


void imprimirBinario(unsigned char b)
{
    for (int pos = 7; pos >= 0; pos--)
        cout << leerBit(b, pos);
}


//Estructuras para escribir los bits en un archivo (sirve para la compresion)
struct EscritorBits
{
    FILE* salida;// archivo donde se escribe
    unsigned char buffer;// byte que se esta armando
    int bitsUsados;// cuantos bits del buffer ya se llenaron
    int bytesEscritos;// cuantos bytes se han escrito al archivo
};


void escribirBit(EscritorBits* e, int bit)
{
    if (bit == 1)
        e->buffer = encenderBit(e->buffer, 7 - e->bitsUsados);   // se llena de izquierda a derecha
    e->bitsUsados++;

    if (e->bitsUsados == 8)//Si ya se llenó el byte, lo escribimos al archivo y reiniciamos el buffer
    {
        fputc(e->buffer, e->salida);
        e->bytesEscritos++;
        e->buffer = 0;
        e->bitsUsados = 0;
    }
}


void terminarEscritura(EscritorBits* e)//Si al terminar de escribir los bits, el buffer no está vacío, lo escribimos al archivo y reiniciamos el buffer
{
    if (e->bitsUsados > 0)
    {
        fputc(e->buffer, e->salida);
        e->bytesEscritos++;
        e->buffer = 0;
        e->bitsUsados = 0;
    }
}


//Funciones para leer los bits
int contarFrecuencias(const char* nombreArchivo, int frecuencia[256]) //Lo que hace es contar la frecuencia (o mas simple, cuantas veces aparece un caracter) de cada byte en el archivo y guardarlo en el arreglo frecuencia. Devuelve el total de bytes leídos.
{
    FILE* f;
    if (fopen_s(&f, nombreArchivo, "rb") != 0) return -1;
	for (int i = 0; i < 256; i++) frecuencia[i] = 0; //Pone la frecuencia de todos los bytes en 0
	int total = 0; //Variable para contar el total de bytes leídos
	int c; //Esta leerá cada byte del archivo
    while ((c = fgetc(f)) != EOF) //Lee cada byte del archivo hasta llegar al final del archivo
    {
		frecuencia[c]++; //Incrementa la frecuencia del byte leído
		total++; //Incrementa el total de bytes leídos
    }
    fclose(f);
    return total;
}

NodoA* nuevoNodo(int f, int b, NodoA* i, NodoA* d) //Crea un nuevo nodo con la frecuencia f, el byte b, el hijo izquierdo i y el hijo derecho d
{
    NodoA* n = &almacen[cantidadNodos];
    cantidadNodos++;
    n->frecuencia = f;
    n->byte = b;
    n->HijoIzquierdo = i;
    n->HijoDerecho = d;
    return n;
}

int buscarMenor(NodoA* lista[], int n) //Esto busca el nodo con la menor frecuencia en la lista de nodos y devuelve su índice esto es para el arbol de huffman. el menor abajo a la derecha graficamente
{
    int menor = 0;
    for (int i = 1; i < n; i++)
        if (lista[i]->frecuencia < lista[menor]->frecuencia)
            menor = i;
    return menor;
}

NodoA* construirArbol(int frecuencia[256]) //Y ahora construye el árbol de Huffman a partir de las frecuencias de los bytes. Devuelve la raíz del árbol.
{
    NodoA* lista[256];
    int n = 0;
    cantidadNodos = 0;
    for (int i = 0; i < 256; i++)
        if (frecuencia[i] > 0)
            lista[n++] = nuevoNodo(frecuencia[i], i, NULL, NULL);
    if (n == 0) return NULL;
    if (n == 1)
        return nuevoNodo(lista[0]->frecuencia, -1, lista[0], NULL);
    while (n > 1) {
        int a = buscarMenor(lista, n);
        NodoA* x = lista[a];
        lista[a] = lista[n - 1];
        n--;
        int b = buscarMenor(lista, n);
        NodoA* y = lista[b];
        lista[b] = lista[n - 1];
        n--;
        lista[n++] = nuevoNodo(x->frecuencia + y->frecuencia, -1, x, y);
    }
    return lista[0];
}

void imprimirHojas(NodoA* raiz, int profundidad) //Imprime las hojas del árbol de Huffman, es decir, los bytes y su profundidad en el árbol. La profundidad es la cantidad de bits que se necesitan para hacer ese byte (como por ejemplo 11110)
{
    if (raiz == NULL) return;
    if (raiz->HijoIzquierdo == NULL && raiz->HijoDerecho == NULL) {
        cout << "byte " << raiz->byte << " profundidad " << profundidad << endl;
        return;
    }
    imprimirHojas(raiz->HijoIzquierdo, profundidad + 1);
    imprimirHojas(raiz->HijoDerecho, profundidad + 1);
}

void generarCodigos(NodoA* nodo, char camino[], int profundidad) //Este ya genera los codigos Huffman para cada byte
{
    if (nodo == NULL) return;

    // Si es una hoja: el camino recorrido es el codigo de ese byte
    if (nodo->HijoIzquierdo == NULL && nodo->HijoDerecho == NULL)
    {
        for (int i = 0; i < profundidad; i++)
            codigos[nodo->byte][i] = camino[i];
        largoCodigo[nodo->byte] = profundidad;
        return;
    }

    camino[profundidad] = '0';
    generarCodigos(nodo->HijoIzquierdo, camino, profundidad + 1);

    camino[profundidad] = '1';
    generarCodigos(nodo->HijoDerecho, camino, profundidad + 1);
}

void quitarComillas(char* ruta) //Esta funcion lo que va a hacer es quitar las comillas del nombre del arvhico para que si quiero poner cualquier nombre de archivo con espacios no me de error
{
    int n = 0;
    while (ruta[n] != '\0') n++;

    if (n >= 2 && ruta[0] == '"' && ruta[n - 1] == '"')
    {
        for (int i = 0; i < n - 2; i++)
            ruta[i] = ruta[i + 1];
        ruta[n - 2] = '\0';
    }
}

//Las funciones de abajo son para modificar el nombre de los archivos y que queden como ArchivoX.ext a ArchivoX.cmp o de ArchivoX.ext a ArchivoX-descomprimido.ext
int largoTexto(const char* t)
{
    int n = 0;
    while (t[n] != '\0') n++;
    return n;
}

// Devuelve la posicion de la ULTIMA vez que aparece el caracter c (o -1 si no aparece)
int ultimaPosicion(const char* t, char c)
{
    int pos = -1;
    for (int i = 0; t[i] != '\0'; i++)
        if (t[i] == c) pos = i;
    return pos;
}

// Pega al final de 'destino' las primeras 'cantidad' letras de 'origen'
void concatenar(char* destino, const char* origen, int cantidad)
{
    int n = largoTexto(destino);
    for (int i = 0; i < cantidad; i++)
        destino[n + i] = origen[i];
    destino[n + cantidad] = '\0';
}

// Posicion donde empieza el nombre del archivo (despues de la ultima \ o /)
int inicioNombre(const char* ruta)
{
    int a = ultimaPosicion(ruta, '\\');
    int b = ultimaPosicion(ruta, '/');
    if (b > a) a = b;
    return a + 1;
}

//aca estas dos funciones van a escribir y leer numeros como bytes para comprimirlos
void escribirNumero(FILE* f, int valor, int cantBytes)
{
    for (int i = 0; i < cantBytes; i++)
        fputc((valor >> (8 * i)) & 0xFF, f);
}


int leerNumero(FILE* f, int cantBytes)//La funcion lee un numero y devuelve su valor o sea que si escribo un numero de 4 bytes, lo lee y devuelve el valor del numero
{
    int valor = 0;
    for (int i = 0; i < cantBytes; i++)
    {
        int b = fgetc(f);
        valor = valor | (b << (8 * i));
    }
    return valor;
}

void escribirByteComoBits(EscritorBits* e, int valor) //se pasa de bytes a bits basicamente
{
    for (int pos = 7; pos >= 0; pos--)
        escribirBit(e, leerBit((unsigned char)valor, pos));
}


void guardarArbol(NodoA* nodo, EscritorBits* e) //Esto se inspiro de la fuente, lo que hace es guardar el arbol de huffman en el archivo comprimido, para que despues se pueda descomprimir
{
    if (nodo == NULL)
    {
        escribirBit(e, 1);
        escribirByteComoBits(e, 0);
        return;
    }
    if (nodo->HijoIzquierdo == NULL && nodo->HijoDerecho == NULL)
    {
        escribirBit(e, 1);
        escribirByteComoBits(e, nodo->byte);
        return;
    }
    escribirBit(e, 0);
    guardarArbol(nodo->HijoIzquierdo, e);
    guardarArbol(nodo->HijoDerecho, e);
}


bool comprimir(const char* rutaEntrada) //Aca la funcion ya comprime todo y le pone el nombrecito
{
    //Paso 1, cuenta las frecuencias de cada byte en el archivo original y las guarda en un arreglo frecuenciay devuelve el total de bytes leídos
    int frecuencia[256];
    int total = contarFrecuencias(rutaEntrada, frecuencia);
    if (total < 0) { cout << "No se pudo abrir el archivo.\n"; return false; }

    //El paso 2 seria todo el tema de los nombres de los archivos, aca se hace para que el archivo comprimido tenga el mismo nombre que el original pero con extension .cmp
    const char* nombre = rutaEntrada + inicioNombre(rutaEntrada);
    int largoNombre = largoTexto(nombre);
    if (largoNombre > 255) { cout << "El nombre del archivo es muy largo.\n"; return false; }

    char rutaSalida[600];
    rutaSalida[0] = '\0';
    concatenar(rutaSalida, rutaEntrada, inicioNombre(rutaEntrada));//Esto es para que la ruta de salida sea igual a la ruta de entrada, pero sin el nombre del archivo, o sea que si el archivo esta en C:\Archivos\Archivo.txt, la ruta de salida va a ser C:\Archivos
    int punto = ultimaPosicion(nombre, '.');
    int largoBase = (punto > 0) ? punto : largoNombre;
	concatenar(rutaSalida, nombre, largoBase);//Este es el nombre del archivo comprimido sin la extension
	concatenar(rutaSalida, ".cmp", 4);//Este es el nombre del archivo comprimido

	//El paso 3 es construir el arbol de huffman y generar los codigos de cada byte
    NodoA* raiz = construirArbol(frecuencia);
    for (int i = 0; i < 256; i++) largoCodigo[i] = 0;
    char camino[256];
    generarCodigos(raiz, camino, 0);

	//Luego aca se hace el 4 paso que es hacer un checksum (el cual esun numero de 1 solo byte que sirve para que despues, cuando se va a descomprimir, se pueda verificar que el archivo comprimido no se haya dañado)
    //El checksum se calcula haciendo un XOR de todos los bytes que tienen una frecuencia impar porque si un byte aparece un numero impar de veces, al descomprimirlo va a aparecer un numero impar de veces tambien, y eso es lo que se quiere verificar
    unsigned char checksum = 0;
    for (int i = 0; i < 256; i++)
        if ((frecuencia[i] & 1) == 1)
            checksum = checksum ^ (unsigned char)i;

    //Paso 5, se crea el archivo de salida y se escribe el header del archivo comprimido
    FILE* salida;
    if (fopen_s(&salida, rutaSalida, "wb") != 0) { cout << "No se pudo crear el archivo de salida.\n"; return false; }

    fputc(largoNombre, salida);
    for (int i = 0; i < largoNombre; i++) fputc(nombre[i], salida);
    escribirNumero(salida, total, 4);
    fputc(checksum, salida);

	//Paso 6, se escribe el arbol y el header del archivo comprimido, y luego se escriben los bytes del archivo original convertidos a bits usando los codigos de huffman
    EscritorBits esc;
    esc.salida = salida;
    esc.buffer = 0;
    esc.bitsUsados = 0;
    esc.bytesEscritos = 0;

    if (raiz != NULL) guardarArbol(raiz, &esc);
    
    FILE* entrada;
    fopen_s(&entrada, rutaEntrada, "rb");
    int c;
    while ((c = fgetc(entrada)) != EOF)// cada byte original se cambia por su codigo
        for (int i = 0; i < largoCodigo[c]; i++)
            escribirBit(&esc, (codigos[c][i] == '1') ? 1 : 0);
    terminarEscritura(&esc);
    fclose(entrada);

	int tamanoCmp = ftell(salida);// tamano final del .cmp (eso es para mostrarlo al usuario)
    fclose(salida);

    //Y esto na mas es para informar que se comprimio
    cout << "Archivo comprimido: " << rutaSalida << endl;
    cout << "Tamano original : " << total << " bytes\n";
    cout << "Tamano comprimido: " << tamanoCmp << " bytes\n";
    return true;
}


//DE ACA EN ADELANTE ES CODIGO DEL DESCOMPRESOR SOLAMENTE

struct LectorBits//Es como la hermana de EscritorBits: en vez de armar un byte bit por bit, lo va desarmando
{
    FILE* entrada;// archivo del que se lee
    unsigned char buffer;// byte que se esta leyendo
    int bitsLeidos;// cuantos bits del buffer ya se usaron (8 = hay que leer otro byte)
    bool error;// true si el archivo se acabo antes de tiempo
};

int leerBitArchivo(LectorBits* l) //Devuelve el siguiente bit (de izquierda a derecha, igual que escribirBit)
{
    if (l->bitsLeidos == 8)
    {
        int c = fgetc(l->entrada);
        if (c == EOF) { l->error = true; return 0; }
        l->buffer = (unsigned char)c;
        l->bitsLeidos = 0;
    }
    int bit = leerBit(l->buffer, 7 - l->bitsLeidos); //Esto basicamente dice que 
    l->bitsLeidos++;
    return bit;
}

int leerByteDeBits(LectorBits* l)//Es lo contrario de escribirByteComoBits, junta 8 bits y arma un byte
{
    int valor = 0;
    for (int i = 0; i < 8; i++)
        valor = (valor << 1) | leerBitArchivo(l);
    return valor;
}

NodoA* leerArbol(LectorBits* l)//Este es el contrario a guardarArbol, lee el arbol de huffman del archivo comprimido y lo reconstruye
{
    if (l->error || cantidadNodos >= 512) 
    { 
        l->error = true; return NULL; 
    }

    if (leerBitArchivo(l) == 1) //bit 1 = hoja (a hoja se refiere al nodo que no tiene hijos por si acaso), los siguientes 8 bits son el byte
    {
        int b = leerByteDeBits(l);
        return nuevoNodo(0, b, NULL, NULL);
    }
    NodoA* izq = leerArbol(l);
    NodoA* der = leerArbol(l);
    if (l->error || cantidadNodos >= 512) { l->error = true; return NULL; }
    return nuevoNodo(0, -1, izq, der);
}

bool descomprimir(const char* rutaEntrada) //Para descompresor es muy parecido a como se hace graficamente o a mano, se lee el header, se arma el arbol y se leen los bits
{
    FILE* entrada;
    if (fopen_s(&entrada, rutaEntrada, "rb") != 0) { cout << "No se pudo abrir el archivo.\n"; return false; }

    //Paso 1 es leer el header del archivo comprimido
    int largoNombre = fgetc(entrada);
    if (largoNombre == EOF || largoNombre == 0) { cout << "Archivo .cmp invalido.\n"; fclose(entrada); return false; }

    char nombre[256];
    for (int i = 0; i < largoNombre; i++)
    {
        int c = fgetc(entrada);
        if (c == EOF) { cout << "Archivo .cmp incompleto.\n"; fclose(entrada); return false; }
        nombre[i] = (char)c;
    }
    nombre[largoNombre] = '\0';

    int total = leerNumero(entrada, 4);
    int checksumGuardado = fgetc(entrada);
    if (checksumGuardado == EOF) { cout << "Archivo .cmp incompleto.\n"; fclose(entrada); return false; }

    //Paso 2 es armar el bombre del archivo con concatenar (esto es lo unico que no se parece a lo grafico)
    int punto = ultimaPosicion(nombre, '.');
    int largoBase = (punto > 0) ? punto : largoNombre;

    char rutaSalida[600];
    rutaSalida[0] = '\0';
    concatenar(rutaSalida, rutaEntrada, inicioNombre(rutaEntrada)); // la carpeta
    concatenar(rutaSalida, nombre, largoBase);
    concatenar(rutaSalida, "-descomprimido", 14);
    if (punto > 0)
        concatenar(rutaSalida, nombre + punto, largoNombre - punto);// la extension original (con el punto)

	//El paso 3 es leer el arbol de huffman del archivo comprimido y reconstruirlo
    LectorBits lector;
    lector.entrada = entrada;
    lector.buffer = 0;
    lector.bitsLeidos = 8;// 8 para que el primer leerBitArchivo lea un byte nuevo
    lector.error = false;

    cantidadNodos = 0;
    NodoA* raiz = NULL;
    if (total > 0) raiz = leerArbol(&lector);//si el archivo estaba vacio no se guardo ningun arbol

	//El paso 4 es leer los bits del archivo comprimido y reconstruir el archivo original usando el arbol de huffman
    FILE* salida;
    if (fopen_s(&salida, rutaSalida, "wb") != 0) { cout << "No se pudo crear el archivo de salida.\n"; fclose(entrada); return false; }

    unsigned char checksum = 0;
    for (int k = 0; k < total && !lector.error; k++)
    {
        NodoA* nodo = raiz;
        while (nodo->HijoIzquierdo != NULL || nodo->HijoDerecho != NULL)
        {
            if (leerBitArchivo(&lector) == 0) nodo = nodo->HijoIzquierdo;
            else nodo = nodo->HijoDerecho;
        }
        if (lector.error) break;
        fputc(nodo->byte, salida);
        checksum = checksum ^ (unsigned char)nodo->byte;
    }
    fclose(salida);
    fclose(entrada);

    //El paso 5 seroa verificar con el checksum del header (el que hicimos en lo de comprimir)
    if (lector.error)
    {
        cout << "El archivo .cmp esta incompleto o corrupto.\n";
        return false;
    }
    cout << "Archivo descomprimido: " << rutaSalida << endl;
    if (checksum == (unsigned char)checksumGuardado)
        cout << "Checksum correcto: el archivo quedo igual al original.\n";
    else
        cout << "ADVERTENCIA: el checksum no coincide, el archivo puede estar danado.\n";
    return true;
}
