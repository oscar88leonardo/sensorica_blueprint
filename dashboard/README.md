# Dashboard: Monitoreo de Sensores

Este directorio contiene la definición exportada del dashboard creado en la nube **Nodrix** (Cloudflare Workers) a través del protocolo **Nodrix MCP**.

---

## Metadatos del Dashboard

- **ID del Dashboard:** `dsh_9oOUFs5IEnsH`
- **Proyecto:** `sensorica-blueprint` (`prj_0gE5nb29t9EJ`)
- **Nombre:** Monitoreo de Sensores
- **Acceso:** Inicia sesión en tu instancia de Cloudflare Workers:  
  `https://nodrixapp.oscar88leonardo-351.workers.dev` y selecciona el proyecto `sensorica-blueprint`.
- **Acceso Publico:** https://nodrixapp.oscar88leonardo-351.workers.dev/share/zbEBe-cFM4Vb 
---

## Disposición de Widgets (Grid de 16 Columnas)

```
+-------------------------------------------------------------------------------+
|                      FILA 1: TARJETAS DE VALOR INSTANTÁNEO                     |
| [Temp: °C] (w=5)       | [Humedad: % HR] (w=6)    | [Iluminación: lx] (w=5)   |
+-------------------------------------------------------------------------------+
|                      FILA 2: INDICADORES RADIALES (GAUGES)                    |
| [Gauge 0-100 °C] (w=5) | [Gauge 0-100 % HR] (w=6) | [Gauge 0-1000 lx] (w=5)   |
+-------------------------------------------------------------------------------+
|                      FILA 3: GRÁFICO HISTÓRICO MULTIVARIABLE                  |
|                    [Líneas de Temperatura, Humedad y Lux] (w=16)              |
+-------------------------------------------------------------------------------+
```

### Detalle de Widgets

| ID | Tipo | Variable | Rango / Unidad | Propósito |
|---|---|---|---|---|
| `wid_Dr2xLXvzSZzA` | `iot-value` | `temperatura` | °C | Lectura numérica instantánea |
| `wid_Q4Clm23L0iw6` | `iot-value` | `humedad` | % HR | Lectura numérica instantánea |
| `wid_z4Mp3JJ-Hxm2` | `iot-value` | `iluminacion` | lx | Lectura numérica instantánea |
| `wid_zYPhsLLTmH6Z` | `iot-gauge` | `temperatura` | 0 a 100 °C | Dial analógico con aguja |
| `wid_3j4U5BKHiLtv` | `iot-gauge` | `humedad` | 0 a 100 % HR | Dial analógico con aguja |
| `wid_Sm2kYJ41EnXS` | `iot-gauge` | `iluminacion` | 0 a 1000 lx | Dial analógico con aguja |
| `wid_Ov2M8Hn9cPCi` | `iot-chart` | `temperatura`, `humedad`, `iluminacion` | Ventana 1 hora | Curvas temporales continuas |
