/*
 * EJERCICIO 3 - Algoritmo de Boruvka por rondas
 *
 * En cada ronda imprime qué aristas se agregaron y cuántas componentes quedan.
 * Al final verifica que el resultado coincide con Kruskal y con Prim.
 *
 * Compilar: g++ -O2 -std=c++17 ejercicio3_boruvka.cpp -o ejercicio3
 * Ejecutar: ./ejercicio3
 *
 * Formato de entrada (vértices numerados de 0 a n-1):
 *   n m
 *   u v w   (m veces)
 */
#include <bits/stdc++.h>
using namespace std;

struct Arista { int u, v; long long w; };   // siempre u < v

// Orden total (peso, u, v). ES OBLIGATORIO en Boruvka: si dos aristas empatan en
// peso y cada componente elige una distinta, podrían formarse CICLOS. Con un
// orden estricto la "arista más barata" de cada componente es única y segura.
bool menor(const Arista& a, const Arista& b) {
    if (a.w != b.w) return a.w < b.w;
    if (a.u != b.u) return a.u < b.u;
    return a.v < b.v;
}

struct UnionFind {
    vector<int> padre, rango;
    int componentes;
    UnionFind(int n) : padre(n), rango(n, 0), componentes(n) { iota(padre.begin(), padre.end(), 0); }
    int buscar(int x) { return padre[x] == x ? x : padre[x] = buscar(padre[x]); }  // compresión de caminos
    bool unir(int a, int b) {
        a = buscar(a); b = buscar(b);
        if (a == b) return false;
        if (rango[a] < rango[b]) swap(a, b);
        padre[b] = a;
        if (rango[a] == rango[b]) rango[a]++;
        componentes--;
        return true;
    }
};

long long pesoTotal(const vector<Arista>& v) {
    long long s = 0;
    for (auto& e : v) s += e.w;
    return s;
}

// ------------------------------- BORUVKA -----------------------------------
vector<Arista> boruvka(int n, const vector<Arista>& E) {
    UnionFind uf(n);
    vector<Arista> res;
    int ronda = 0;

    cout << "Componentes iniciales: " << uf.componentes << "\n";
    while (uf.componentes > 1) {
        ronda++;

        // PASO 1: para cada componente, hallar su arista SALIENTE más barata.
        // mejor[r] = índice (en E) de la mejor arista de la componente cuya raíz es r.
        vector<int> mejor(n, -1);
        for (int i = 0; i < (int)E.size(); i++) {
            int a = uf.buscar(E[i].u), b = uf.buscar(E[i].v);
            if (a == b) continue;                                   // arista interna: no sale de la componente
            if (mejor[a] == -1 || menor(E[i], E[mejor[a]])) mejor[a] = i;
            if (mejor[b] == -1 || menor(E[i], E[mejor[b]])) mejor[b] = i;
        }

        // PASO 2: agregar todas esas aristas a la vez (fusionando componentes).
        // La misma arista puede ser elegida por sus dos extremos: unir() devuelve
        // false la segunda vez, así no se duplica ni se crea un ciclo.
        cout << "\n--- Ronda " << ronda << " ---\n";
        int agregadas = 0;
        for (int r = 0; r < n; r++) {
            if (mejor[r] == -1) continue;
            const Arista& e = E[mejor[r]];
            if (uf.unir(e.u, e.v)) {
                res.push_back(e);
                agregadas++;
                cout << "  + arista " << e.u << " - " << e.v << " (peso " << e.w << ")\n";
            }
        }

        cout << "  Aristas agregadas: " << agregadas
             << " | Componentes restantes: " << uf.componentes << "\n";

        // Si ninguna componente tiene arista saliente, el grafo es disconexo: terminamos.
        if (agregadas == 0) {
            cout << "  (No hay mas aristas salientes: el grafo no es conexo)\n";
            break;
        }
    }
    return res;
}

// ------------------------------- KRUSKAL (para verificar) -------------------
vector<Arista> kruskal(int n, vector<Arista> E) {
    sort(E.begin(), E.end(), menor);
    UnionFind uf(n);
    vector<Arista> res;
    for (auto& e : E) if (uf.unir(e.u, e.v)) res.push_back(e);
    return res;
}

// ------------------------------- PRIM (para verificar) ----------------------
vector<Arista> prim(int n, const vector<Arista>& E) {
    // Lista de adyacencia: ady[x] = {(índice de arista, vecino)}
    vector<vector<pair<int,int>>> ady(n);
    for (int i = 0; i < (int)E.size(); i++) {
        ady[E[i].u].push_back({i, E[i].v});
        ady[E[i].v].push_back({i, E[i].u});
    }
    // Min-heap por el mismo orden total de aristas.
    auto cmp = [&](const pair<int,int>& a, const pair<int,int>& b) { return menor(E[b.first], E[a.first]); };
    priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(cmp)> pq(cmp);

    vector<bool> vis(n, false);
    vector<Arista> res;
    for (int s = 0; s < n; s++) {
        if (vis[s]) continue;
        vis[s] = true;
        for (auto& p : ady[s]) pq.push(p);
        while (!pq.empty()) {
            auto [idx, dest] = pq.top(); pq.pop();
            if (vis[dest]) continue;
            vis[dest] = true;
            res.push_back(E[idx]);
            for (auto& p : ady[dest]) if (!vis[p.second]) pq.push(p);
        }
    }
    return res;
}

// Compara dos AEM: mismas aristas (tras ordenarlas) y mismo peso.
bool iguales(vector<Arista> a, vector<Arista> b) {
    if (a.size() != b.size()) return false;
    sort(a.begin(), a.end(), menor);
    sort(b.begin(), b.end(), menor);
    for (size_t i = 0; i < a.size(); i++)
        if (a[i].u != b[i].u || a[i].v != b[i].v || a[i].w != b[i].w) return false;
    return true;
}

const char* EJEMPLO =
    "8 13\n"
    "0 1 4\n0 2 3\n1 2 5\n1 3 6\n2 3 7\n2 4 8\n3 4 2\n3 5 9\n4 5 10\n4 6 11\n5 6 1\n5 7 12\n6 7 13\n";

int main() {
    cout << "Usar grafo de ejemplo? (s/n): ";
    char op; cin >> op;
    istringstream ejemplo(EJEMPLO);
    istream& in = (op == 's' || op == 'S') ? static_cast<istream&>(ejemplo) : cin;
    if (op != 's' && op != 'S') cout << "Ingrese 'n m' y luego m lineas 'u v w':\n";

    int n, m;
    in >> n >> m;
    vector<Arista> E;
    for (int i = 0; i < m; i++) {
        int u, v; long long w;
        in >> u >> v >> w;
        if (u == v || u < 0 || v < 0 || u >= n || v >= n) continue;
        if (u > v) swap(u, v);
        E.push_back({u, v, w});
    }

    vector<Arista> aB = boruvka(n, E);
    vector<Arista> aK = kruskal(n, E);
    vector<Arista> aP = prim(n, E);

    cout << "\n=== VERIFICACION FINAL ===\n";
    cout << "Peso Boruvka = " << pesoTotal(aB) << "\n";
    cout << "Peso Kruskal = " << pesoTotal(aK) << "\n";
    cout << "Peso Prim    = " << pesoTotal(aP) << "\n";
    cout << "Boruvka == Kruskal (aristas): " << (iguales(aB, aK) ? "SI" : "NO") << "\n";
    cout << "Boruvka == Prim    (aristas): " << (iguales(aB, aP) ? "SI" : "NO") << "\n";
    return 0;
}
