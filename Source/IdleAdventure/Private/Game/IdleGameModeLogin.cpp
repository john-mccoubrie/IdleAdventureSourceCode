


#include "Game/IdleGameModeLogin.h"
#include <Kismet/GameplayStatics.h>

void AIdleGameModeLogin::BeginPlay()
{
    LoginGameChatManagerInstance = GetWorld()->SpawnActor<AGameChatManager>(AGameChatManager::StaticClass());
}

void AIdleGameModeLogin::BeginDestroy()
{
    if (LoginGameChatManagerInstance)
    {
        LoginGameChatManagerInstance->Destroy();
        LoginGameChatManagerInstance = nullptr;
    }

    Super::BeginDestroy();
}

void AIdleGameModeLogin::LoadIdleForestLevel()
{
    //UGameplayStatics::OpenLevel(this, TEXT("IdleForest"));
}
