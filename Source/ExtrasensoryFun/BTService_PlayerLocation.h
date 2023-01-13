// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_PlayerLocation.generated.h"

/**
 * BT Service for constantly updating the player's location when within the AI's line of sight.
 */
UCLASS()
class EXTRASENSORYFUN_API UBTService_PlayerLocation : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	// Default constructor
	UBTService_PlayerLocation();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
