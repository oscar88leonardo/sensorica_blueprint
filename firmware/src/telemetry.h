#pragma once

#include "sensors.h"

/**
 * @brief Inicializa la conexión WiFi y el cliente Nodrix.
 */
void telemetry_init();

/**
 * @brief Gestiona el bucle de eventos de fondo de la librería Nodrix.
 */
void telemetry_run();

/**
 * @brief Publica las lecturas de los 3 sensores hacia la nube Nodrix.
 * @param data Estructura con lecturas de temperatura, humedad e iluminación.
 * @return true si la solicitud fue encolada/enviada correctamente.
 */
bool telemetry_publish(const SensorData& data);

/**
 * @brief Comprueba si hay conexión de red activa.
 */
bool telemetry_is_connected();
