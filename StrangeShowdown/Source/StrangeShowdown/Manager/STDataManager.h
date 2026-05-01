// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "protocol.h"

#include "STDataManager.generated.h"

class ASTPlayerBase;

/**
* @brief:
*  레벨이 바뀌어도 유지되는 플레이어 정보를 담는 구조체
*  DataManager에서 관리해 서버와 연동된다.
*/
USTRUCT(BlueprintType)
struct FPlayerInfo
{
	GENERATED_BODY()

	// 이 정보를 가지고 있는 현재 레벨의 플레이어 객체
	ASTPlayerBase* Player{ nullptr };

	FString NickName{ "NONE" };

	uint64 PlayerID{ 0 };

	bool bIsHost{ false };

	bool bIsReady{ false };
};

/**
* @brief:  
*  서버와 연동되는 클라이언트의 정보관리
*/
UCLASS(Blueprintable, BlueprintType, EditInlineNew, DefaultToInstanced)

class STRANGESHOWDOWN_API USTDataManager : public UObject
{
	GENERATED_BODY()


	// --
	// static packet handlers
	// --

public:
	void HandleSpawn(const Common::SCSpawnObject& Packet);
	void HandleMove(const Common::SCMovePlayer& Packet);
	void HandleCreateRoom(const Common::SCCreateRoom& Packet);
	void HandleJoinRoom(const Common::SCJoinRoom& Packet);
	void HandleReady(const Common::SCReady& Packet);
	void HandleStartGame(const Common::SCStartGame& Packet);

	// 동적

	//void HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);
	//void HandleChat(const Common::SCChat& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);


	// --
	// method
	// --

public:
	// 레벨이 바뀌었을 때 기존 플레이어 객체들을 새로 만드는 함수.
	void RefreshPlayers();
	// 본인 플레이어를 호스트 플레이어로 변경
	void TrySetHostPlayer();


	// --
	// 내부 메소드
	// --

private:
	// PlayerID로 플레이어 객체를 반환하는 함수
	ASTPlayerBase* GetPlayer(const uint64 PlayerID) const;
	ASTPlayerBase* SpawnPlayer(
		const FTransform& Transform,
		const FActorSpawnParameters& SpawnParams,
		const uint64 PlayerID
	);

	// todo cham: 나중에 private화.



	// --
	// room variables
	// --

private:
	bool bIsInGame{ false };
	bool bIsHost{ false };
	uint64 HostID{};


	// --
	// players
	// --

private:
	UPROPERTY()
	TMap<uint64, FPlayerInfo> PlayerInfoMap{};


	// --
	// spawn data (blueprint)
	// --

public:
	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTPlayerBase> FieldPlayerClass;

	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTPlayerBase> LobbyFieldPlayerClass;

};
