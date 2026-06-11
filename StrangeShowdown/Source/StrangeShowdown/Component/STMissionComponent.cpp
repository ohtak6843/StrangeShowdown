#include "Component/STMissionComponent.h"
#include "Net/UnrealNetwork.h"

USTMissionComponent::USTMissionComponent()
{
	SetIsReplicatedByDefault(true);
}

void USTMissionComponent::AddMission()
{
	// TODO: 이 시점에서 서버에서 몇 번 미션인지 받아옴
	// 임시로 랜덤으로 설정
	EMissionType MissionType = static_cast<EMissionType>(FMath::RandRange(0, static_cast<int32>(EMissionType::Test)));

	switch(MissionType)
	{
		case EMissionType::Attack:
			OnMissionStart.Broadcast(SetAttack());
			break;
		case EMissionType::GainGold:
			OnMissionStart.Broadcast(SetGainGold());
			break;
		case EMissionType::DestroyDoor:
			OnMissionStart.Broadcast(SetDestroyDoor());
			break;
		case EMissionType::DamageSheriff:
			OnMissionStart.Broadcast(SetDamageSheriff());
			break;
		case EMissionType::SurviveWithoutAttack:
			OnMissionStart.Broadcast(SetSurviveWithoutAttack());
			break;
		case EMissionType::BuyItem:
			OnMissionStart.Broadcast(SetBuyItem());
			break;
		case EMissionType::Test:
			OnMissionStart.Broadcast(SetTest());
			break;
	}
}

void USTMissionComponent::ClearMission(int32 Index)
{
	if (!Missions.IsValidIndex(Index))
		return;

	USTMissionRowData* Data = Missions[Index];

	OnMissionClear.Broadcast(Data);

	Missions.RemoveAt(Index);
}

void USTMissionComponent::UpdateMissionProgress(int32 Index, int32 ProgressToAdd)
{
	if (!Missions.IsValidIndex(Index))
		return;
	USTMissionRowData* Data = Missions[Index];
	Data->CurrentProgress += ProgressToAdd;
	Data->MissionProgress = FText::FromString(FString::Printf(TEXT("진행도: %d / %d"), Data->CurrentProgress, Data->MaxProgress));
	if (Data->CurrentProgress >= Data->MaxProgress)
	{
		ClearMission(Index);
	}

	OnMissionUpdate.Broadcast(Data);
}

void USTMissionComponent::AddProgress(USTMissionRowData* Mission)
{
	Mission->CurrentProgress++;

	if (Mission->CurrentProgress >= Mission->MaxProgress)
	{
		int32 Index = Missions.IndexOfByKey(Mission);
		if (Missions.IsValidIndex(Index))
		{
			ClearMission(Index);
		}
		return;
	}
	else
	{
		int32 Index = Missions.IndexOfByKey(Mission);
		if (Missions.IsValidIndex(Index))
		{
			OnMissionUpdate.Broadcast(Mission);
		}
		return;
	}
}

void USTMissionComponent::TestMissionProgressUp()
{
	UpdateMissionProgress(0, 1);
}

USTMissionRowData* USTMissionComponent::SetAttack()
{
	FText Title = FText::FromString(TEXT("학살자"));
	FText Desc = FText::FromString(TEXT("다른 플레이어들에게 5 이상의 피해를 준다."));
	int32 CurrentProgress = 0;
	int32 MaxProgress = 5;

	USTMissionRowData* NewData = NewObject<USTMissionRowData>(this);
	if (!NewData) return 0;

	NewData->Init(Title, Desc, EMissionType::Attack, CurrentProgress, MaxProgress);
	Missions.Add(NewData);

	return NewData;
}

USTMissionRowData* USTMissionComponent::SetGainGold()
{
	FText Title = FText::FromString(TEXT("평화주의자"));
	FText Desc = FText::FromString(TEXT("공격 포기를 통해 6골드 이상 획득한다."));
	int32 CurrentProgress = 0;
	int32 MaxProgress = 6;

	USTMissionRowData* NewData = NewObject<USTMissionRowData>(this);
	if (!NewData) return 0;

	NewData->Init(Title, Desc, EMissionType::GainGold, CurrentProgress, MaxProgress);
	Missions.Add(NewData);

	return NewData;
}

USTMissionRowData* USTMissionComponent::SetDestroyDoor()
{
	FText Title = FText::FromString(TEXT("파괴왕"));
	FText Desc = FText::FromString(TEXT("문을 3개 파괴한다."));
	int32 CurrentProgress = 0;
	int32 MaxProgress = 3;

	USTMissionRowData* NewData = NewObject<USTMissionRowData>(this);
	if (!NewData) return 0;

	NewData->Init(Title, Desc, EMissionType::DestroyDoor, CurrentProgress, MaxProgress);
	Missions.Add(NewData);

	return NewData;
}

USTMissionRowData* USTMissionComponent::SetDamageSheriff()
{
	FText Title = FText::FromString(TEXT("뼈를 주고 살을 취해"));
	FText Desc = FText::FromString(TEXT("보안관에게 1번 공격당한다."));
	int32 CurrentProgress = 0;
	int32 MaxProgress = 1;

	USTMissionRowData* NewData = NewObject<USTMissionRowData>(this);
	if (!NewData) return 0;

	NewData->Init(Title, Desc, EMissionType::DamageSheriff, CurrentProgress, MaxProgress);
	Missions.Add(NewData);

	return NewData;
}

USTMissionRowData* USTMissionComponent::SetSurviveWithoutAttack()
{
	FText Title = FText::FromString(TEXT("은둔형 외톨이"));
	FText Desc = FText::FromString(TEXT("1턴동안 공격하거나 공격받지 않는다."));
	int32 CurrentProgress = 0;
	int32 MaxProgress = 1;

	USTMissionRowData* NewData = NewObject<USTMissionRowData>(this);
	if (!NewData) return 0;

	NewData->Init(Title, Desc, EMissionType::SurviveWithoutAttack, CurrentProgress, MaxProgress);
	Missions.Add(NewData);

	return NewData;
}

USTMissionRowData* USTMissionComponent::SetBuyItem()
{
	FText Title = FText::FromString(TEXT("아이템 수집가"));
	FText Desc = FText::FromString(TEXT("상점에서 물건을 2개 구매한다."));
	int32 CurrentProgress = 0;
	int32 MaxProgress = 2;

	USTMissionRowData* NewData = NewObject<USTMissionRowData>(this);
	if (!NewData) return 0;

	NewData->Init(Title, Desc, EMissionType::BuyItem, CurrentProgress, MaxProgress);
	Missions.Add(NewData);

	return NewData;
}

USTMissionRowData* USTMissionComponent::SetTest()
{
	FText Title = FText::FromString(TEXT("테스트 미션"));
	FText Desc = FText::FromString(TEXT("아무튼 테스트 미션임"));
	int32 CurrentProgress = 0;
	int32 MaxProgress = 2;

	USTMissionRowData* NewData = NewObject<USTMissionRowData>(this);
	if (!NewData) return 0;

	NewData->Init(Title, Desc, EMissionType::Test, CurrentProgress, MaxProgress);
	Missions.Add(NewData);

	return NewData;
}

void USTMissionComponent::OnAttack()
{
	for (USTMissionRowData* Mission : Missions)
	{
		if (Mission->MissionType == EMissionType::Attack)
		{
			AddProgress(Mission);
		}
	}
}

void USTMissionComponent::OnGainGold()
{
	for (USTMissionRowData* Mission : Missions)
	{
		if (Mission->MissionType == EMissionType::GainGold)
		{
			AddProgress(Mission);
		}
	}
}

void USTMissionComponent::OnDestroyDoor()
{
	for (USTMissionRowData* Mission : Missions)
	{
		if (Mission->MissionType == EMissionType::DestroyDoor)
		{
			AddProgress(Mission);
		}
	}
}

void USTMissionComponent::OnDamageSheriff()
{
	for (USTMissionRowData* Mission : Missions)
	{
		if (Mission->MissionType == EMissionType::DamageSheriff)
		{
			AddProgress(Mission);
		}
	}
}

void USTMissionComponent::OnSurviveWithoutAttack()
{
	for (USTMissionRowData* Mission : Missions)
	{
		if (Mission->MissionType == EMissionType::Attack)
		{
			AddProgress(Mission);
		}
	}
}

void USTMissionComponent::OnBuyItem()
{
	for (USTMissionRowData* Mission : Missions)
	{
		if (Mission->MissionType == EMissionType::BuyItem)
		{
			AddProgress(Mission);
		}
	}
}

void USTMissionComponent::OnTest()
{
	for (USTMissionRowData* Mission : Missions)
	{
		if (Mission->MissionType == EMissionType::Test)
		{
			AddProgress(Mission);
		}
	}
}