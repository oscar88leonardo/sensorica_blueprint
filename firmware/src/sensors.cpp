#include "sensors.h"
#include "config.h"

void sensors_init() {
    // Configurar resolución de 12 bits (0 - 4095) y atenuación de 11 dB (escala completa hasta ~3.3V)
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);

    // Pines de entrada analógica exclusivamente en ADC1
    pinMode(PIN_TEMP, INPUT);
    pinMode(PIN_HUM, INPUT);
    pinMode(PIN_LUX, INPUT);
}

SensorData sensors_read() {
    SensorData data;

    // Lectura cruda del convertidor analógico a digital (12 bits)
    data.temperatura.raw_adc = analogRead(PIN_TEMP);
    data.humedad.raw_adc     = analogRead(PIN_HUM);
    data.iluminacion.raw_adc = analogRead(PIN_LUX);

    // Escalado lineal directo a unidades físicas:
    // valor = raw * (MAX - MIN) / ADC_MAX_VAL + MIN
    data.temperatura.value = (static_cast<float>(data.temperatura.raw_adc) * (TEMP_MAX - TEMP_MIN) / ADC_MAX_VAL) + TEMP_MIN;
    data.humedad.value     = (static_cast<float>(data.humedad.raw_adc) * (HUM_MAX - HUM_MIN) / ADC_MAX_VAL) + HUM_MIN;
    data.iluminacion.value = (static_cast<float>(data.iluminacion.raw_adc) * (LUX_MAX - LUX_MIN) / ADC_MAX_VAL) + LUX_MIN;

    return data;
}
