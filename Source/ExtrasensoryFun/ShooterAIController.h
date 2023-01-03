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
	
public:
	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* AIBehavior;
};
