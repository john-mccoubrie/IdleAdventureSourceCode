
// QuestEnums.h

#pragma once

#include "CoreMinimal.h"
#include "QuestEnums.generated.h"

UENUM(BlueprintType)
enum class EQuestState : uint8
{
    NotStarted UMETA(DisplayName = "Not Started"),
    InProgress UMETA(DisplayName = "In Progress"),
    ReadyToTurnIn UMETA(DisplayName = "Ready to Turn In"),
    Completed UMETA(DisplayName = "Completed")
};