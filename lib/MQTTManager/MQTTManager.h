#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class MQTTManager
{
public:
    struct Message
    {
        char topic[128];
        char payload[32];
        bool retain;
    };

    MQTTManager(const char *broker, uint16_t port = 1883, const char *clientId = "MadMax");
    ~MQTTManager();

    bool Begin();

    /**
     * Ajoute un message dans la queue.
     *
     * Cette fonction ne fait aucun accès réseau.
     * Elle peut donc être appelée depuis le code MadMax.
     */
    bool Publish(const char *topic, const char *payload, bool retain = false);

    bool IsConnected();

private:
    static void TaskEntry(void *parameter);
    void Task();

    bool Connect();

private:
    const char *broker;
    uint16_t port;
    const char *clientId;

    WiFiClient networkClient;
    PubSubClient mqttClient;

    QueueHandle_t queue = nullptr;
    TaskHandle_t taskHandle = nullptr;

    uint32_t lastConnectionAttempt = 0;

    static constexpr uint32_t CONNECTION_RETRY_MS = 5000;
    static constexpr size_t QUEUE_SIZE = 32;
};