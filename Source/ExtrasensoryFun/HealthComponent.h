// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class EXTRASENSORYFUN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Default constructor
	UHealthComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Health properties
	UPROPERTY(EditAnywhere)
	float MaxHealth = 100.f;
	float Health = 0.f;
	UPROPERTY(EditAnywhere)
	bool CanDie = true;

	// Process damage taken
	UFUNCTION()
	void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* Instigator, AActor* DamageCauser);

	// Game mode
	class AExtrasensoryFunGameMode* ExtrasensoryFunGameMode;
};