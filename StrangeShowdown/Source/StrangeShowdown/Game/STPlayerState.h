#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Component/STMissionComponent.h"
#include "STPlayerState.generated.h"

UCLASS()
class STRANGESHOWDOWN_API ASTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASTPlayerState();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mission")
	USTMissionComponent* MissionComponent;
};