# Ejercicio 1 — Kruskal con Union-Find (compresión de caminos)

**Archivo:** `Codigos/ejercicio1_kruskal.cpp`
**Compilar:** `g++ -O2 -std=c++17 ejercicio1_kruskal.cpp -o ejercicio1` · **Ejecutar:** `./ejercicio1`

## 2. Explicación detallada del código

### 2.1 Bloques esenciales y por qué se aplicaron

**a) Modelo del problema (struct `Arista` + nombres → índices).**
Cada edificio es un vértice y cada tendido posible de cable es una arista con su costo. Se usa un `unordered_map<string,int>` para convertir el nombre que escribe el usuario en un índice 0..n-1, porque Union-Find trabaja con enteros.
*Por qué:* conectar todos los edificios con el menor costo es exactamente el problema del **Árbol de Expansión Mínima (AEM)**: un subgrafo conexo, sin ciclos, con n-1 aristas y suma de costos mínima.

**b) Ordenar las aristas por costo.**
```cpp
sort(aristas.begin(), aristas.end(), [](auto& x, auto& y){ return x.w < y.w; });
```
*Por qué:* Kruskal es un algoritmo **voraz**. Su elección local ("la arista más barata disponible") es siempre segura gracias a la *propiedad del corte*: la arista más barata que cruza cualquier corte pertenece a algún AEM. Ordenar permite recorrerlas siempre de la más barata a la más cara. Costo: O(E log E).

**c) Union-Find: `buscar` con compresión de caminos.**
```cpp
int buscar(int x) {
    if (padre[x] != x) padre[x] = buscar(padre[x]);   // compresión
    return padre[x];
}
```
Cada conjunto es una "isla" de edificios ya conectados. `buscar` devuelve la raíz de la isla. Al regresar de la recursión, **todos los nodos del camino recorrido quedan apuntando directo a la raíz**, de modo que el árbol se aplana y las siguientes búsquedas son casi instantáneas.
*Por qué:* la pregunta crítica de Kruskal es "¿estos dos edificios ya están conectados?". Con listas o recorridos costaría O(V) por pregunta; con Union-Find es O(α(n)) amortizado (α = inversa de Ackermann, ≤ 4 en la práctica).

**d) Union-Find: `unir` con unión por rango.**
```cpp
if (rango[a] < rango[b]) swap(a, b);
padre[b] = a;
if (rango[a] == rango[b]) rango[a]++;
```
Se cuelga siempre el árbol más bajo del más alto. *Por qué:* mantiene la altura ≤ log n incluso antes de comprimir (y garantiza que la recursión de `buscar` es poco profunda). Compresión + rango juntos dan el O(α(n)) mencionado.

**e) Detección de ciclos con el valor de retorno de `unir`.**
```cpp
if (uf.unir(e.u, e.v)) { /* aceptar */ } else { /* rechazar: ciclo */ }
```
Si `buscar(u) == buscar(v)` los dos edificios ya están conectados; agregar la arista cerraría un ciclo y solo aumentaría el costo. Por eso `unir` devuelve `false` y la arista se descarta.

**f) Corte anticipado.**
`if (elegidas.size() == n-1) break;` — un árbol sobre n vértices tiene exactamente n-1 aristas, no hay que revisar el resto.

**g) Detección de grafo no conexo.**
`uf.componentes` se decrementa en cada unión exitosa. Si al final es > 1, los tendidos ingresados no alcanzan para conectar todos los edificios y se avisa al usuario (se obtiene un *bosque* de expansión mínima).

### 2.2 Flujo de ejecución (resumen de los comentarios del código)
1. Se elige entre datos de ejemplo o ingreso manual (`istream& in` apunta a uno u otro).
2. Se leen n, los nombres, m y los m tendidos; se validan (nombre existente, costo ≥ 0, sin lazos).
3. Se ordenan las aristas por costo.
4. Se crea `UnionFind(n)`: cada edificio es su propia isla.
5. Para cada arista en orden: si une dos islas distintas → se acepta y se suma su costo; si no → se rechaza.
6. Se imprime la red, el costo total y un aviso si no es conexa.

### Salida real con los datos de ejemplo
```
ACEPTADA  Cafeteria - Gimnasio (costo 1)
ACEPTADA  Cafeteria - Auditorio (costo 2)
ACEPTADA  Rectorado - Biblioteca (costo 4)
ACEPTADA  Laboratorio - Auditorio (costo 4)
rechazada Gimnasio - Auditorio (costo 6) -> formaria un ciclo
rechazada Laboratorio - Gimnasio (costo 7) -> formaria un ciclo
ACEPTADA  Rectorado - Cafeteria (costo 8)
Costo total: 19
```
(Al llegar a n-1 = 5 aristas el algoritmo se detiene, por eso no se muestran las demás aristas.)

## 3. Síntesis del funcionamiento
Kruskal construye el árbol de cableado **eligiendo, de la más barata a la más cara, cada tendido que conecte dos grupos de edificios todavía separados**. Union-Find responde en tiempo casi constante si dos edificios ya están en el mismo grupo:
```cpp
for (const Arista& e : aristas)
    if (uf.unir(e.u, e.v)) { elegidas.push_back(e); costoTotal += e.w; }
```
- **Complejidad:** O(E log E) por el ordenamiento (+ O(E·α(V)) del Union-Find, despreciable).
- **Espacio:** O(V + E).
- **Correcto porque:** la arista más barata que cruza cualquier corte siempre pertenece a un AEM (propiedad del corte), y las aristas que forman ciclo nunca son necesarias.
