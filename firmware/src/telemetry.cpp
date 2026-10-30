#include "telemetry.h"
#include "config.h"

#if __has_include("secrets.h")
    #include "secrets.h"
#elif __has_include("../include/secrets.h")
    #include "../include/secrets.h"
#else
    #error "secrets.h no encontrado. Crea secrets.h basándote en secrets.h.example con tus credenciales."
#endif

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <math.h>

// Instancias persistentes para reutilizar la misma sesion TLS (HTTP Keep-Alive)
static WiFiClientSecure s_tlsClient;
static HTTPClient       s_http;
static String           s_telemetryUrl;
static String           s_authHeader;
static unsigned long    s_lastWifiRetry = 0;

static float round2(float val) {
    return roundf(val * 100.0f) / 100.0f;
}

static void connect_wifi_fast() {
    if (WiFi.status() == WL_CONNECTED) return;

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);

    // En Wokwi-GUEST el AP virtual opera en el canal 6 (evita escaneo de 13 canales)
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
    Serial.println(F("  Inicializando Telemetria Nodrix (HTTPS Reuse)   "));
    Serial.println(F("=================================================="));

    s_telemetryUrl = String("https://") + NODRIX_HOST + "/v1/telemetry";
    s_authHeader   = String("Bearer ") + NODRIX_TOKEN;

    // Configurar cliente TLS persistente
    s_tlsClient.setInsecure(); // Omitir validacion de CA en simulacion/pruebas
    s_tlsClient.setHandshakeTimeout(TLS_HANDSHAKE_TIMEOUT_S);

    // Configurar HTTPClient en modo HTTP/1.1 Keep-Alive (reutiliza el socket TLS)
    s_http.setReuse(true);
    s_http.useHTTP10(false);
    s_http.setTimeout(HTTP_TIMEOUT_MS);
    s_http.setUserAgent("SensoricaESP32/1.0");

    Serial.printf("[Telemetria] Conectando a red WiFi SSID: \"%s\"...\n", WIFI_SSID);
    Serial.printf("[Telemetria] Endpoint: %s\n", s_telemetryUrl.c_str());

    connect_wifi_fast();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[Telemetria] WiFi OK! IP Asignada: %s\n", WiFi.localIP().toString().c_str());
    } else {
        Serial.println(F("[Telemetria] Esperando asignacion de red..."));
    }

    Serial.println(F("==================================================\n"));
}

void telemetry_run() {
    if (WiFi.status() != WL_CONNECTED) {
        unsigned long now = millis();
        if (now - s_lastWifiRetry >= 5000) {
            s_lastWifiRetry = now;
            Serial.println(F("[Telemetria] Reconectando WiFi..."));
            s_tlsClient.stop();
            if (strcmp(WIFI_SSID, "Wokwi-GUEST") == 0) {
                WiFi.begin(WIFI_SSID, WIFI_PASS, 6);
            } else {
                WiFi.begin(WIFI_SSID, WIFI_PASS);
            }
        }
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

    JsonDocument doc;
    JsonObject metrics = doc["metrics"].to<JsonObject>();
    metrics["temperatura"] = round2(data.temperatura.value);
    metrics["humedad"]     = round2(data.humedad.value);
    metrics["iluminacion"] = round2(data.iluminacion.value);

    String payload;
    serializeJson(doc, payload);

    bool reusingTls = s_tlsClient.connected();
    unsigned long t0 = millis();

    if (!s_http.begin(s_tlsClient, s_telemetryUrl)) {
        Serial.println(F("[Telemetria] Error: no se pudo inicializar HTTPClient."));
        s_tlsClient.stop();
        return false;
    }

    s_http.addHeader("Authorization", s_authHeader);
    s_http.addHeader("Content-Type", "application/json");

    int code = s_http.POST(payload);
    unsigned long dt = millis() - t0;

    // Con setReuse(true), end() limpia la peticion actual pero mantiene abierto el socket TLS
    s_http.end();

    if (code == 204 || code == 200) {
        Serial.printf("[Telemetria] OK (HTTP %d en %lu ms | %s)\n",
                      code,
                      dt,
                      reusingTls ? "TLS Keep-Alive reutilizado" : "Nuevo handshake TLS");
        return true;
    }

    Serial.printf("[Telemetria] Fallo envio -> HTTP %d (%s) tras %lu ms\n",
                  code,
                  s_http.errorToString(code).c_str(),
                  dt);
    // Solo cerrar el socket TLS si hubo fallo de transporte (code < 0)
    if (code < 0) {
        s_tlsClient.stop();
    }
    return false;
}
