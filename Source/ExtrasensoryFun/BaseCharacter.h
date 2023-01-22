// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UHealthComponent;
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

	FHitResult Target;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to player input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handle character death
	void HandleDeath();

	// Getter methods
	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UCameraComponent* GetCamera() const { return Camera; }

private:
	// -----Functions and properties for player character controls-----
	// Character movement
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	// Camera rotation for gamepads only
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);
	UPROPERTY(EditAnywhere, Category = "Components")
	float RotationRate = 70.f;

	// -----Camera-----
	void CenterCameraBehindCharacter();
	void TargetLockOn();
	
};