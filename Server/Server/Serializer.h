#pragma once

class Serializer
{
public:
    Serializer();
    ~Serializer();

    template <typename T>
    static SendBuffer Serialize(const T& data)
    {
        SendBuffer out_buffer(sizeof(T));
        memcpy(out_buffer.data(), &data, sizeof(T));
        return out_buffer;
    }

    template <typename T>
    static T Deserialize(const RecvBuffer& buffer)
    {
        T out_data{};
        memcpy(&out_data, buffer.data(), sizeof(T));
        return out_data;
    }

};

