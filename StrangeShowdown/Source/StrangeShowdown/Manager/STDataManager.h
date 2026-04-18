// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "protocol.h"
#include "Character/STCharacter.h"

#include "STDataManager.generated.h"

class ASTFieldPlayer;

/**
* @brief:
*  레벨이 바뀌어도 유지되는 플레이어 정보를 담는 구조체
*  DataManager에서 관리해 서버와 연동된다.
*/
struct FPlayerInfo
{
	// 이 정보를 가지고 있는 현재 레벨의 플레이어 객체
	ASTCharacter* Player;

	FString NickName;

	uint64 PlayerID;
};

/**
* @brief:
*  서버와 연동되는 클라이언트의 정보관리
*/
UCLASS()
class STRANGESHOWDOWN_API USTDataManager : public UObject
{
	GENERATED_BODY()

public:

	// --
	// 패킷 처리 함수들
	// --

	void HandleSpawn(TSubclassOf<ASTFieldPlayer> OtherPlayerClass, const Common::SCSpawnObject& Packet);
	void HandleMove(const Common::SCMovePlayer& Packet);
	//void HandleCreateRoom(const Common::SCCreateRoom& Packet);
	//void HandleJoinRoom(const Common::SCJoinRoom& Packet);
	//void HandleReady(const Common::SCReady& Packet);
	//void HandleStartGame(const Common::SCStartGame& Packet);

	// 동적
	//void HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);
	//void HandleChat(const Common::SCChat& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);


	void ChangeMap(const FString& MapName);

	// todo cham: 나중에 private화.
public:

	// 가비지 컬렉터 사라짐 방지 위해 UPROPERTY로 선언
	UPROPERTY()
	TMap<uint64, ASTFieldPlayer*> PlayerMap{};

	// 가비지 컬렉터 사라짐 방지 위해 UPROPERTY로 선언
	//UPROPERTY(VisibleAnywhere, Category = "Data")
	//TMap<uint64, FPlayerInfo*> PlayerInfoMap{};
};
