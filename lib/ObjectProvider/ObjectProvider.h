#pragma once
#include <unordered_map>
#include <memory>
#include <string>

template <typename T>
class ObjectProvider
{
protected:
    ExecutionEnv *executionEnv;

private:
    struct Entry
    {
        // Modbus address associated to the object, we store it here to be able to retrieve it later if needed
        uint32_t address;
        std::unique_ptr<T> object;
    };

    std::unordered_map<std::string, Entry> objects;

public:
    // -------- CREATE --------
    template <typename... Args>
    T *Create(const std::string &name, uint32_t address, Args &&...args)
    {
        if (objects.find(name) != objects.end())
            return nullptr;

        Entry entry;
        entry.address = address;
        entry.object = std::unique_ptr<T>(new T(std::forward<Args>(args)...));

        T *raw = entry.object.get();
        objects[name] = std::move(entry);
        return raw;
    }

    /// @brief Getter function to get the desired object given by his name
    /// @param name Name of the object to be retrieived
    /// @return The Object
    T *Get(const std::string &name) const
    {
        auto it = objects.find(name);
        if (it == objects.end())
            return nullptr;

        return it->second.object.get();
    }

    /// @brief Getter function to get the desired object given by his name
    /// @param name Name of the object to be retrieived
    /// @return The Object
    T *Get(const std::string &name, int32_t &address) const
    {
        auto it = objects.find(name);
        if (it == objects.end())
        {
            address = -1;
            return nullptr;
        }

        address = it->second.address;
        return it->second.object.get();
    }

    int32_t getAddress(const std::string &name) const
    {
        auto it = objects.find(name);
        if (it == objects.end())
            return -1;

        return it->second.address;
    }

    bool exists(const std::string &name) const
    {
        return objects.find(name) != objects.end();
    }

    size_t size() const
    {
        return objects.size();
    }

    bool remove(const std::string &name)
    {
        return objects.erase(name) > 0;
    }

    void clear()
    {
        objects.clear();
    }

    bool inject(const std::string &name, uint32_t address, T *raw)
    {
        if (objects.find(name) != objects.end())
            return false;

        Entry entry;
        entry.address = address;
        entry.object = std::unique_ptr<T>(raw);
        objects[name] = std::move(entry);
        return true;
    }

    /// @brief Function that iterate over all objects of the provider and apply the given function on them
    /// @param fn Function to be applied on each object, it receive as parameters the name of the object and a pointer to the object
    void ForEach(std::function<void(const std::string &, T *)> fn) const
    {
        for (const auto &pair : objects)
            fn(pair.first, pair.second.object.get());
    }

    // -------- ITERATORS --------
    typedef typename std::unordered_map<std::string, Entry>::iterator iterator;
    typedef typename std::unordered_map<std::string, Entry>::const_iterator const_iterator;

    iterator begin() { return objects.begin(); }
    iterator end() { return objects.end(); }

    const_iterator begin() const { return objects.begin(); }
    const_iterator end() const { return objects.end(); }
};