// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "protocol.h"

#include "STDataManager.generated.h"


class ASTCharacter;

using CharacterWeakPtr = TWeakObjectPtr<ASTCharacter>;

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
	CharacterWeakPtr Player;

	FString NickName{ UTF8_TO_TCHAR(std::string(Common::PlayerConstants::Name).c_str()) };

	uint64 ID{ 0 };

	bool bIsHost{ false };

	bool bIsReady{ false };

	Common::PlayerType Type{ Common::PlayerType::None };
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
	void HandleSpawnPlayer(const Common::SCSpawnPlayer& Packet);
	void HandleDespawnPlayer(const Common::SCDespawnPlayer& Packet);
	void HandleMove(const Common::SCMovePlayer& Packet);
	void HandleCreateRoom(const Common::SCCreateRoom& Packet);
	void HandleJoinRoom(const Common::SCJoinRoom& Packet);
	void HandleReady(const Common::SCReady& Packet);
	void HandleStartGame(const Common::SCStartGame& Packet);
	void HandleUseItem(const Common::SCUseItem& Packet);
	void HandleStatusUpdate(const Common::SCStatusUpdate& Packet);

	// 동적
	//void HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);
	//void HandleChat(const Common::SCChat& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);


	// --
	// method
	// --
public:
	// 레벨이 바뀌었을 때 기존 플레이어 객체들을 새로 만드는 함수.
	void OnLevelChanged();


private:
	// 레벨이 바뀌었을 때 기존 플레이어 객체들을 새로 만드는 함수.
	void RefreshPlayers();
	// 본인 플레이어를 호스트 플레이어로 변경
	void InitController();
	// 레벨이 로드 완료된 후 본인의 플레이어 객체를 가져오는 함수
	void GetMyPlayer();


	// --
	// getter and setter
	// --
public:

	uint64 GetHostID() const { return HostID; }

	FPlayerInfo GetMyPlayerInfo() const { return MyPlayerInfo; }


	// --
	// 내부 메소드
	// --
private:
	// ID로 PlayerInfo를 반환하는 함수
	FPlayerInfo* GetPlayerInfo(const uint64 ID);
	// 플레이어 객체를 반환하는 함수

	ASTCharacter* SpawnFieldPlayer(
		const FTransform& Transform,
		const FActorSpawnParameters& SpawnParams,
		const FPlayerInfo& PlayerInfo
	);
	
	ASTCharacter* SpawnFieldPlayer(
		const FTransform& Transform,
		const FActorSpawnParameters& SpawnParams,
		const FPlayerInfo& PlayerInfo,
		const Common::PlayerType PlayerType
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

	// 다른 플레이어가 저장되어 있는 맵
	UPROPERTY()
	TMap<uint64, FPlayerInfo> PlayerInfoMap{};

	// 본인 플레이어 정보
	UPROPERTY()
	FPlayerInfo MyPlayerInfo{};


	// --
	// spawn data (blueprint)
	// --

public:
	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTCharacter> FieldPlayerClass;

	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTCharacter> LobbyFieldPlayerClass;

	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTCharacter> FieldGhostClass;

	UPROPERTY(EditAnywhere, Category = "SpawnData")
	TSubclassOf<ASTCharacter> LocalGhostClass;



};
