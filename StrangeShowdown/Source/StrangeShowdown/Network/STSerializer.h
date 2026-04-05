// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * Socket I/O Ελ½Ε
 */
class STRANGESHOWDOWN_API STSerializer
{
public:
	STSerializer();
	~STSerializer();

	template <typename T>
	static TArray<uint8> Serialize(const T& Data)
	{
		TArray<uint8> OutBuffer;
		OutBuffer.AddUninitialized(sizeof(T));
		FMemory::Memcpy(OutBuffer.GetData(), &Data, sizeof(T));
		return OutBuffer;
	}

	template <typename T, typename AT>
	static TArray<uint8> Serialize(const T& Data, const TArray<AT>& AdditionalData)
	{
		uint32 Size{ sizeof(T) + sizeof(AT) * AdditionalData.Num() };
		TArray<uint8> OutBuffer;
		OutBuffer.AddUninitialized(Size);

		FMemory::Memcpy(OutBuffer.GetData(), &Data, sizeof(T));
		FMemory::Memcpy(OutBuffer.GetData() + sizeof(T), AdditionalData.GetData(), sizeof(AT) * AdditionalData.Num());
		return OutBuffer;
	}

	template <typename T>
	static T Deserialize(const TArray<uint8>& Buffer)
	{
		T OutData{};
		FMemory::Memcpy(&OutData, Buffer.GetData(), sizeof(T));
		return OutData;
	}

};