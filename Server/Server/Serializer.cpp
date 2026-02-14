#include "pch.h"
#include "Serializer.h"

Serializer::Serializer()
{
}

Serializer::~Serializer()
{
}

SendBuffer Serializer::Serialize(char* data, const size_t size)
{
	SendBuffer out_buffer(size);
	memcpy(out_buffer.data(), data, size);
    return out_buffer;
}

