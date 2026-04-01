namespace MadMax
{
    template <class T>
    Accum<T>::Accum(ExecutionEnv *_executionEnv, AccumPersistencyValues<T> data)
    {
        this->executionEnv = _executionEnv;
        this->memInput = false;
        this->memResetTrigger = false;
        this->value = data.value;
    }

    template <class T>
    bool Accum<T>::SetValue(T value)
    {
        if (this->value != value)
        {
            this->value = value;
            return true;
        }
        return false;
    }

    /// @brief Function that increment internal value by increment value when input value has rising edge
    /// @tparam T Any types
    /// @param input The Input value to be process
    /// @param increment The value to be added to the internal value at each rising edge of the input, if the increment is negative and the absolute value of the increment is greater than the current internal value, the internal value will be set to 0 to avoid underflow. If the increment is positive and the internal value is greater than the maximum value of T minus the increment, the internal value will be set to the maximum value of T to avoid overflow.
    /// @param resetTrigger
    /// @param resetValue
    /// @return Internal value
    template <class T>
    T Accum<T>::Evaluate(bool input, T increment, bool resetTrigger, T resetValue)
    {
        // We just reset the value, we don't want to count the increment in the same cycle even if the input is true
        if (resetTrigger && !memResetTrigger)
        {
            Reset(resetValue);
        }
        else if (input && !memInput)
        {
            if (increment > 0 && value > std::numeric_limits<T>::max() - increment)
                value = std::numeric_limits<T>::max();
            else
                value += increment;
        }

        memInput = input;
        memResetTrigger = resetTrigger;

        return value;
    }

    /// @brief Function that SET the internal value to the requested value
    /// @tparam T Any types
    /// @param resetValue Reset value
    template <class T>
    void Accum<T>::Reset(T resetValue)
    {
        value = resetValue;
    }

    template <class T>
    void Accum<T>::GetPersistencyValues(AccumPersistencyValues<T> &persistencyValues) const
    {
        persistencyValues.value = this->value;
    }

    /// @brief Serialize persistency values into a byte vector
    /// @param data Output vector that will receive the serialized bytes
    template <class T>
    void Accum<T>::GetBytesFromData(std::vector<uint8_t> &data) const
    {
        size_t size = sizeof(AccumPersistencyValues<T>);
        data.resize(size);

        memcpy(data.data(), &this->value, size);
    }

    template <class T>
    void Accum<T>::SetDataFromBytes(std::vector<uint8_t> &data)
    {
        if (data.size() != sizeof(AccumPersistencyValues<T>))
            return;

        AccumPersistencyValues<T> persistencyValues;
        memcpy(&persistencyValues, data.data(), sizeof(AccumPersistencyValues<T>));

        this->value = persistencyValues.value;
    }

#pragma region mmVariableValue
    template <class T>
    VariableValue Accum<T>::GetVariantValue() const
    {
        // We return the value as a mmVariableValue variant, this will allow to access to the variable value without knowing its type, it will be used for example in the web interface to display variable values in a generic way
        return VariableValue{value};
    }

    template <class T>
    bool Accum<T>::SetVariantValue(const VariableValue &v)
    {
        return std::visit([this](const auto &val) -> bool
                          {
        using V = std::decay_t<decltype(val)>;

        if constexpr (std::is_same_v<V, T>) {
            // Types identiques, pas besoin de cast
            return this->SetValue(val);
        }
        else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            // Conversion entre types scalaires ok
            return this->SetValue(static_cast<T>(val));
        }
        else {
            // Types incompatibles (ex: mmByteArray -> float)
            return false;
        } }, v);
    }
#pragma endregion mmVariableValue

}