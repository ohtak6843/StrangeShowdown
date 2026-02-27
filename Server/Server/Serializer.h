#pragma once

class Serializer
{
public:
    Serializer();
    ~Serializer();

    static SendBuffer Serialize(char* data, const size_t size);

    template <typename T>
    static SendBuffer Serialize(const T& data)
    {
        SendBuffer out_buffer(sizeof(T));
        memcpy(out_buffer.data(), &data, sizeof(T));
        return out_buffer;
    }

    template <typename T, typename AT>
    static SendBuffer Serialize(const T& data, const std::vector<AT>& additional_data)
    {
        SendBuffer out_buffer(sizeof(T) + sizeof(AT) * additional_data.size());
        memcpy(out_buffer.data(), &data, sizeof(T));
        memcpy(out_buffer.data() + sizeof(T), additional_data.data(), sizeof(AT) * additional_data.size());
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

