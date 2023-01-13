// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShooterAIController.generated.h"

class UBehaviorTree;

/**
 * Controller for the Shooter AI.
 */
UCLASS()
class EXTRASENSORYFUN_API AShooterAIController : public AAIController {
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior;
};
