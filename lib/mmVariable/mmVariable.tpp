template <class T>
mmVariable<T>::mmVariable(ExecutionEnv *_executionEnv, mmVariablePersistencyValues<T> data)
{
    this->executionEnv = _executionEnv;
    this->value = data.value;
}

template <class T>
bool mmVariable<T>::SetValue(T value)
{
    if (this->value != value)
    {
        this->value = value;
        return true;
    }
    return false;
}

template <class T>
void mmVariable<T>::GetPersistencyValues(mmVariablePersistencyValues<T> &persistencyValues) const
{
    persistencyValues.value = this->value;
}

/// @brief Serialize persistency values into a byte vector
/// @param data Output vector that will receive the serialized bytes
template <class T>
void mmVariable<T>::GetBytesFromData(std::vector<uint8_t> &data) const
{
    mmVariablePersistencyValues<T> persistencyValues;
    GetPersistencyValues(persistencyValues);

    size_t size = sizeof(mmVariablePersistencyValues<T>);
    data.resize(size);

    memcpy(data.data(), &persistencyValues, size);
}

template <class T>
void mmVariable<T>::SetDataFromBytes(std::vector<uint8_t> &data)
{
    if (data.size() != sizeof(mmVariablePersistencyValues<T>))
        return;

    mmVariablePersistencyValues<T> persistencyValues;
    memcpy(&persistencyValues, data.data(), sizeof(mmVariablePersistencyValues<T>));

    this->value = persistencyValues.value;
}

#pragma region mmVariableValue
template <class T>
mmVariableValue mmVariable<T>::GetVariantValue() const
{
    // We return the value as a mmVariableValue variant, this will allow to access to the variable value without knowing its type, it will be used for example in the web interface to display variable values in a generic way
    return mmVariableValue{value};
}

template <class T>
bool mmVariable<T>::SetVariantValue(const mmVariableValue &v)
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

/*#pragma region IObjectDTO
template <class T>
bool mmVariable<T>::GetDTO(DTOBase &dto) const
{
    dto.objectName = "mmVariable"; // Generic name here as the Object didn't know its name
    dto.fields.push_back({"value", GetVariantValue()});
    return true;
}

template <class T>
bool mmVariable<T>::GetDetailDTO(DTOBase &dto) const
{
    if (GetDTO(dto))
    {
        dto.fields.push_back({"type", GetDataType(value)});
        return true;
    }
    return false;
}
#pragma endregion IObjectDTO*/