// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "ESPCharacter.generated.h"

// Struct for telekinesis properties
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
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to player input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called when movement mode changes
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) override;

private:
	// -----Telekinesis-----
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
	
	// -----Jumping-----
	void Jumping();
	int JumpCount = 0;
	float JumpTimer = 0.f;
	float JumpTime = 0.2f;

	// -----Physics-----
	UPROPERTY(EditAnywhere, Category = "Physics Handles")
	TArray<class UPhysicsHandleComponent*> PhysicsHandles;
	UPROPERTY(EditAnywhere, Category = "Physics Handles")
	float InterpolationSpeed = 50.f;
	// Grabbed objects' relative positions from the character
	UPROPERTY(VisibleAnywhere, Category = "Physics Handles")
	TArray<FVector> PositionsFromChar;

	// -----Telekinesis FX-----
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

	// -----Camera-----
	void CenterCameraBehindCharacter();
};