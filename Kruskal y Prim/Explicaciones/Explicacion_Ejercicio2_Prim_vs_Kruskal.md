# Ejercicio 2 — Prim con cola de prioridad vs Kruskal

**Archivo:** `Codigos/ejercicio2_prim_vs_kruskal.cpp`
**Compilar:** `g++ -O2 -std=c++17 ejercicio2_prim_vs_kruskal.cpp -o ejercicio2` · **Ejecutar:** `./ejercicio2`

## 2. Explicación detallada del código

### 2.1 Bloques esenciales y por qué se aplicaron

**a) Orden total de aristas (`menor`).**
```cpp
bool menor(const Arista& a, const Arista& b) {
    if (a.w != b.w) return a.w < b.w;
    if (a.u != b.u) return a.u < b.u;
    return a.v < b.v;
}
```
Compara por peso y, si hay empate, por (u, v). *Por qué:* si dos aristas pesan lo mismo, Kruskal y Prim podrían escoger aristas distintas y dar **árboles diferentes pero igual de buenos**, lo que haría la comparación de aristas ambigua. Con un orden estricto el AEM es **único**, y la comparación es exacta. Ambos algoritmos usan la misma función.

**b) Kruskal (para comparar).**
Se ordena todo el conjunto de aristas con `menor` y se aceptan las que no forman ciclo usando Union-Find con compresión de caminos (igual que el Ejercicio 1). Estrategia: **global por aristas**.

**c) Prim con cola de prioridad.**
```cpp
struct MayorPrioridad {
    bool operator()(const Candidato& a, const Candidato& b) const { return menor(b.e, a.e); }
};
priority_queue<Candidato, vector<Candidato>, MayorPrioridad> pq;
```
`priority_queue` de C++ es un *max-heap*; invertir la comparación la convierte en *min-heap* (siempre sale la arista más barata). Cada `Candidato` guarda la arista y el vértice destino al que llevaría.
*Por qué:* Prim necesita, en cada paso, "la arista más barata que sale del árbol actual hacia un vértice nuevo". El heap lo da en O(log E) en lugar de O(E) buscando linealmente.

**d) Núcleo de Prim (variante perezosa o *lazy*).**
```cpp
Candidato c = pq.top(); pq.pop();
if (visitado[c.destino]) continue;      // arista que ya no sirve
visitado[c.destino] = true;
res.push_back(c.e);
for (const Candidato& sig : ady[c.destino])
    if (!visitado[sig.destino]) pq.push(sig);
```
Se extrae la mejor arista; si su destino ya está en el árbol se descarta (habría un ciclo), si no, se incorpora el vértice y se insertan sus aristas hacia vértices aún no visitados. *Por qué lazy:* en vez de actualizar prioridades dentro del heap (que `priority_queue` no permite), se deja que entradas obsoletas se acumulen y se ignoran al salir.

**e) Bucle exterior `for (inicio...)`.**
Prim solo recorre la componente del vértice inicial. Al repetirlo desde cada vértice no visitado, un grafo desconexo produce un **bosque de expansión mínima**, igual que Kruskal.

**f) Comparación.**
Se copian ambos resultados, se ordenan con `menor` y se comparan elemento a elemento (u, v, w), además de comparar los pesos totales. Así se demuestra que, aunque el **orden de selección** es distinto, el **árbol final** es el mismo.

### 2.2 Flujo de ejecución
1. Lectura del grafo; se guarda tanto la lista de aristas (Kruskal) como la lista de adyacencia (Prim), siempre con u < v.
2. `kruskal(...)` → vector de aristas elegidas.
3. `prim(...)` → vector de aristas elegidas.
4. Se imprimen ambos árboles con sus pesos totales.
5. Se ordenan y comparan: se informa si pesos y aristas son idénticos.

### Salida real con el grafo de ejemplo (7 vértices, 11 aristas)
```
=== KRUSKAL ===            === PRIM ===
 0-3 (5)  2-4 (5)           0-3 (5)  3-5 (6)
 3-5 (6)  0-1 (7)           0-1 (7)  1-4 (7)
 1-4 (7)  4-6 (9)           2-4 (5)  4-6 (9)
 Peso total: 39             Peso total: 39
Aristas: ambos arboles tienen EXACTAMENTE las mismas aristas
```
Observa el **orden distinto** de selección: Kruskal toma las aristas globalmente por peso (5,5,6,7,7,9); Prim crece desde el vértice 0 y recién toma 2-4 al final porque ese vértice solo se vuelve alcanzable tras llegar a 1-4.

## 3. Síntesis del funcionamiento
Prim hace crecer **un único árbol** desde un vértice, añadiendo siempre la arista más barata que lo conecte con un vértice nuevo; Kruskal hace crecer **un bosque** de muchos árboles que se van fusionando.
```cpp
while (!pq.empty()) {
    Candidato c = pq.top(); pq.pop();
    if (visitado[c.destino]) continue;
    visitado[c.destino] = true; res.push_back(c.e);
    for (auto& sig : ady[c.destino]) if (!visitado[sig.destino]) pq.push(sig);
}
```
| | Kruskal | Prim (cola) |
|---|---|---|
| Estrategia | Aristas globales ordenadas | Vértices desde un árbol |
| Estructura clave | Union-Find | Min-heap + lista de adyacencia |
| Complejidad | O(E log E) | O(E log V) |
| Resultado | Mismo AEM (mismo peso y, con orden total, mismas aristas) | |

Como log E ≤ 2 log V, ambas complejidades son del mismo orden; la diferencia práctica se estudia en el Ejercicio 4.
