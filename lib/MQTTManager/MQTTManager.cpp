#include "MQTTManager.h"

MQTTManager::MQTTManager(const char *broker, uint16_t port, const char *clientId) : broker(broker), port(port), clientId(clientId), mqttClient(networkClient)
{
}

MQTTManager::~MQTTManager()
{
    if (taskHandle)
    {
        vTaskDelete(taskHandle);
        taskHandle = nullptr;
    }

    if (queue)
    {
        vQueueDelete(queue);
        queue = nullptr;
    }
}

bool MQTTManager::Begin()
{
    mqttClient.setServer(broker, port);

    queue = xQueueCreate(QUEUE_SIZE, sizeof(Message));

    if (!queue)
        return false;

    BaseType_t result = xTaskCreate(TaskEntry, "MQTT", 4096, this, 1, &taskHandle);

    return result == pdPASS;
}

bool MQTTManager::Publish(const char *topic, const char *payload, bool retain)
{
    if (!queue || !topic || !payload)
        return false;

    Message message{};

    strncpy(message.topic, topic, sizeof(message.topic) - 1);
    strncpy(message.payload, payload, sizeof(message.payload) - 1);
    message.retain = retain;

    /*
     * 0 = queue pleine.
     *
     * On ne bloque PAS le thread appelant.
     */
    return xQueueSend(queue, &message, 0) == pdTRUE;
}

bool MQTTManager::IsConnected()
{
    return mqttClient.connected();
}

void MQTTManager::TaskEntry(void *parameter)
{
    auto *manager = static_cast<MQTTManager *>(parameter);

    manager->Task();

    vTaskDelete(nullptr);
}

/// @brief Main task loop for MQTT management
void MQTTManager::Task()
{
    Message message;

    for (;;)
    {
        /*
         * Maintien de la connexion MQTT.
         */
        if (!mqttClient.connected())
        {
            uint32_t now = millis();

            if (now - lastConnectionAttempt >= CONNECTION_RETRY_MS)
            {
                lastConnectionAttempt = now;
                Connect();
            }
        }
        else
        {
            /*
             * Très important :
             * PubSubClient doit être régulièrement appelé.
             */
            mqttClient.loop();
        }

        /*
         * On récupère un message.
         *
         * Timeout de 100 ms :
         * la tâche reste réactive même sans message.
         */
        if (xQueueReceive(queue, &message, pdMS_TO_TICKS(100)) == pdTRUE)
        {
            if (mqttClient.connected())
            {
                mqttClient.publish(message.topic, message.payload, message.retain);
            }
        }
    }
}

bool MQTTManager::Connect()
{
    if (mqttClient.connected())
        return true;

    /*
     * Important :
     * on ne tente pas de connexion réseau
     * si aucune interface n'est disponible.
     *
     * À adapter à ta gestion ETH/WiFi.
     */
    // if (!ETH.linkUp() && WiFi.status() != WL_CONNECTED)
    //     return false;

    return mqttClient.connect(clientId);
}