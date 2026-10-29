#pragma once

#include <Arduino.h>

struct SensorReading {
    uint16_t raw_adc;
    float value;
};

struct SensorData {
    SensorReading temperatura;
    SensorReading humedad;
    SensorReading iluminacion;
};

/**
 * @brief Configura la resolución ADC y los pines de entrada analógica.
 */
void sensors_init();

/**
 * @brief Lee los canales ADC1 y calcula los valores físicos escalados.
 * @return SensorData Estructura con lecturas crudas y escaladas.
 */
SensorData sensors_read();
