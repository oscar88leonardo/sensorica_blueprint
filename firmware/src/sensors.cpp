#include "sensors.h"
#include "config.h"
#include <math.h>

static SensorReading read_scaled_channel(uint8_t pin, float min_val, float max_val) {
    SensorReading reading{};
    reading.raw_adc = analogRead(pin);

    if (reading.raw_adc > ADC_RAW_MAX) {
        reading.value = NAN;
        reading.valid = false;
        return reading;
    }

    reading.value = (static_cast<float>(reading.raw_adc) * (max_val - min_val) / ADC_MAX_VAL) + min_val;
    reading.valid = !isnan(reading.value) && !isinf(reading.value) &&
                    (reading.value >= min_val) && (reading.value <= max_val);
    return reading;
}

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
    SensorData data{};

    // Lectura y validación individual por canal (Regla 1 de nodrix-esp32)
    data.temperatura = read_scaled_channel(PIN_TEMP, TEMP_MIN, TEMP_MAX);
    data.humedad     = read_scaled_channel(PIN_HUM, HUM_MIN, HUM_MAX);
    data.iluminacion = read_scaled_channel(PIN_LUX, LUX_MIN, LUX_MAX);

    return data;
}
