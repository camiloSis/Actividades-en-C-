/*
 * EJERCICIO 1 - Kruskal con Union-Find (compresión de caminos + unión por rango)
 * Problema: diseñar la red de cableado de MENOR COSTO que conecte todos los
 *           edificios ingresados por el usuario.
 *
 * Compilar: g++ -O2 -std=c++17 ejercicio1_kruskal.cpp -o ejercicio1
 * Ejecutar: ./ejercicio1
 *
 * Formato de entrada (si se elige ingresar datos):
 *   n                      -> cantidad de edificios
 *   nombre_1 ... nombre_n  -> nombres (sin espacios)
 *   m                      -> cantidad de tendidos posibles
 *   A B costo   (m veces)  -> se puede tender cable entre A y B con ese costo
 */
#include <bits/stdc++.h>
using namespace std;

// Una arista = un posible tendido de cable entre dos edificios (u, v) con costo w.
struct Arista {
    int u, v;
    long long w;
};

// ---------------------------------------------------------------------------
// Estructura Union-Find (Conjuntos Disjuntos)
// Cada conjunto representa una "isla" de edificios ya conectados entre sí.
// ---------------------------------------------------------------------------
struct UnionFind {
    vector<int> padre;   // padre[x] = padre de x en el bosque (la raíz se apunta a sí misma)
    vector<int> rango;   // cota superior de la altura del árbol (para unión por rango)
    int componentes;     // cuántas islas distintas quedan

    UnionFind(int n) : padre(n), rango(n, 0), componentes(n) {
        iota(padre.begin(), padre.end(), 0);  // al inicio cada edificio es su propia isla
    }

    // buscar(x): devuelve el representante (raíz) del conjunto de x.
    // COMPRESIÓN DE CAMINOS: al volver de la recursión, cada nodo visitado pasa a
    // apuntar directamente a la raíz, así las próximas búsquedas son casi O(1).
    int buscar(int x) {
        if (padre[x] != x)
            padre[x] = buscar(padre[x]);   // <- aquí ocurre la compresión
        return padre[x];
    }

    // unir(a,b): fusiona las islas de a y b. Devuelve false si YA estaban unidas
    // (es decir, la arista formaría un ciclo).
    bool unir(int a, int b) {
        a = buscar(a);
        b = buscar(b);
        if (a == b) return false;                 // misma isla -> ciclo
        if (rango[a] < rango[b]) swap(a, b);      // el árbol más alto será la raíz
        padre[b] = a;                             // cuelga el árbol más bajo del más alto
        if (rango[a] == rango[b]) rango[a]++;     // solo crece la altura si eran iguales
        componentes--;
        return true;
    }
};

// Datos de ejemplo para no tener que escribir todo a mano.
const char* EJEMPLO =
    "6\n"
    "Rectorado Biblioteca Laboratorio Cafeteria Gimnasio Auditorio\n"
    "9\n"
    "Rectorado Biblioteca 4\n"
    "Rectorado Cafeteria 8\n"
    "Biblioteca Laboratorio 8\n"
    "Biblioteca Cafeteria 11\n"
    "Laboratorio Gimnasio 7\n"
    "Cafeteria Gimnasio 1\n"
    "Cafeteria Auditorio 2\n"
    "Gimnasio Auditorio 6\n"
    "Laboratorio Auditorio 4\n";

int main() {
    // --- 1. Elegir fuente de datos -----------------------------------------
    cout << "Usar datos de ejemplo? (s/n): ";
    char op; cin >> op;
    istringstream ejemplo(EJEMPLO);
    istream& in = (op == 's' || op == 'S') ? static_cast<istream&>(ejemplo) : cin;
    if (op != 's' && op != 'S')
        cout << "Ingrese n, luego los n nombres, luego m y luego m lineas 'A B costo':\n";

    // --- 2. Leer edificios y tendidos --------------------------------------
    int n, m;
    in >> n;
    vector<string> nombre(n);
    unordered_map<string, int> id;           // nombre -> índice 0..n-1
    for (int i = 0; i < n; i++) { in >> nombre[i]; id[nombre[i]] = i; }

    in >> m;
    vector<Arista> aristas;
    for (int i = 0; i < m; i++) {
        string a, b; long long c;
        in >> a >> b >> c;
        if (!id.count(a) || !id.count(b) || c < 0) {   // validación básica
            cout << "Tendido invalido ignorado: " << a << " " << b << " " << c << "\n";
            continue;
        }
        if (id[a] == id[b]) continue;                  // un lazo nunca sirve
        aristas.push_back({id[a], id[b], c});
    }

    // --- 3. KRUSKAL ---------------------------------------------------------
    // Idea voraz: tomar siempre el tendido más barato que NO cierre un ciclo.
    sort(aristas.begin(), aristas.end(),
         [](const Arista& x, const Arista& y) { return x.w < y.w; });  // O(E log E)

    UnionFind uf(n);
    vector<Arista> elegidas;
    long long costoTotal = 0;

    cout << "\n--- Recorrido de Kruskal (aristas por costo creciente) ---\n";
    for (const Arista& e : aristas) {
        if (uf.unir(e.u, e.v)) {                       // une dos islas distintas
            elegidas.push_back(e);
            costoTotal += e.w;
            cout << "ACEPTADA  " << nombre[e.u] << " - " << nombre[e.v]
                 << " (costo " << e.w << ")\n";
            if ((int)elegidas.size() == n - 1) break;  // árbol completo: n-1 aristas
        } else {
            cout << "rechazada " << nombre[e.u] << " - " << nombre[e.v]
                 << " (costo " << e.w << ") -> formaria un ciclo\n";
        }
    }

    // --- 4. Resultado -------------------------------------------------------
    cout << "\n=== RED DE CABLEADO DE MENOR COSTO ===\n";
    for (const Arista& e : elegidas)
        cout << nombre[e.u] << " <-> " << nombre[e.v] << "  costo " << e.w << "\n";
    cout << "Costo total: " << costoTotal << "\n";

    if (uf.componentes > 1)
        cout << "AVISO: los tendidos dados no conectan todos los edificios; "
             << "quedan " << uf.componentes << " grupos separados.\n";
    return 0;
}
