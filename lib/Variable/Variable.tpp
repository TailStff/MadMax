namespace MadMax
{
    template <class T>
    Variable<T>::Variable(ExecutionEnv *_executionEnv, VariablePersistencyValues<T> data)
    {
        this->executionEnv = _executionEnv;
        this->status.value = data.value;
    }

    template <class T>
    bool Variable<T>::SetValue(T value)
    {
        if (this->status.value != value)
        {
            this->status.value = value;
            return true;
        }
        return false;
    }

    template <class T>
    void Variable<T>::GetPersistencyValues(VariablePersistencyValues<T> &persistencyValues) const
    {
        persistencyValues.value = this->status.value;
    }

    /// @brief ISerializable implementation, Serialize persistency values into a byte vector
    /// @param data Output vector that will receive the serialized bytes
    /// @tparam T Type of the variable value
    template <class T>
    void Variable<T>::GetBytesFromData(std::vector<uint8_t> &data) const
    {
        VariablePersistencyValues<T> persistencyValues;
        GetPersistencyValues(persistencyValues);

        size_t size = sizeof(VariablePersistencyValues<T>);
        data.resize(size);

        memcpy(data.data(), &persistencyValues, size);
    }

    /// @brief ISerializable implementation, Deserialize persistency values from a byte vector
    /// @param data Input vector that contains the serialized bytes
    /// @tparam T Type of the variable value
    template <class T>
    void Variable<T>::SetDataFromBytes(std::vector<uint8_t> &data)
    {
        if (data.size() != sizeof(VariablePersistencyValues<T>))
            return;

        VariablePersistencyValues<T> persistencyValues;
        memcpy(&persistencyValues, data.data(), sizeof(VariablePersistencyValues<T>));

        this->status.value = persistencyValues.value;
    }

#pragma region IVariableValue
    template <class T>
    VariableValue Variable<T>::GetVariantValue() const
    {
        // We return the value as a mmVariableValue variant, this will allow to access to the variable value without knowing its type, it will be used for example in the web interface to display variable values in a generic way
        return VariableValue{this->status.value};
    }

    template <class T>
    bool Variable<T>::SetVariantValue(const VariableValue &v)
    {
        return std::visit(
            [this](const auto &val) -> bool
            {
                using V = std::decay_t<decltype(val)>;

                if constexpr (std::is_same_v<V, T>)
                {
                    // Types identiques, pas besoin de cast
                    return this->SetValue(val);
                }
                else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>)
                {
                    // Conversion entre types scalaires ok
                    return this->SetValue(static_cast<T>(val));
                }
                else
                {
                    // Types incompatibles (ex: mmByteArray -> float)
                    return false;
                }
            },
            v);
    }

    /// @brief Function that read variable value from Modbus memory space, the address is given by the provider, if the address is valid and the variable is associated to a Modbus register, we copy the value from the Modbus memory space to the variable value, this will allow to update variable value with the last value received from Modbus master
    /// @tparam T Type of the variable value
    /// @param address The Modbus address associated to the variable, this address is given by the provider, if it's valid and the variable is associated to a Modbus register, we copy the value from the Modbus memory space to the variable value, this will allow to update variable value with the last value received from Modbus master
    /// @return Return true if the value was successfully read from Modbus memory space and updated to the variable value, false if the address is invalid or the variable is not associated to any Modbus register
    template <class T>
    bool Variable<T>::ReadFromModbus(int32_t address)
    {
        // Copy the last saved value to modbus memory space, if it's valid, we consider that an address of -1 is an invalid address that mean that the variable is not associated to any Modbus register, this allow to create variable that are not exposed through Modbus if we want to
        if (address != -1)
        {
            // We consider that the address is a 32 bits integer where the 16 most significant bits represent the Modbus memory space (for example, holding registers, input registers, coils, discrete inputs) and the 16 least significant bits represent the Modbus address in that memory space, this allow to associate variables to different types of Modbus registers and not only holding registers
            uint8_t modbusMemorySpace = (address & 0x00FF0000) >> 16;

            // Holding registers
            if (modbusMemorySpace == 4)
            {
                // We associate the variable to the Modbus register using the modbus server manager, we also store the Modbus address in the provider to be able to retrieve it later if needed
                uint16_t modbusAddress = address & 0x0000FFFF;

                T *value = executionEnv->GetModbusServerMemoryManager()->AssociateHoldingRegister<T>(modbusAddress);

                this->status.value = *value;

                return true;
            }
        }

        return false;
    }

    /// @brief Function that write variable value to Modbus memory space, the address is given by the provider, if the address is valid and the variable is associated to a Modbus register, we copy the value from the variable value to the Modbus memory space, this will allow to update Modbus register with the latest value
    /// @tparam T Type of the variable value
    /// @param address The Modbus address associated to the variable, this address is given by the provider, if it's valid and the variable is associated to a Modbus register, we copy the value from the variable value to the Modbus memory space, this will allow to update Modbus register with the latest value
    /// @remarks This function will be called by the provider when we want to update Modbus memory space with the latest variable value, for example when we receive a write request from a Modbus master, we will update the variable value with the new value received from Modbus master and then call this function to update Modbus memory space with the latest variable value, this will allow to keep Modbus memory space updated with the latest variable values and avoid inconsistencies between variable values and Modbus memory space
    template <class T>
    bool Variable<T>::WriteToModbus(int32_t address)
    {
        // Copy the last saved value to modbus memory space, if it's valid, we consider that an address of -1 is an invalid address that mean that the variable is not associated to any Modbus register, this allow to create variable that are not exposed through Modbus if we want to
        if (address != -1)
        {
            // We consider that the address is a 32 bits integer where the 16 most significant bits represent the Modbus memory space (for example, holding registers, input registers, coils, discrete inputs) and the 16 least significant bits represent the Modbus address in that memory space, this allow to associate variables to different types of Modbus registers and not only holding registers
            uint8_t modbusMemorySpace = (address & 0x00FF0000) >> 16;

            // Holding registers
            if (modbusMemorySpace == 4)
            {
                // We associate the variable to the Modbus register using the modbus server manager, we also store the Modbus address in the provider to be able to retrieve it later if needed
                uint16_t modbusAddress = address & 0x0000FFFF;

                T *value = executionEnv->GetModbusServerMemoryManager()->AssociateHoldingRegister<T>(modbusAddress);
                *value = this->status.value;

                return true;
            }
        }
        return false;
    }
#pragma endregion IVariableValue

}