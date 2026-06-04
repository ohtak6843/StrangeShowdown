// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/STRoomInfoObject.h"
#include "Engine/GameInstance.h"

#include "Containers/Map.h"
#include "Protocol.h"
#include "Character/Player/STPlayerBase.h"
#include "Character/Player/STFieldPlayer.h"


#include "STGameInstance.generated.h"

class SocketIO;
class STPacketHandler;
class USTNetworkManager;
class USTDataManager;

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoomListUpdated);

UCLASS()
class STRANGESHOWDOWN_API USTGameInstance : public UGameInstance
{
	GENERATED_BODY()

	// --
	// client variables and methods
	// --
public:
	// 방 목록이 업데이트되었음을 알리는 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnRoomListUpdated OnRoomListUpdated;

	// 방 목록을 저장하는 배열
	UPROPERTY(BlueprintReadWrite)
	TArray<USTRoomInfoObject*> RoomList;

	// 해상도
	UPROPERTY(BlueprintReadWrite)
	FIntPoint ScreenResolution;

	// 창모드
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EWindowMode::Type> WindowMode = EWindowMode::Windowed;
	
	// 볼륨 설정
	UPROPERTY(BlueprintReadWrite)
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float BGMVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite)
	float SFXVolume = 1.0f;

	// 새로운 방을 추가
	void AddRoom(USTRoomInfoObject* NewRoom);


	// --
	// virtual method
	// --
public:
	virtual void Init() override;
	virtual void OnStart() override;
	virtual void Shutdown() override;


	// --
	// packet handler
	// --

	// 정적
public:
	void HandleSpawn(const Common::SCSpawnObject& Packet);
	void HandleMove(const Common::SCMovePlayer& Packet);
	void HandleCreateRoom(const Common::SCCreateRoom& Packet);
	void HandleJoinRoom(const Common::SCJoinRoom& Packet);
	void HandleReady(const Common::SCReady& Packet);
	void HandleStartGame(const Common::SCStartGame& Packet);
	void HandleUseItem(const Common::SCUseItem& Packet);
	void HandleStatusUpdate(const Common::SCStatusUpdate& Packet);

	// 동적
public:
	void HandleGiveRoomList(const Common::SCGiveRoomList& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);
	void HandleChat(const Common::SCChat& Packet, const uint8* PayloadPtr, const uint16 PayloadSize);

	// --
	// network method
	// --
public:
	UFUNCTION(BlueprintCallable, Category = "Network")
	void GetRoomList();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void JoinRoom(const int64 RoomID, const FText& Password = FText::GetEmpty());

	UFUNCTION(BlueprintCallable, Category = "Network")
	void CreateRoom(const FText& Name, const FText& Password);

	UFUNCTION(BlueprintCallable, Category = "Network")
	void ChangeWorld(const FText& Level);

	void Chat(const FText& Message);

	void Ready(bool Value);

	void StartGame();
	
	void UseItem(uint64 TargetID, Common::ItemType ItemID);

	void PickUpItem(Common::ItemType ItemID);





	// --
	// network method (exec)
	// --
public:
	UFUNCTION(Exec)
	void DevGetRoomList();

	UFUNCTION(Exec)
	void DevJoinRoom(const uint32 RoomID);

	UFUNCTION(Exec)
	void DevCreateRoom(const FString& RoomName, const FString& Password);

	UFUNCTION(Exec)
	void DevChangeWorld(const FString& Level);

	UFUNCTION(Exec)
	void DevChat(const FString& Message);

	UFUNCTION(Exec)
	void DevReady(bool Ready);

	UFUNCTION(Exec)
	void DevStartGame();

	UFUNCTION(Exec)
	void DevUseItem(uint32 TargetID, uint32 ItemType);


	// --
	// util method
	// --
public:
	inline void SendPacket(const TArray<uint8>& data);


	// 레벨이 로드된 후 호출되는 함수.
	// 새로운 객체를 만들고, 기존 객체를 초기화하는 등의 작업을 수행.
	void OnLevelLoaded(UWorld* LoadedWorld);


	// --
	// getter and setter
	// --
public:
	USTDataManager* GetDataManager() const { return DataManager; }


	// --
	// manager variable
	// --
private:
	UPROPERTY()
	USTNetworkManager* NetworkManager{};

	UPROPERTY(EditAnywhere, Instanced, Category = "Manager")
	USTDataManager* DataManager{};
};
