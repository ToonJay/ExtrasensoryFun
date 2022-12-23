// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "ESPCharacter.generated.h"

/**
 * This class adds the telekinesis functionality to a character.
 * All properties and functions related to telekinesis functionality will be here.
 */
UCLASS()
class EXTRASENSORYFUN_API AESPCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Default constructor
	AESPCharacter();
	// Called to bind functionality to player input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// -----Telekinesis functions-----
	// Sphere sweep to find objects the character can grab
	bool GetGrabbableObjectsInReach(TArray<FHitResult>& OutHitResults) const;
	// Sorts hit results from the above sphere sweep in ascending distance from the character
	void SortHitResults(TArray<FHitResult>& OutHitResults) const;
	void Grab();
	void Release();
	void Throw();

	// -----Telekinesis properties used for the telekinesis functions-----
	UPROPERTY(VisibleAnywhere, Category = "Telekinesis")
	int ObjectGrabLimit = 10;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float GrabRange = 401;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float GrabRadius = 400;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float ThrowForce = 6000;

	// -----Properties related to the physics handles used for the telekinesis functions-----
	//Array of physics handle components that will be attached to the character.
	//Need a physics handle for each object we want to grab and manipulate at the same time.
	UPROPERTY(EditAnywhere, Category = "Physics Handles")
	TArray<class UPhysicsHandleComponent*> PhysicsHandles;
	// Interpolation speed of every physics handle component
	UPROPERTY(EditAnywhere, Category = "Physics Handles")
	float InterpolationSpeed = 50.f;
	// Relative position from the character for each object it's grabbing 
	UPROPERTY(VisibleAnywhere, Category = "Physics Handles")
	TArray<FVector> PositionsFromCharacter;
};