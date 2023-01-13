// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ClearBlackboardValue.generated.h"

/**
 * BT Task that clears a selected blackboard value.
 */
UCLASS()
class EXTRASENSORYFUN_API UBTTask_ClearBlackboardValue : public UBTTask_BlackboardBase {
	GENERATED_BODY()

public:
	// Default constructor
	UBTTask_ClearBlackboardValue();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
