#include "ArbolesHuffman.h"

int main() 
{
    char ruta[600];
    cout << "Ruta del archivo a comprimir: ";
    cin.getline(ruta, 600);
    quitarComillas(ruta);
    comprimir(ruta);
    return 0;
}