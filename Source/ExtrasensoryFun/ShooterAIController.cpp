// by Jason Hilani


#include "ShooterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AShooterAIController::BeginPlay() {
	Super::BeginPlay();
	// Assign behaviour tree and pawn's start location
	if (AIBehavior) {
		RunBehaviorTree(AIBehavior);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
}

void AShooterAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}