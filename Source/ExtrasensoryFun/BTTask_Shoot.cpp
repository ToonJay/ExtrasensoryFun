// by Jason Hilani


#include "BTTask_Shoot.h"
#include "AIController.h"
#include "ShooterCharacter.h"

// Default constructor
UBTTask_Shoot::UBTTask_Shoot() {
	NodeName = "Shoot";
}

EBTNodeResult::Type UBTTask_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// Check if the OwnerComp is an AI
	// If not, return EBTNodeResult::Failed
	if (OwnerComp.GetAIOwner()) {
		// Check if OwnerComp is a ShooterCharacter
		if (AShooterCharacter* Character = Cast<AShooterCharacter>(OwnerComp.GetAIOwner()->GetPawn())) {
			// Shoot player after getting within the acceptable amount of distance.
			Character->Shoot();
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}