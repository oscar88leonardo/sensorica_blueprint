#include "telemetry.h"
#include "config.h"

#if __has_include("secrets.h")
    #include "secrets.h"
#elif __has_include("../include/secrets.h")
    #include "../include/secrets.h"
#else
    #error "secrets.h no encontrado. Crea secrets.h basándote en secrets.h.example con tus credenciales."
#endif

#include <Nodrix.h>
#include <WiFi.h>
#include <math.h>

static float round2(float val) {
    return roundf(val * 100.0f) / 100.0f;
}

#ifdef ENABLE_NODRIX_RELAY_CONTROL
// Handler de escritura desde la nube (debe declararse en ámbito de archivo según nodrix-esp32)
NODRIX_WRITE("relay") {
    bool relay_state = value.asBool();
    digitalWrite(PIN_RELAY, relay_state ? HIGH : LOW);
    // Confirmar el estado real del hardware hacia el dashboard
    Nodrix.send("relay", relay_state);
}
#endif

static void connect_wifi_fast() {
    if (WiFi.status() == WL_CONNECTED) return;

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    // En Wokwi-GUEST el AP virtual opera en el canal 6 (evita escaneo lento de 13 canales en WiFiMulti)
    if (strcmp(WIFI_SSID, "Wokwi-GUEST") == 0) {
        WiFi.begin(WIFI_SSID, WIFI_PASS, 6);
        unsigned long t0 = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - t0) < 5000) {
            delay(150);
            Serial.print('.');
        }
    }

    // Respaldo estándar para cualquier red WiFi física
    if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        unsigned long t0 = millis();
        while (WiFi.status() != WL_CONNECTED && (millis() - t0) < 15000) {
            delay(250);
            Serial.print('.');
        }
    }
    Serial.println();
}

void telemetry_init() {
    Serial.println();
    Serial.println(F("=================================================="));
    Serial.println(F("  Inicializando Telemetria Nodrix SDK             "));
    Serial.println(F("=================================================="));

#ifdef ENABLE_NODRIX_RELAY_CONTROL
    pinMode(PIN_RELAY, OUTPUT);
    digitalWrite(PIN_RELAY, LOW);
#endif

    Nodrix.setDebug(true);

#ifdef NODRIX_CA_CERT
    Nodrix.setCACert(NODRIX_CA_CERT);
    Serial.println(F("[Telemetria] TLS: Certificado CA fijado (setCACert)."));
#else
    Nodrix.setInsecure();
    Serial.println(F("[Telemetria] TLS: Modo desarrollo sin verificacion CA (setInsecure)."));
#endif

    Nodrix.onConnect([]() {
        Serial.println(F("[Telemetria] Sesion WebSocket Nodrix conectada."));
    });
    Nodrix.onDisconnect([]() {
        Serial.println(F("[Telemetria] Sesion WebSocket Nodrix desconectada."));
    });

    Serial.printf("[Telemetria] Conectando a red WiFi SSID: \"%s\"...\n", WIFI_SSID);
    Serial.printf("[Telemetria] Host Nodrix: %s\n", NODRIX_HOST);

    // 1. Asociación rápida (optimizada para canal 6 en Wokwi)
    connect_wifi_fast();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[Telemetria] WiFi OK! IP Asignada: %s\n", WiFi.localIP().toString().c_str());
    }

    // 2. Registrar AP en Nodrix para reconexión autónoma si cae el enlace (nodrix-esp32 skill)
    Nodrix.addAP(WIFI_SSID, WIFI_PASS);

#ifdef NODRIX_USE_HTTP_POLL
    Nodrix.beginHTTP(NODRIX_HOST, NODRIX_TOKEN);
    Serial.println(F("[Telemetria] Modo HTTP Polling inicializado."));
#else
    Nodrix.begin(NODRIX_HOST, NODRIX_TOKEN);
    Serial.println(F("[Telemetria] Modo WebSocket en tiempo real inicializado."));
#endif

    Serial.println(F("==================================================\n"));
}

void telemetry_run() {
#ifndef NODRIX_USE_HTTP_POLL
    // Regla 3 de nodrix-esp32: llamar a Nodrix.run() continuamente sin delay()
    Nodrix.run();
#endif
}

bool telemetry_is_connected() {
#ifdef NODRIX_USE_HTTP_POLL
    return (WiFi.status() == WL_CONNECTED);
#else
    return Nodrix.connected();
#endif
}

bool telemetry_publish(const SensorData& data) {
    uint8_t staged_count = 0;

    // Regla 1 de nodrix-esp32: nunca enviar valores de relleno si falla una lectura; omitir ese canal
    if (data.temperatura.valid) {
        Nodrix.send("temperatura", round2(data.temperatura.value));
        staged_count++;
    } else {
        Serial.println(F("[Telemetria] Alerta: Lectura de 'temperatura' invalida, canal omitido."));
    }

    if (data.humedad.valid) {
        Nodrix.send("humedad", round2(data.humedad.value));
        staged_count++;
    } else {
        Serial.println(F("[Telemetria] Alerta: Lectura de 'humedad' invalida, canal omitido."));
    }

    if (data.iluminacion.valid) {
        Nodrix.send("iluminacion", round2(data.iluminacion.value));
        staged_count++;
    } else {
        Serial.println(F("[Telemetria] Alerta: Lectura de 'iluminacion' invalida, canal omitido."));
    }

    if (staged_count == 0) {
        Serial.println(F("[Telemetria] Error: Ningun canal valido en este ciclo; envio cancelado."));
        return false;
    }

#ifdef NODRIX_USE_HTTP_POLL
    return Nodrix.poll();
#else
    // En modo WebSocket, flush() envía el lote de inmediato si el socket está activo,
    // o mantiene las lecturas encoladas en el buffer interno hasta que reconecte.
    Nodrix.flush();
    return Nodrix.connected();
#endif
}
