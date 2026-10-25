#pragma once

#include "ProtocolBinding.h"
#include <cstdint>
#include <string>

namespace MadMax
{
    class MQTTBinding : public ProtocolBinding
    {
    private:
        std::string property;
        std::string publishTopic;
        std::string subscribeTopic;

    public:
        MQTTBinding(const std::string &property, const std::string &publishTopic, const std::string &subscribeTopic) : property(property), publishTopic(publishTopic), subscribeTopic(subscribeTopic)
        {
        }

        ProtocolType GetProtocolType() const override
        {
            return ProtocolType::MQTT;
        }

        const std::string &GetProperty() const
        {
            return property;
        }

        const std::string &GetPublishTopic() const
        {
            return publishTopic;
        }

        const std::string &GetSubscribeTopic() const
        {
            return subscribeTopic;
        }

        MQTTBinding *AsMQTTBinding() override
        {
            return this;
        }

        const MQTTBinding *AsMQTTBinding() const override
        {
            return this;
        }
    };
}