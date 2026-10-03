/*
 * EJERCICIO 4 - Comparación de tiempos: Kruskal vs Prim en grafos aleatorios
 *               DISPERSOS y DENSOS de distintos tamaños.
 *
 * Se miden tres variantes:
 *   - Kruskal (ordenar aristas + Union-Find)           O(E log E)
 *   - Prim con cola de prioridad (lista de adyacencia) O(E log V)
 *   - Prim con matriz de adyacencia, sin cola          O(V^2)
 * (la tercera se agrega porque es la variante clásica que gana en grafos densos)
 *
 * Compilar: g++ -O2 -std=c++17 ejercicio4_comparacion_tiempos.cpp -o ejercicio4
 * Ejecutar: ./ejercicio4
 */
#include <bits/stdc++.h>
using namespace std;
using Reloj = chrono::steady_clock;

struct Arista { int u, v; int w; };

bool menor(const Arista& a, const Arista& b) {
    if (a.w != b.w) return a.w < b.w;
    if (a.u != b.u) return a.u < b.u;
    return a.v < b.v;
}

// ------------------------- Generador de grafos aleatorios -------------------
// Devuelve un grafo CONEXO con n vértices y exactamente m aristas (n-1 <= m <= n(n-1)/2).
vector<Arista> generarGrafo(int n, long long m, mt19937& rng) {
    vector<Arista> E;
    E.reserve(m);
    vector<char> usada((size_t)n * n, 0);           // matriz de "ya existe la arista (u,v)"
    uniform_int_distribution<int> peso(1, 1000000);

    auto agregar = [&](int u, int v) {
        if (u > v) swap(u, v);
        usada[(size_t)u * n + v] = 1;
        E.push_back({u, v, peso(rng)});
    };

    // 1) Árbol aleatorio: garantiza que el grafo sea conexo.
    vector<int> perm(n);
    iota(perm.begin(), perm.end(), 0);
    shuffle(perm.begin(), perm.end(), rng);
    for (int i = 1; i < n; i++) {
        int j = uniform_int_distribution<int>(0, i - 1)(rng);
        agregar(perm[i], perm[j]);
    }

    // 2) Aristas adicionales al azar, evitando repetidas, hasta llegar a m.
    uniform_int_distribution<int> vert(0, n - 1);
    while ((long long)E.size() < m) {
        int u = vert(rng), v = vert(rng);
        if (u == v) continue;
        if (u > v) swap(u, v);
        if (usada[(size_t)u * n + v]) continue;
        agregar(u, v);
    }
    return E;
}

// ------------------------------- Kruskal ------------------------------------
struct UnionFind {
    vector<int> padre, rango;
    UnionFind(int n) : padre(n), rango(n, 0) { iota(padre.begin(), padre.end(), 0); }
    int buscar(int x) {                      // versión iterativa con compresión (evita recursión profunda)
        int r = x;
        while (padre[r] != r) r = padre[r];
        while (padre[x] != r) { int sig = padre[x]; padre[x] = r; x = sig; }
        return r;
    }
    bool unir(int a, int b) {
        a = buscar(a); b = buscar(b);
        if (a == b) return false;
        if (rango[a] < rango[b]) swap(a, b);
        padre[b] = a;
        if (rango[a] == rango[b]) rango[a]++;
        return true;
    }
};

long long kruskal(int n, vector<Arista> E) {    // copia: el ordenamiento forma parte del costo
    sort(E.begin(), E.end(), menor);
    UnionFind uf(n);
    long long total = 0;
    int cnt = 0;
    for (const Arista& e : E) {
        if (uf.unir(e.u, e.v)) {
            total += e.w;
            if (++cnt == n - 1) break;       // árbol completo, no hace falta seguir
        }
    }
    return total;
}

// ------------------------- Prim con cola de prioridad -----------------------
long long primCola(int n, const vector<Arista>& E) {
    // Construir la lista de adyacencia cuenta como parte del costo de Prim.
    vector<vector<pair<int,int>>> ady(n);    // (vecino, peso)
    for (const Arista& e : E) {
        ady[e.u].push_back({e.v, e.w});
        ady[e.v].push_back({e.u, e.w});
    }
    using Par = pair<int,int>;               // (peso, vértice)
    priority_queue<Par, vector<Par>, greater<Par>> pq;   // min-heap
    vector<char> vis(n, 0);
    long long total = 0;
    int cnt = 0;

    pq.push({0, 0});
    while (!pq.empty() && cnt < n) {
        auto [w, u] = pq.top(); pq.pop();
        if (vis[u]) continue;                // entrada vieja de la cola (lazy deletion)
        vis[u] = 1; total += w; cnt++;
        for (auto [v, pw] : ady[u])
            if (!vis[v]) pq.push({pw, v});
    }
    return total;
}

// ------------------------- Prim con matriz (O(V^2)) -------------------------
long long primMatriz(int n, const vector<Arista>& E) {
    const int INF = INT_MAX;
    vector<int> mat((size_t)n * n, INF);     // construir la matriz también cuenta
    for (const Arista& e : E) {
        mat[(size_t)e.u * n + e.v] = e.w;
        mat[(size_t)e.v * n + e.u] = e.w;
    }
    vector<int> dist(n, INF);                // dist[v] = arista más barata que conecta v al árbol
    vector<char> vis(n, 0);
    long long total = 0;
    dist[0] = 0;
    for (int it = 0; it < n; it++) {
        int u = -1;
        for (int v = 0; v < n; v++)          // elegir el vértice no visitado con menor dist: O(V)
            if (!vis[v] && (u == -1 || dist[v] < dist[u])) u = v;
        vis[u] = 1;
        total += dist[u];
        const int* fila = &mat[(size_t)u * n];
        for (int v = 0; v < n; v++)          // relajar todas las salidas de u: O(V)
            if (!vis[v] && fila[v] < dist[v]) dist[v] = fila[v];
    }
    return total;
}

// Mide el tiempo medio (ms) de ejecutar f() 'reps' veces y devuelve también su resultado.
template <class F>
double medir(F f, int reps, long long& resultado) {
    double acum = 0;
    for (int i = 0; i < reps; i++) {
        auto t0 = Reloj::now();
        resultado = f();
        auto t1 = Reloj::now();
        acum += chrono::duration<double, milli>(t1 - t0).count();
    }
    return acum / reps;
}

int main() {
    mt19937 rng(12345);                                // semilla fija -> resultados reproducibles
    const vector<int> tamanos = {100, 500, 1000, 2000};
    const int REPS = 3;

    cout << fixed << setprecision(3);
    cout << left << setw(9) << "Tipo" << right << setw(7) << "n" << setw(10) << "m"
         << setw(13) << "Kruskal(ms)" << setw(13) << "PrimCola(ms)" << setw(14) << "PrimMatriz(ms)"
         << "  Mas rapido  Pesos iguales\n";
    cout << string(88, '-') << "\n";

    vector<string> resumen;
    for (string tipo : {"Disperso", "Denso"}) {
        for (int n : tamanos) {
            long long maxAristas = (long long)n * (n - 1) / 2;
            long long m = (tipo == "Disperso") ? min(maxAristas, 3LL * n)       // ~3n aristas: E ~ V
                                               : (long long)(0.7 * maxAristas); // 70% de las posibles: E ~ V^2
            vector<Arista> E = generarGrafo(n, m, rng);

            long long tk, tp, tm;
            double msK = medir([&] { return kruskal(n, E); }, REPS, tk);
            double msP = medir([&] { return primCola(n, E); }, REPS, tp);
            double msM = medir([&] { return primMatriz(n, E); }, REPS, tm);

            string ganador = "Kruskal";
            double mejor = msK;
            if (msP < mejor) { mejor = msP; ganador = "PrimCola"; }
            if (msM < mejor) { mejor = msM; ganador = "PrimMatriz"; }
            bool ok = (tk == tp && tp == tm);                    // los 3 deben dar el mismo peso

            cout << left << setw(9) << tipo << right << setw(7) << n << setw(10) << E.size()
                 << setw(13) << msK << setw(13) << msP << setw(14) << msM
                 << "  " << left << setw(12) << ganador << (ok ? "SI" : "NO (error)") << "\n";
        }
        cout << string(88, '-') << "\n";
    }

    cout << "\nCONCLUSIONES TEORICAS (contrastar con la tabla):\n"
         << " - Grafo disperso (E ~ V):  Kruskal O(E log E) y Prim-cola O(E log V) son equivalentes;\n"
         << "   Prim-matriz pierde por pagar O(V^2) y memoria V^2.\n"
         << " - Grafo denso (E ~ V^2):   Prim-matriz O(V^2) supera a los que usan log, pues\n"
         << "   Kruskal ordena ~V^2 aristas (V^2 log V) y Prim-cola maneja un heap enorme.\n";
    return 0;
}
