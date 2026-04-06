namespace MadMax
{
    template <class T>
    Variable<T>::Variable(ExecutionEnv *_executionEnv, VariablePersistencyValues<T> data)
    {
        this->executionEnv = _executionEnv;
        this->value = data.value;
    }

    template <class T>
    bool Variable<T>::SetValue(T value)
    {
        if (this->value != value)
        {
            this->value = value;
            return true;
        }
        return false;
    }

    template <class T>
    void Variable<T>::GetPersistencyValues(VariablePersistencyValues<T> &persistencyValues) const
    {
        persistencyValues.value = this->value;
    }

    /// @brief Serialize persistency values into a byte vector
    /// @param data Output vector that will receive the serialized bytes
    template <class T>
    void Variable<T>::GetBytesFromData(std::vector<uint8_t> &data) const
    {
        VariablePersistencyValues<T> persistencyValues;
        GetPersistencyValues(persistencyValues);

        size_t size = sizeof(VariablePersistencyValues<T>);
        data.resize(size);

        memcpy(data.data(), &persistencyValues, size);
    }

    template <class T>
    void Variable<T>::SetDataFromBytes(std::vector<uint8_t> &data)
    {
        if (data.size() != sizeof(VariablePersistencyValues<T>))
            return;

        VariablePersistencyValues<T> persistencyValues;
        memcpy(&persistencyValues, data.data(), sizeof(VariablePersistencyValues<T>));

        this->value = persistencyValues.value;
    }

#pragma region VariableValue
    template <class T>
    VariableValue Variable<T>::GetVariantValue() const
    {
        // We return the value as a mmVariableValue variant, this will allow to access to the variable value without knowing its type, it will be used for example in the web interface to display variable values in a generic way
        return VariableValue{value};
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
#pragma endregion VariableValue
}