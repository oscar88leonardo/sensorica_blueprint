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

void telemetry_init() {
    Serial.println();
    Serial.println(F("=================================================="));
    Serial.println(F("  Inicializando Telemetria Nodrix (Etapa 2)       "));
    Serial.println(F("=================================================="));

    Nodrix.setDebug(true);
    Nodrix.setInsecure(); // Modo de prueba TLS sin verificacion estricta de CA

    Serial.printf("[Telemetria] Conectando a red WiFi SSID: \"%s\"...\n", WIFI_SSID);
    Serial.printf("[Telemetria] Destino Cloudflare: https://%s\n", NODRIX_HOST);

    if (USE_WEBSOCKET_MODE) {
        Serial.println(F("[Telemetria] Modo de transporte: WebSocket"));
        Nodrix.begin(WIFI_SSID, WIFI_PASS, NODRIX_HOST, NODRIX_TOKEN);
    } else {
        Serial.println(F("[Telemetria] Modo de transporte: HTTPS (beginHTTP)"));
        Nodrix.beginHTTP(WIFI_SSID, WIFI_PASS, NODRIX_HOST, NODRIX_TOKEN);
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[Telemetria] WiFi OK! IP Asignada: %s\n", WiFi.localIP().toString().c_str());
    } else {
        Serial.println(F("[Telemetria] Esperando asignacion de red..."));
    }
    Serial.println(F("==================================================\n"));
}

void telemetry_run() {
    if (USE_WEBSOCKET_MODE) {
        Nodrix.run();
    }
}

bool telemetry_is_connected() {
    return (WiFi.status() == WL_CONNECTED);
}

bool telemetry_publish(const SensorData& data) {
    if (!telemetry_is_connected()) {
        Serial.println(F("[Telemetria] Alerta: WiFi no disponible, se omitio el envio."));
        return false;
    }

    Serial.println(F("[Telemetria] Enviando lote de métricas a Nodrix..."));
    Nodrix.send("temperatura", data.temperatura.value);
    Nodrix.send("humedad", data.humedad.value);
    Nodrix.send("iluminacion", data.iluminacion.value);
    Nodrix.flush();
    Serial.println(F("[Telemetria] Publicación completada exitosamente hacia Cloudflare."));

    return true;
}
