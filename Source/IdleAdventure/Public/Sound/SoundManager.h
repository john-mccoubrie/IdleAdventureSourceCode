

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "SoundManager.generated.h"

UCLASS()
class IDLEADVENTURE_API ASoundManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundManager();

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	static ASoundManager* GetInstance(UWorld* World);
	void ResetInstance();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TArray<USoundCue*> SoundCues;

private:
	static ASoundManager* SoundManagerSingletonInstance;
	void PlayRandomSound();
	FTimerHandle SoundTimerHandle;

};
