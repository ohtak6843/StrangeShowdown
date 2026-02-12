#include "Game/STPlayerState.h"

ASTPlayerState::ASTPlayerState()
{
	MissionComponent = CreateDefaultSubobject<USTMissionComponent>(TEXT("MissionComponent"));
}