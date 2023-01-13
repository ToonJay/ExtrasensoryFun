// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_PlayerLocationIfFound.generated.h"

/**
 * BT Service to get the player's location when within line of sight of the AI.
 */
UCLASS()
class EXTRASENSORYFUN_API UBTService_PlayerLocationIfFound : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
public:
	// Default constructor
	UBTService_PlayerLocationIfFound();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
