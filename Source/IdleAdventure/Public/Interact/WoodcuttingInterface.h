

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WoodcuttingInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWoodcuttingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IDLEADVENTURE_API IWoodcuttingInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual int32 GetPlayerLevel();

};
