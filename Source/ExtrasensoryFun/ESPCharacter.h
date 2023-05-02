// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "ESPCharacter.generated.h"

// Struct for telekinesis properties
USTRUCT()
struct FTelekinesis {

	GENERATED_USTRUCT_BODY()

	// Necessary properties for grabbing and throwing
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

	// Default values
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

	// Getter Methods
	bool GetIsFrozen() { return IsFrozen; }
	bool GetIsAiming() { return IsAiming; }

	// Setter Methods
	void SetIsFrozen(bool bIsFrozen) { IsFrozen = bIsFrozen; }
	void SetIsAiming(bool bIsAiming) { IsAiming = bIsAiming; }
	UFUNCTION(BlueprintCallable)
	void SetFootstepSound(USoundBase* NewFootstepSound) { FootstepSound = NewFootstepSound; }

	// Blueprint functions to call from CPP
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartAiming();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StopAiming();

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
	float AimTimer = 0.f;
	float AimTime = 0.5f;
	bool AimByHolding = false;
	bool IsAiming = false;
	FVector FreezeLocation;
	bool ThrowAimTrace(FHitResult& OutHitResult) const;
	int GetClosestGrabbedObject(FHitResult* HitResult) const;
	int GetFarthestGrabbedObject() const;
	void Throw();
	void CancelAim();
	virtual void TargetLockOn() override;
	
	
	// -----Jumping-----
	// Jump Properties
	void Jumping();
	int JumpCount = 0;
	float JumpTimer = 0.f;
	float JumpTime = 0.2f;
	// Jump VFX
	UPROPERTY(EditAnywhere)
	UParticleSystem* SecondJumpFX;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ThirdJumpFX;
	UParticleSystemComponent* JumpEmitterLeft1;
	UParticleSystemComponent* JumpEmitterRight1;
	UParticleSystemComponent* JumpEmitterLeft2;
	UParticleSystemComponent* JumpEmitterRight2;

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
	UPROPERTY(EditAnywhere, Category = "Telekinesis FX")
	UParticleSystem* MuzzleAim;
	UPROPERTY(EditAnywhere, Category = "Telekinesis FX")
	UParticleSystem* MuzzleGlow;
	UParticleSystemComponent* CastEmitter;
	UParticleSystemComponent* AimEmitter;
	UParticleSystemComponent* GlowEmitter;
	// Decals for objects being grabbed
	TArray<UDecalComponent*> TelekinesisDecals;
	UPROPERTY(EditAnywhere, Category = "Telekinesis FX")
	UMaterialInstance* TelekinesisDecalMaterial;
	// Attaches a decal to an object being grabbed
	void AttachTelekinesisDecal(UPrimitiveComponent* HitComponent, int Index);

	// Footsteps sound
	UPROPERTY(EditAnywhere, Category = "Sound FX")
	USoundBase* FootstepSound;
	float FootstepTimer = 0.f;
	float FootstepTime = 0.25f;
};