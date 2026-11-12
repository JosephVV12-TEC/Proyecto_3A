#include "ArbolesHuffman.h"

int main()
{
    int opcion;
    cout << "1. Comprimir\n2. Descomprimir\nOpcion: ";
    cin >> opcion;
    cin.ignore(); // quita el Enter que queda pendiente antes del getline

    char ruta[600];
    if (opcion == 1) cout << "Ruta del archivo a comprimir: ";
    else             cout << "Ruta del archivo .cmp a descomprimir: ";
    cin.getline(ruta, 600);
    quitarComillas(ruta);

    if (opcion == 1) comprimir(ruta);
    else if (opcion == 2) descomprimir(ruta);
    else cout << "Opcion invalida.\n";
    return 0;
}