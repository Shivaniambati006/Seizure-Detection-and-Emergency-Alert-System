#include <Wire.h>
#include <MPU6050.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <Normal_vs_Seizure_inferencing.h>

// CHANGE THESE
const char* ssid = "";
const char* password = "";
const char* mqtt_server = "10.115.139.138";
const int mqtt_port = 1883;
const char* mqtt_topic = "seizure/alert";

WiFiClient espClient;
PubSubClient client(espClient);
MPU6050 mpu;

#define SAMPLE_RATE 50
#define SAMPLE_INTERVAL_MS (1000 / SAMPLE_RATE)
#define AXES 6

float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
unsigned long last_sample_time = 0;
int feature_ix = 0;

// ── WiFi connect ──────────────────────────────
void connectWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.print("WiFi connected! IP: ");
    Serial.println(WiFi.localIP());
}

// ── MQTT connect ──────────────────────────────
void connectMQTT() {
    client.setServer(mqtt_server, mqtt_port);
    while (!client.connected()) {
        Serial.print("Connecting to MQTT...");
        if (client.connect("ESP32_SeizureDevice")) {
            Serial.println("MQTT connected!");
        } else {
            Serial.print("Failed, rc=");
            Serial.println(client.state());
            delay(2000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9);

    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println("MPU6050 error");
        while (1);
    }
    Serial.println("MPU6050 ready");

    connectWiFi();
    connectMQTT();

    Serial.println("System ready — inference running");
}

void loop() {
    // Keep MQTT alive
    if (!client.connected()) connectMQTT();
    client.loop();

    // Maintain exact sampling rate
    if (millis() - last_sample_time < SAMPLE_INTERVAL_MS) return;
    last_sample_time = millis();

    // Read all 6 axes
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // Add new sample to buffer
    features[feature_ix++] = ax / 16384.0;
    features[feature_ix++] = ay / 16384.0;
    features[feature_ix++] = az / 16384.0;
    features[feature_ix++] = gx / 131.0;
    features[feature_ix++] = gy / 131.0;
    features[feature_ix++] = gz / 131.0;

    // When buffer is full → run inference
    if (feature_ix >= EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {

        signal_t signal;
        numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

        ei_impulse_result_t result = { 0 };
        run_classifier(&signal, &result, false);

        float seizure = result.classification[1].value;
        float normal  = result.classification[0].value;

        Serial.print("Seizure: ");
        Serial.print(seizure, 3);
        Serial.print(" | Normal: ");
        Serial.println(normal, 3);

        // Seizure detected → publish MQTT
        if (seizure > 0.8) {
            Serial.println("SEIZURE DETECTED — sending alert!");
            client.publish(mqtt_topic, "SEIZURE_DETECTED");
            Serial.println("MQTT alert sent!");

            // Small delay to avoid duplicate alerts
            delay(5000);
        } else {
            Serial.println("Normal activity");
        }

        //Sliding window
        memmove(features, features + AXES,
                (EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - AXES) * sizeof(float));
        feature_ix = EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE - AXES;
    }
}