// by Jason Hilani


#include "BTService_PlayerLocationIfFound.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"

UBTService_PlayerLocationIfFound::UBTService_PlayerLocationIfFound() {
	NodeName = TEXT("Update Player Location If Found");
}

void UBTService_PlayerLocationIfFound::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	// Get player pawn
	if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) {
		if (OwnerComp.GetAIOwner()) {
			// If the player pawn is within the AI's line of sight, set the location of the player for the AI to move to.
			if (OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn)) {
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(GetSelectedBlackboardKey(), PlayerPawn);
			} else {
				OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey()); // Clear value when player is out of sight.
			}
		}
	}
}
