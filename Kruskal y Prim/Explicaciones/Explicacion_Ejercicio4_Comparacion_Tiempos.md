# Ejercicio 4 — Comparación de tiempos: Kruskal vs Prim (grafos densos y dispersos)

**Archivo:** `Codigos/ejercicio4_comparacion_tiempos.cpp`
**Compilar:** `g++ -O2 -std=c++17 ejercicio4_comparacion_tiempos.cpp -o ejercicio4` · **Ejecutar:** `./ejercicio4`
(Usa ~100 MB de RAM en el caso n = 2000; tarda unos segundos.)

## 2. Explicación detallada del código

### 2.1 Bloques esenciales y por qué se aplicaron

**a) Generador de grafos aleatorios conexos (`generarGrafo`).**
1. *Árbol aleatorio primero:* se baraja una permutación de vértices y cada vértice i se conecta con uno aleatorio anterior. Esto **garantiza conectividad**, así todos los algoritmos calculan un AEM completo y sus pesos son comparables.
2. *Aristas extra:* se sortean pares (u, v) evitando lazos y repetidos (matriz `usada`) hasta llegar a exactamente `m` aristas.
3. Pesos uniformes en [1, 10⁶]; `mt19937` con **semilla fija** → experimento reproducible.

**b) Definición de "disperso" y "denso".**
- Disperso: `m = 3n` (E ≈ V).
- Denso: `m = 0.7 · n(n-1)/2` (E ≈ V²).
Se prueba con n = 100, 500, 1000, 2000 para ver cómo escala cada algoritmo.

**c) Tres algoritmos medidos.**
- `kruskal`: ordenar + Union-Find (con compresión iterativa). O(E log E). Recibe una *copia* del vector porque el ordenamiento es parte de su costo real.
- `primCola`: min-heap + lista de adyacencia. O(E log V). **La construcción de la lista de adyacencia se incluye en el tiempo** (Prim la necesita, Kruskal no).
- `primMatriz`: Prim clásico sin heap: en cada iteración busca el vértice de menor `dist` en O(V) y relaja su fila de la matriz en O(V) → **O(V²)** independientemente de E. También incluye construir la matriz.
*Por qué la tercera:* la pregunta del ejercicio es *cuándo conviene cada uno*; Prim-matriz es la variante que explica el caso denso.

**d) Medición (`medir`).**
Plantilla que ejecuta una función `REPS = 3` veces con `steady_clock` y devuelve el promedio en milisegundos, además del peso total obtenido.

**e) Validación cruzada.**
La última columna ("Pesos iguales") comprueba que los tres algoritmos dieron el mismo peso total. Si no, habría un error y la comparación no tendría sentido.

### 2.2 Flujo de ejecución
Para cada tipo (Disperso, Denso) y cada n: generar el grafo → medir los tres algoritmos → elegir el más rápido → verificar pesos → imprimir una fila de la tabla.

### Salida real (una ejecución en el entorno de prueba; en tu equipo los ms serán distintos)
```
Tipo        n        m  Kruskal(ms) PrimCola(ms) PrimMatriz(ms)  Mas rapido
Disperso   100      300        0.015        0.041        0.059   Kruskal
Disperso   500     1500        0.106        0.294        0.893   Kruskal
Disperso  1000     3000        0.248        0.515        6.460   Kruskal
Disperso  2000     6000        1.228        1.099       14.659   PrimCola
Denso      100     3465        0.254        0.199        0.047   PrimMatriz
Denso      500    87325        9.255        3.765        1.054   PrimMatriz
Denso     1000   349650       44.084       16.754        5.036   PrimMatriz
Denso     2000  1399300      196.523       92.734       24.228   PrimMatriz
```

### Análisis de resultados
- **Disperso:** Kruskal y Prim-cola van parejos (ambos O(E log E) con E ≈ 3V; a n = 2000 casi empatan y puede ganar cualquiera por ruido de medición). **Prim-matriz pierde claramente** porque siempre paga V²: con n = 2000 son 4 millones de operaciones aunque solo haya 6000 aristas.
- **Denso:** **Prim-matriz gana siempre** y la ventaja crece con n (≈ 8× sobre Kruskal en n = 2000). Kruskal debe ordenar ~1.4 millones de aristas; Prim-cola gestiona un heap grande; Prim-matriz solo hace barridos lineales sin logaritmos.
- Entre las variantes con logaritmo, **Prim-cola supera a Kruskal en grafos densos** (≈ 2×) porque no ordena todas las aristas.

## 3. Síntesis y criterio de decisión
| Caso | Mejor elección | Razón |
|---|---|---|
| Grafo disperso (E ≈ V) | **Kruskal** (o Prim-cola) | O(E log E) ≈ O(V log V); simple, solo necesita la lista de aristas y no construye estructuras de adyacencia |
| Grafo denso (E ≈ V²) | **Prim con matriz O(V²)** | no depende de E ni usa logaritmos; con E ≈ V², E log E ≈ V² log V es peor |
| Intermedio con grafo ya en lista de adyacencia | **Prim con cola** | no hace falta ordenar todas las aristas |
| Aristas ya ordenadas o muy pocas | **Kruskal** | el ordenamiento es gratis o muy barato |

Regla práctica: comparar **E log V** contra **V²**; si E es del orden de V²/log V o mayor, conviene Prim con matriz.
Nota: Kruskal es a menudo preferido cuando el grafo llega como lista de aristas; Prim cuando ya está en adyacencia.
