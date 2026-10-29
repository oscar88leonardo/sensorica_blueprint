#include <Arduino.h>
#include "config.h"
#include "sensors.h"
#include "telemetry.h"

static unsigned long last_telemetry_time = 0;

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial && millis() < 2000) {
        ; // Espera breve para conexión de puerto serie
    }

    Serial.println();
    Serial.println(F("=================================================="));
    Serial.println(F("  Sensorica Blueprint - ESP32 Telemetria IoT      "));
    Serial.println(F("=================================================="));

    // 1. Inicializar sensores analógicos (ADC1)
    sensors_init();
    Serial.println(F("[Hardware] Entradas ADC1 configuradas (GPIO 34, 35, 32)."));

    // 2. Inicializar conectividad WiFi y cliente Nodrix
    telemetry_init();

    Serial.printf("[Sistema] Publicacion programada cada %lu segundos.\n\n", TELEMETRY_INTERVAL_MS / 1000);
}

void loop() {
    // Mantener la pila de red y el cliente Nodrix
    telemetry_run();

    unsigned long current_time = millis();

    // Muestreo y publicación periódica cada TELEMETRY_INTERVAL_MS (10 segundos)
    if (current_time - last_telemetry_time >= TELEMETRY_INTERVAL_MS) {
        last_telemetry_time = current_time;

        // Lectura de los 3 canales analógicos
        SensorData data = sensors_read();

        // Salida formateada en monitor serie
        Serial.printf("[ADC1] T=%lu ms | ", current_time);
        Serial.printf("Temp: raw=%4u (%.1f °C) | ", data.temperatura.raw_adc, data.temperatura.value);
        Serial.printf("Hum: raw=%4u (%.1f %% HR) | ", data.humedad.raw_adc, data.humedad.value);
        Serial.printf("Lux: raw=%4u (%.1f lx)\n", data.iluminacion.raw_adc, data.iluminacion.value);

        // Envío de métricas a la nube Nodrix
        telemetry_publish(data);
    }

    delay(20);
}
