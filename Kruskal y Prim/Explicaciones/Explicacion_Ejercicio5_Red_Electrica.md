# Ejercicio 5 — Red de distribución eléctrica (o de agua) entre localidades

**Archivo:** `Codigos/ejercicio5_red_electrica.cpp`
**Compilar:** `g++ -O2 -std=c++17 ejercicio5_red_electrica.cpp -o ejercicio5` · **Ejecutar:** `./ejercicio5`

> Las localidades, distancias y costos del ejemplo son **ficticios**, solo para demostrar el modelo. Puedes ingresar los tuyos.

## 2. Explicación detallada del código

### 2.1 Bloques esenciales y por qué se aplicaron

**a) Modelado como grafo ponderado.**
- Vértice = localidad. Arista = tramo de tendido posible. **Peso = km × costo por km**.
```cpp
double costo = km * cpk;     // costo total del tramo (miles de S/)
tramos.push_back({id[a], id[b], km, costo});
```
*Por qué:* el costo depende del terreno (un tramo corto por cerro puede costar más que uno largo en llano), por eso el usuario ingresa kilómetros **y** costo por km, y el algoritmo optimiza el costo, no la distancia.

**b) Por qué un AEM resuelve el problema.**
Conectar todas las localidades con el menor costo implica: (1) conexo, (2) sin ciclos (un ciclo significa un tramo redundante, un costo que se puede ahorrar). Eso es exactamente un árbol de expansión mínimo. Se usa **Kruskal** porque los datos llegan naturalmente como lista de tramos.

**c) Kruskal con pesos reales (`double`).**
Se ordena por costo con desempate por (u, v) para que el resultado sea determinista. El Union-Find (compresión de caminos + rango) decide si un tramo conecta zonas todavía separadas:
```cpp
if (uf.unir(t.u, t.v)) { red.push_back(t); costoMin += t.costo; kmTotal += t.km; }
```

**d) Métricas de negocio añadidas.**
- Costo total y longitud total de la red.
- `costoTodo`: lo que costaría construir **todos** los tramos posibles → se calcula el **ahorro** (%) del diseño óptimo. Da un resultado interpretable para una decisión real.

**e) Detección de localidades aisladas.**
Si `uf.componentes > 1`, los tramos disponibles no alcanzan para conectar todo y se avisa cuántas zonas quedan aisladas (se necesitaría proponer nuevos tramos).

### 2.2 Flujo de ejecución
1. Leer localidades y tramos (A, B, km, costo por km) y validarlos.
2. Calcular el costo de cada tramo y el costo de construir todo.
3. Ordenar tramos por costo; recorrerlos con Kruskal/Union-Find.
4. Imprimir los tramos elegidos, longitud total, costo mínimo, y ahorro.

### Salida real con la red de ejemplo (8 localidades, 14 tramos)
```
Characato <-> Sabandia    5.0 km   costo   175.0
 Socabaya <-> Sabandia    7.0 km   costo   280.0
    Cayma <-> Socabaya   10.0 km   costo   450.0
 Uchumayo <-> Socabaya    9.0 km   costo   495.0
     Yura <-> Cayma      12.0 km   costo   720.0
Characato <-> Chiguata   14.0 km   costo   980.0
 Chiguata <-> Polobaya   16.0 km   costo  1440.0
Tramos construidos : 7 de 14   |  Longitud total: 73.0 km
COSTO MINIMO TOTAL : 4540.0 (miles de S/)
Costo de construir TODOS los tramos: 13520.0  ->  ahorro 66.4%
```
Se eligen 7 = n-1 tramos. Nótese que `Uchumayo-Polobaya` (30 km, 95/km) nunca se construye: es el más caro y las localidades ya se alcanzan por otras vías.

## 3. Síntesis del funcionamiento
Se transforma el problema de ingeniería en un grafo cuyo peso es el costo real de cada tendido y se aplica Kruskal: **recorrer los tramos de más barato a más caro y construir solo los que unen zonas aún desconectadas**.
```cpp
for (const Tramo& t : tramos)
    if (uf.unir(t.u, t.v)) { red.push_back(t); costoMin += t.costo; }
```
- **Complejidad:** O(E log E). **Resultado:** conexión de todas las localidades con costo mínimo garantizado (óptimo, no una aproximación).
- **Aplicación:** el mismo modelo sirve para red de agua, fibra óptica o caminos; solo cambia lo que representa el peso.
- **Limitación:** el AEM minimiza el costo de *conectar*, no la fiabilidad (un árbol no tiene rutas alternativas: si un tramo falla, se aísla una parte). Para redundancia habría que añadir tramos extra.
