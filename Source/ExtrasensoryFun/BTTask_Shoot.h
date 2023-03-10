// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Shoot.generated.h"

/**
 * BT Task for shooting the player.
 */
UCLASS()
class EXTRASENSORYFUN_API UBTTask_Shoot : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	// Default constructor
	UBTTask_Shoot();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
