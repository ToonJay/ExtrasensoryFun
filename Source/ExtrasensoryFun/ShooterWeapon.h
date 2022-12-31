// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterWeapon.generated.h"

class AShooterProjectile;
class AShooterCharacter;

/**
 * Responsible for the shooter character's weapons.
 * For each new weapon, we'll create a blueprint from this class.
 */
UCLASS()
class EXTRASENSORYFUN_API AShooterWeapon : public AActor {
	GENERATED_BODY()
	
public:	
	// Default constructor
	AShooterWeapon();

	// Spawn/shoot projectile
	void FireWeapon();

	// Getter methods
	UStaticMeshComponent* GetWeaponMesh() { return WeaponMesh; }
	AController* GetOwnerController() const { return GetOwner()->GetInstigatorController(); }

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AShooterProjectile> ShooterProjectileClass;
};
