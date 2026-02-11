#pragma once

class Serializer
{
public:
    Serializer();
    ~Serializer();

    template <typename T>
    static std::vector<int8> Serialize(const T& data)
    {
        std::vector<int8> out_buffer(sizeof(T));
        memcpy(out_buffer.data(), &data, sizeof(T));
        return out_buffer;
    }

    template <typename T>
    static T Deserialize(const std::vector<int8>& buffer)
    {
        T out_data{};
        memcpy(&out_data, buffer.data(), sizeof(T));
        return out_data;
    }

};

