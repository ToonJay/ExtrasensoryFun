// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "ESPCharacter.generated.h"

// -----Telekinesis properties used for the telekinesis functions-----
USTRUCT()
struct FTelekinesis {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	int ObjectGrabLimit;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float GrabRange;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float GrabRadius;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float ThrowAimRange;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float ThrowAimRadius;
	UPROPERTY(EditAnywhere, Category = "Telekinesis")
	float ThrowForce;

	FTelekinesis() {
		ObjectGrabLimit = 10;
		GrabRange = 401.f;
		GrabRadius = 400.f;
		ThrowAimRange = 10000.f;
		ThrowAimRadius = 1200.f;
		ThrowForce = 12000.f;
	}
};

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
	// -----Telekinesis properties and functions-----
	UPROPERTY(EditAnywhere, Category = Config)
	FTelekinesis TelekinesisConfig;
	// Functions and property for grabbing
	bool GetGrabbableObjectsInReach(TArray<FHitResult>& OutHitResults) const;
	void SortHitResults(TArray<FHitResult>& OutHitResults) const;
	bool IsGrabbing = false;
	void StartGrabbing();
	void StopGrabbing();
	void Grab();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsGrabbingObject();
	// Release all grabbed objects at once
	void Release();
	// Functions and properties for throwing
	void ThrowAim();
	bool IsFrozen = false;
	FVector FreezeLocation;
	bool ThrowAimTrace(FHitResult& OutHitResult) const;
	int GetClosestGrabbedObject(FHitResult* HitResult) const;
	int GetFarthestGrabbedObject() const;
	void Throw();
	
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
	TArray<FVector> PositionsFromChar;

	// -----Properties and functions for telekinesis FX-----
	// Particles for telekinesis casting effect
	UPROPERTY(EditAnywhere, Category = "Telekinesis FX")
	UParticleSystem* MuzzleCast;
	UParticleSystemComponent* CastEmitter;
	// Decals for objects being grabbed
	TArray<UDecalComponent*> TelekinesisDecals;
	UPROPERTY(EditAnywhere, Category = "Telekinesis FX")
	UMaterialInstance* TelekinesisDecalMaterial;
	// Attaches a decal to an object being grabbed
	void AttachTelekinesisDecal(UPrimitiveComponent* HitComponent, int Index);
};