// by Jason Hilani


#include "ShooterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AShooterAIController::BeginPlay() {
	Super::BeginPlay();
	// Assign behaviour tree and pawn's start location and rotation
	if (AIBehavior) {
		RunBehaviorTree(AIBehavior);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
		GetBlackboardComponent()->SetValueAsRotator(TEXT("StartRotation"), GetPawn()->GetActorRotation());
	}
}

void AShooterAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}