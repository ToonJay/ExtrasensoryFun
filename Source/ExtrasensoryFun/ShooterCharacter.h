// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "ShooterCharacter.generated.h"

class AShooterWeapon;

/**
 * Adds shooter functionality to a character.
 * This class will be complemented by the ShooterWeapon and ShooterProjectile classes.
 */
UCLASS()
class EXTRASENSORYFUN_API AShooterCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called to bind functionality to player input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Fire weapon
	void Shoot();

private:
	// Weapon class
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AShooterWeapon> ShooterWeaponClass;
	UPROPERTY()
	AShooterWeapon* ShooterWeapon;
};
