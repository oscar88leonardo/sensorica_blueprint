#pragma once

#include <Arduino.h>

// ==========================================
// Configuración de Hardware y Sensores
// ==========================================

// Pines ADC (Solo ADC1 - GPIO 32 a 39)
// Importante: ADC2 entra en conflicto con el controlador de radio WiFi.
constexpr uint8_t PIN_TEMP = 34; // CH0: GPIO34
constexpr uint8_t PIN_HUM  = 35; // CH1: GPIO35
constexpr uint8_t PIN_LUX  = 32; // CH2: GPIO32

// Pin de actuador opcional para control remoto (NODRIX_WRITE)
constexpr uint8_t PIN_RELAY = 2; // LED integrado en ESP32 DevKitC V4

// ==========================================
// Temporización y Comunicación
// ==========================================
#ifndef TELEMETRY_INTERVAL_MS
constexpr unsigned long TELEMETRY_INTERVAL_MS = 1000; // Publicación cada 1 s (modo demo interactiva)
#endif
constexpr unsigned long SERIAL_BAUD_RATE      = 115200;

// ==========================================
// Rangos Físicos de Escalado (ADC 12-bit: 0-4095)
// ==========================================
constexpr uint16_t ADC_RAW_MAX = 4095;
constexpr float    ADC_MAX_VAL = 4095.0f;

// Temperatura: 0 - 100 °C
constexpr float TEMP_MIN = 0.0f;
constexpr float TEMP_MAX = 100.0f;

// Humedad: 0 - 100 % HR
constexpr float HUM_MIN = 0.0f;
constexpr float HUM_MAX = 100.0f;

// Iluminación: 0 - 1000 lx
constexpr float LUX_MIN = 0.0f;
constexpr float LUX_MAX = 1000.0f;
