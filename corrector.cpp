/*****************************************************************************************************************
	UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
	FACULTAD DE ESTUDIOS SUPERIORES -ARAGON-

	Computadoras y programacion. 
	
	323261070 Hernandez Romero Diego
	
	Corrector Ortografico - Distancia de Levenstein
	
******************************************************************************************************************/

#include <iostream>
#include <fstream>
#include <string.h>
#include <cctype>
#include "corrector.h"
using namespace std;

const char LETRAS[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    (char)0xF1, 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    (char)0xE1, (char)0xE9, (char)0xED, (char)0xF3, (char)0xFA, '\0'
};
const int TOTAL_LETRAS = 32;

void convertirMinusculas(char* texto) {
    for (int i = 0; texto[i] != '\0'; i++) {
        texto[i] = tolower(texto[i]);
    }
}

bool esSeparador(char caracter) {
    return (caracter == ' ' || caracter == '\t' || caracter == '\n' || 
            caracter == '\r' || caracter == ',' || caracter == '.' || 
            caracter == ';' || caracter == '(' || caracter == ')');
}

void limpiarPuntuacion(char* texto) {
    int longitud = strlen(texto);
    while (longitud > 0 && (texto[longitud-1] == ',' || texto[longitud-1] == '.' || 
           texto[longitud-1] == ';' || texto[longitud-1] == '(' || texto[longitud-1] == ')')) {
        texto[longitud-1] = '\0';
        longitud--;
    }
}

int buscarPalabra(char lista[][TAMTOKEN], int total, const char* palabra) {
    int inicio = 0, fin = total - 1;
    
    while (inicio <= fin) {
        int mitad = (inicio + fin) / 2;
        int comparacion = strcmp(lista[mitad], palabra);
        
        if (comparacion == 0) return mitad;
        if (comparacion < 0) inicio = mitad + 1;
        else fin = mitad - 1;
    }
    return -1;
}

void agregarPalabraOrdenada(char lista[][TAMTOKEN], int frecuencias[], 
                            int& total, const char* palabra) {
    int posicion = 0;
    while (posicion < total && strcmp(lista[posicion], palabra) < 0) {
        posicion++;
    }
    
    for (int i = total; i > posicion; i--) {
        strcpy_s(lista[i], TAMTOKEN, lista[i-1]);
        frecuencias[i] = frecuencias[i-1];
    }
    
    strcpy_s(lista[posicion], TAMTOKEN, palabra);
    frecuencias[posicion] = 1;
    total++;
}

// ==================== FUNCION 1: DICCIONARIO ====================
void Diccionario(char* szNombre, char szPalabras[][TAMTOKEN], 
                 int iEstadisticas[], int& iNumElementos) {
    iNumElementos = 0;
    ifstream entrada(szNombre);
    if (!entrada.is_open()) return;
    
    char buffer[TAMTOKEN];
    char caracter;
    int indice = 0;
    
    while (entrada.get(caracter)) {
        if (esSeparador(caracter)) {
            if (indice > 0) {
                buffer[indice] = '\0';
                limpiarPuntuacion(buffer);
                convertirMinusculas(buffer);
                
                if (strlen(buffer) > 0) {
                    int posicion = buscarPalabra(szPalabras, iNumElementos, buffer);
                    
                    if (posicion != -1) {
                        iEstadisticas[posicion]++;
                    } else {
                        agregarPalabraOrdenada(szPalabras, iEstadisticas, iNumElementos, buffer);
                    }
                }
                indice = 0;
            }
        } else {
            if (indice < TAMTOKEN - 1) {
                buffer[indice++] = caracter;
            }
        }
    }
    
    if (indice > 0) {
        buffer[indice] = '\0';
        limpiarPuntuacion(buffer);
        convertirMinusculas(buffer);
        
        if (strlen(buffer) > 0) {
            int posicion = buscarPalabra(szPalabras, iNumElementos, buffer);
            if (posicion != -1) {
                iEstadisticas[posicion]++;
            } else {
                agregarPalabraOrdenada(szPalabras, iEstadisticas, iNumElementos, buffer);
            }
        }
    }
    
    entrada.close();
}

// ==================== FUNCION 2: CLONAR PALABRAS ====================
void ClonaPalabras(char* szPalabraLeida, char szPalabrasSugeridas[][TAMTOKEN], 
                   int& iNumSugeridas) {
    iNumSugeridas = 0;
    int longitud = strlen(szPalabraLeida);
    
    strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, szPalabraLeida);
    
    for (int i = 0; i < longitud; i++) {
        char temporal[TAMTOKEN];
        int idx = 0;
        for (int j = 0; j < longitud; j++) {
            if (j != i) temporal[idx++] = szPalabraLeida[j];
        }
        temporal[idx] = '\0';
        if (strlen(temporal) > 0) {
            strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, temporal);
        }
    }
    
    for (int i = 0; i < longitud - 1; i++) {
        char temporal[TAMTOKEN];
        strcpy_s(temporal, TAMTOKEN, szPalabraLeida);
        char aux = temporal[i];
        temporal[i] = temporal[i+1];
        temporal[i+1] = aux;
        strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, temporal);
    }
    
    for (int i = 0; i < longitud; i++) {
        for (int j = 0; j < TOTAL_LETRAS; j++) {
            char temporal[TAMTOKEN];
            strcpy_s(temporal, TAMTOKEN, szPalabraLeida);
            temporal[i] = LETRAS[j];
            strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, temporal);
        }
    }
    
    for (int i = 0; i <= longitud; i++) {
        for (int j = 0; j < TOTAL_LETRAS; j++) {
            char temporal[TAMTOKEN];
            int idx = 0;
            for (int k = 0; k < i; k++) temporal[idx++] = szPalabraLeida[k];
            temporal[idx++] = LETRAS[j];
            for (int k = i; k < longitud; k++) temporal[idx++] = szPalabraLeida[k];
            temporal[idx] = '\0';
            strcpy_s(szPalabrasSugeridas[iNumSugeridas++], TAMTOKEN, temporal);
        }
    }
    
    for (int i = 0; i < iNumSugeridas - 1; i++) {
        for (int j = 0; j < iNumSugeridas - i - 1; j++) {
            if (strcmp(szPalabrasSugeridas[j], szPalabrasSugeridas[j+1]) > 0) {
                char temporal[TAMTOKEN];
                strcpy_s(temporal, TAMTOKEN, szPalabrasSugeridas[j]);
                strcpy_s(szPalabrasSugeridas[j], TAMTOKEN, szPalabrasSugeridas[j+1]);
                strcpy_s(szPalabrasSugeridas[j+1], TAMTOKEN, temporal);
            }
        }
    }
}

// ==================== FUNCION 3: LISTA CANDIDATAS ====================
void ListaCandidatas(char szPalabrasSugeridas[][TAMTOKEN], int iNumSugeridas,
                     char szPalabras[][TAMTOKEN], int iEstadisticas[], int iNumElementos,
                     char szListaFinal[][TAMTOKEN], int iPeso[], int& iNumLista) {
    iNumLista = 0;
    
    for (int i = 0; i < iNumSugeridas; i++) {
        int posicion = buscarPalabra(szPalabras, iNumElementos, szPalabrasSugeridas[i]);
        
        if (posicion != -1) {
            bool duplicado = false;
            for (int j = 0; j < iNumLista; j++) {
                if (strcmp(szListaFinal[j], szPalabras[posicion]) == 0) {
                    duplicado = true;
                    break;
                }
            }
            
            if (!duplicado) {
                strcpy_s(szListaFinal[iNumLista], TAMTOKEN, szPalabras[posicion]);
                iPeso[iNumLista] = iEstadisticas[posicion];
                iNumLista++;
            }
        }
    }
    
    for (int i = 0; i < iNumLista - 1; i++) {
        for (int j = 0; j < iNumLista - i - 1; j++) {
            if (iPeso[j] < iPeso[j+1]) {
                int tempPeso = iPeso[j];
                iPeso[j] = iPeso[j+1];
                iPeso[j+1] = tempPeso;
                
                char tempPalabra[TAMTOKEN];
                strcpy_s(tempPalabra, TAMTOKEN, szListaFinal[j]);
                strcpy_s(szListaFinal[j], TAMTOKEN, szListaFinal[j+1]);
                strcpy_s(szListaFinal[j+1], TAMTOKEN, tempPalabra);
            }
        }
    }
}

// ==================== PROGRAMA PRINCIPAL ====================
int main() {
    char diccionario[NUMPALABRAS][TAMTOKEN];
    int frecuencias[NUMPALABRAS];
    int totalPalabras = 0;
    
    cout << "=== CORRECTOR ORTOGRAFICO ===" << endl;
    cout << "Archivo diccionario: ";
    char nombreArchivo[100];
    cin >> nombreArchivo;
    
    Diccionario(nombreArchivo, diccionario, frecuencias, totalPalabras);
    
    if (totalPalabras == 0) {
        cout << "Error al cargar diccionario." << endl;
        return 1;
    }
    
    cout << "Cargadas " << totalPalabras << " palabras" << endl;
    
    char entrada[TAMTOKEN];
    while (true) {
        cout << "\nPalabra ('salir' para terminar): ";
        cin >> entrada;
        
        if (strcmp(entrada, "salir") == 0) break;
        
        convertirMinusculas(entrada);
        int posicion = buscarPalabra(diccionario, totalPalabras, entrada);
        
        if (posicion != -1) {
            cout << "Correcta" << endl;
        } else {
            cout << "Incorrecta - Buscando alternativas..." << endl;
            
            char variaciones[5000][TAMTOKEN];
            int totalVariaciones = 0;
            ClonaPalabras(entrada, variaciones, totalVariaciones);
            
            char candidatas[1000][TAMTOKEN];
            int pesos[1000];
            int totalCandidatas = 0;
            ListaCandidatas(variaciones, totalVariaciones, diccionario, 
                           frecuencias, totalPalabras, candidatas, pesos, totalCandidatas);
            
            if (totalCandidatas > 0) {
                cout << "\nSugerencias:" << endl;
                int limite = (totalCandidatas < 10) ? totalCandidatas : 10;
                for (int i = 0; i < limite; i++) {
                    cout << "  " << i+1 << ". " << candidatas[i] 
                         << " (" << pesos[i] << ")" << endl;
                }
            } else {
                cout << "Sin sugerencias disponibles." << endl;
            }
        }
    }
    
    cout << "\nFin del programa." << endl;
    return 0;
}
