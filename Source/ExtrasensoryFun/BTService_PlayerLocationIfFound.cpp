// by Jason Hilani


#include "BTService_PlayerLocationIfFound.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"

// Default constructor
UBTService_PlayerLocationIfFound::UBTService_PlayerLocationIfFound() {
	NodeName = TEXT("Update Player Location If Found");
}

void UBTService_PlayerLocationIfFound::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	if (OwnerComp.GetAIOwner()) {
		// If it exists, get PlayerPawn
		// Otherwise, clear blackboard value
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
			// If the player pawn is within the AI's line of sight, set the location of the player for the AI to move to.
			if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn)) {
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
			} else {
				OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey()); // Clear value when PlayerPawn is out of sight.
			}
		} else {
			OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey()); // Clear value when PlayerPawn doesn't exist anymore
		}
	}
}
