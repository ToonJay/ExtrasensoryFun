// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHealthComponent;
class AStaticMeshActor;
/**
* This class is for the basic functionality for any character.
* Responsible for player inputs as well as basic data that can be useful for any type of character,
* whether it's a player, enemy or npc.
*/
UCLASS()
class EXTRASENSORYFUN_API ABaseCharacter : public ACharacter {
	GENERATED_BODY()

public:
	// Default constructor
	ABaseCharacter();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// -----Components-----
	// Spring and camera components for player characters
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;
	// Health component
	UPROPERTY(EditAnywhere, Category = "Components")
	UHealthComponent* Health;
	// Sound components
	UPROPERTY(EditAnywhere, Category = "Combat")
	class USoundBase* DeathSound;

	// ----Camera-----
	FHitResult Target;
	float LockOnDistanceLimit = 2400.f;
	FVector PositionFromChar(UPrimitiveComponent* Component) const;
	virtual void TargetLockOn(); // virtual since Targetting will have difference effects depending on the character in use
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to player input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handle character death
	void HandleDeath();

	// Reset targeting for player
	void ResetTargeting();

	// Getter methods
	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UCameraComponent* GetCamera() const { return Camera; }
	UFUNCTION(BlueprintPure)
	FHitResult GetTarget() const { return Target; }

private:
	// -----Character movement-----
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);

	// -----Camera-----
	void LookRightRate(float AxisValue);
	UPROPERTY(EditAnywhere, Category = "Components")
	float RotationRate = 70.f;
	void ZoomCamera(float AxisValue);
	void ZoomCameraRate(float AxisValue);
	void CenterCameraBehindCharacter();
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* TargetArrowMesh;
	AStaticMeshActor* TargetArrow;
};