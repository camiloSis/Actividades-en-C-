# Ejercicio 3 — Algoritmo de Borůvka por rondas

**Archivo:** `Codigos/ejercicio3_boruvka.cpp`
**Compilar:** `g++ -O2 -std=c++17 ejercicio3_boruvka.cpp -o ejercicio3` · **Ejecutar:** `./ejercicio3`

## 2. Explicación detallada del código

### 2.1 Bloques esenciales y por qué se aplicaron

**a) Orden total de aristas (`menor`) — imprescindible aquí.**
En Borůvka todas las componentes eligen su arista más barata **al mismo tiempo**. Si hay pesos repetidos, dos componentes podrían elegir aristas distintas de igual peso y, al añadirlas juntas, **formar un ciclo**. Con el desempate (peso, u, v) cada componente tiene una única "mejor" arista y se garantiza que el resultado es un árbol (y el mismo AEM que Kruskal/Prim).

**b) Paso 1 de cada ronda: arista saliente más barata por componente.**
```cpp
vector<int> mejor(n, -1);
for (int i = 0; i < E.size(); i++) {
    int a = uf.buscar(E[i].u), b = uf.buscar(E[i].v);
    if (a == b) continue;                              // arista interna: se ignora
    if (mejor[a] == -1 || menor(E[i], E[mejor[a]])) mejor[a] = i;
    if (mejor[b] == -1 || menor(E[i], E[mejor[b]])) mejor[b] = i;
}
```
Se recorren todas las aristas una sola vez; las que unen dos componentes distintas son candidatas para **ambas** componentes. `mejor[raíz]` guarda el índice de la mejor. *Por qué:* por la propiedad del corte, la arista más barata que sale de una componente pertenece al AEM, así que todas las elegidas son seguras a la vez.

**c) Paso 2: agregar las aristas y fusionar.**
```cpp
for (int r = 0; r < n; r++) {
    if (mejor[r] == -1) continue;
    if (uf.unir(e.u, e.v)) { res.push_back(e); agregadas++; }
}
```
Una misma arista puede ser la mejor de sus **dos** extremos; la segunda vez `unir` devuelve `false` y no se duplica. Union-Find con compresión de caminos mantiene rápida la pregunta "¿en qué componente está este vértice?".

**d) Por qué terminan pocas rondas.**
Cada componente se fusiona con al menos otra, por lo que el número de componentes **al menos se reduce a la mitad** en cada ronda → como mucho ⌈log₂ V⌉ rondas. Cada ronda cuesta O(E α(V)) → total **O(E log V)**.

**e) Condición de parada y grafos disconexos.**
Si en una ronda `agregadas == 0`, ninguna componente tiene aristas salientes: el grafo no es conexo y se devuelve un bosque (se avisa con un mensaje).

**f) Verificación contra Kruskal y Prim.**
El archivo incluye versiones compactas de ambos. `iguales(a,b)` ordena los dos conjuntos con `menor` y compara arista por arista; además se muestran los tres pesos totales.

### 2.2 Flujo de ejecución
1. Leer el grafo (aristas normalizadas con u < v).
2. `uf = UnionFind(n)`, componentes = n.
3. Mientras componentes > 1: calcular `mejor[]` → agregar aristas → imprimir las agregadas y las componentes restantes.
4. Ejecutar Kruskal y Prim sobre el mismo grafo.
5. Imprimir los tres pesos y confirmar que los conjuntos de aristas coinciden.

### Salida real con el grafo de ejemplo (8 vértices, 13 aristas)
```
Componentes iniciales: 8
--- Ronda 1 ---
  + arista 0 - 2 (peso 3)
  + arista 0 - 1 (peso 4)
  + arista 3 - 4 (peso 2)
  + arista 5 - 6 (peso 1)
  + arista 5 - 7 (peso 12)
  Aristas agregadas: 5 | Componentes restantes: 3
--- Ronda 2 ---
  + arista 1 - 3 (peso 6)
  + arista 3 - 5 (peso 9)
  Aristas agregadas: 2 | Componentes restantes: 1
Peso Boruvka = 37 | Peso Kruskal = 37 | Peso Prim = 37
Boruvka == Kruskal (aristas): SI      Boruvka == Prim (aristas): SI
```
De 8 componentes se pasó a 3 y luego a 1 en solo 2 rondas (≤ log₂ 8 = 3).

## 3. Síntesis del funcionamiento
Borůvka es un algoritmo **paralelizable por naturaleza**: en cada ronda **todas las componentes eligen a la vez su arista más barata hacia afuera** y se fusionan; se repite hasta quedar una sola.
```cpp
while (uf.componentes > 1) {
    // 1) mejor[c] = arista saliente más barata de cada componente c
    // 2) para cada c: if (uf.unir(e.u, e.v)) agregar e al AEM
}
```
- **Complejidad:** O(E log V) (≤ log V rondas × O(E) por ronda).
- **Correcto porque:** propiedad del corte + orden total (sin ciclos por empates).
- **Resultado:** idéntico en aristas y peso al de Kruskal y Prim (verificado en la ejecución).
