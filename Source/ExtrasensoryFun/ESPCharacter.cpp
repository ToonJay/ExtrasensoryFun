// by Jason Hilani


#include "ESPCharacter.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/DecalActor.h"
#include "Components/DecalComponent.h"
#include "ShooterProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"

// Default constructor
AESPCharacter::AESPCharacter() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create physics handle components for the character according to ObjectGrabLimit and number them
	for (int i = 0; i < TelekinesisConfig.ObjectGrabLimit; i++) {
		CreateDefaultSubobject<UPhysicsHandleComponent>(FName("Physics Handle " + FString::FromInt(i + 1)));
	}
}

// Called when the game starts or when spawned
void AESPCharacter::BeginPlay() {
	Super::BeginPlay();

	// Assign all physics handle components to the PhysicsHandles TArray
	GetComponents(PhysicsHandles);
	// For each physics handle, set its interpolation speed and add an element to the PositionsFromCharacter and TelekinesisDecals TArrays
	for (UPhysicsHandleComponent* PhysicsHandle : PhysicsHandles) {
		PositionsFromChar.Add(FVector(0.f));
		TelekinesisDecals.Add(nullptr);
	}
	// Set emitter for character's right arm for telekinesis
	CastEmitter = UGameplayStatics::SpawnEmitterAttached(
		MuzzleCast,
		GetMesh(),
		TEXT("Muzzle_01"),
		FVector(ForceInit),
		FRotator::ZeroRotator,
		FVector(1.f),
		EAttachLocation::KeepRelativeOffset,
		false,
		EPSCPoolMethod::None,
		false
	);
	CastEmitter->SetTranslucentSortPriority(1);
	AimEmitter = UGameplayStatics::SpawnEmitterAttached(
		MuzzleAim,
		GetMesh(),
		TEXT("Muzzle_01"),
		FVector(ForceInit),
		FRotator::ZeroRotator,
		FVector(1.f),
		EAttachLocation::KeepRelativeOffset,
		false,
		EPSCPoolMethod::None,
		false
	);
	AimEmitter->SetTranslucentSortPriority(1);
	// Set glow emitter for when character uses telekinesis
	GlowEmitter = UGameplayStatics::SpawnEmitterAttached(
		MuzzleGlow,
		GetMesh(),
		TEXT("Status"),
		FVector(ForceInit),
		FRotator::ZeroRotator,
		FVector(1.f),
		EAttachLocation::KeepRelativeOffset,
		false,
		EPSCPoolMethod::None,
		false
	);
	GlowEmitter->SetTranslucentSortPriority(1);
	// Set feet emitters for the 2nd and third jumps of the triple jump
	JumpEmitterLeft1 = UGameplayStatics::SpawnEmitterAttached(
		SecondJumpFX,
		GetMesh(),
		TEXT("Foot_L"),
		FVector(ForceInit),
		FRotator::ZeroRotator,
		FVector(1.f),
		EAttachLocation::KeepRelativeOffset,
		false,
		EPSCPoolMethod::None,
		false
	);
	JumpEmitterRight1 = UGameplayStatics::SpawnEmitterAttached(
		SecondJumpFX,
		GetMesh(),
		TEXT("Foot_R"),
		FVector(ForceInit),
		FRotator::ZeroRotator,
		FVector(1.f),
		EAttachLocation::KeepRelativeOffset,
		false,
		EPSCPoolMethod::None,
		false
	);
	JumpEmitterLeft2 = UGameplayStatics::SpawnEmitterAttached(
		ThirdJumpFX,
		GetMesh(),
		TEXT("Foot_L"),
		FVector(ForceInit),
		FRotator::ZeroRotator,
		FVector(1.f),
		EAttachLocation::KeepRelativeOffset,
		false,
		EPSCPoolMethod::None,
		false
	);
	JumpEmitterRight2 = UGameplayStatics::SpawnEmitterAttached(
		ThirdJumpFX,
		GetMesh(),
		TEXT("Foot_R"),
		FVector(ForceInit),
		FRotator::ZeroRotator,
		FVector(1.f),
		EAttachLocation::KeepRelativeOffset,
		false,
		EPSCPoolMethod::None,
		false
	);
}

// Called every frame
void AESPCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// Character's speed
	FVector RelativeVelocity = GetActorRotation().UnrotateVector(GetVelocity());
	if (((FMath::Abs(RelativeVelocity.X) + FMath::Abs(RelativeVelocity.Y)) > 400)) {

		FootstepTimer -= GetWorld()->GetDeltaSeconds() / 1200 * FMath::Clamp((FMath::Abs(RelativeVelocity.X) + FMath::Abs(RelativeVelocity.Y)), 0, 1200);
		if (FootstepSound && (FootstepTimer <= 0)) {
			UGameplayStatics::PlaySoundAtLocation(this, FootstepSound, GetActorLocation());
			FootstepTimer = FootstepTime;
		}
	} else {
		FootstepTimer = FootstepTime;
	}
	
	if (GetCharacterMovement()->IsFalling()) {
		FootstepTimer = FootstepTime;
	}

	/**
	* Set the location and rotation of each grabbed object.
	* TargetLocation is always set to the relative position from the character assigned during grabbing, except for the Z axis.
	* Rotation is set to the TargetRotation of the spring arm.
	*/
	for (int i = 0; i < PhysicsHandles.Num(); i++) {
		if (UPrimitiveComponent* Component = PhysicsHandles[i]->GetGrabbedComponent()) {
			// If, while grabbing the component, it gets destroyed by an incoming projectile, release the component.
			if (Component->IsPendingKill()) {
				PhysicsHandles[i]->ReleaseComponent();
			}

			FVector FwdVector;
			FVector Location;
			/**
			* Get Location and ForwardVector to use in HandleTargetLocation.
			* Location and ForwardVector change depending on if there's a Target or not.
			*/
			if (Target.GetActor()) {
				// Component's relative position X from the character divided by the Target's
				float PosFromCharXRatio = PositionFromChar(Component).X / PositionFromChar(Target.GetComponent()).X;
				// Will become Location's Z axis
				float CompPosZ = FMath::Clamp(PosFromCharXRatio, PosFromCharXRatio, 1.f) * PositionFromChar(Target.GetComponent()).Z;
				// CompPosZ calculation changes depending on if it's negative or not
				if (CompPosZ < 0) {
					CompPosZ = GetActorLocation().Z + 90 - FMath::Clamp(-CompPosZ, PositionFromChar(Target.GetComponent()).Z, GetActorLocation().Z);
				} else {
					CompPosZ = FMath::Clamp(CompPosZ + 192.f, GetActorLocation().Z + 90.f, PositionFromChar(Target.GetComponent()).Z + 192.f);
				}
				// Location and FwdVector if there's a Target.
				Location = FVector(GetActorLocation().X, GetActorLocation().Y, CompPosZ);
				FwdVector = GetActorForwardVector();
			} else {
				// Location and FwdVector if there isn't a Target.
				Location = GetActorLocation() + FVector(0.f, 0.f, 90.f);
				FwdVector = FVector(GetActorForwardVector().X, GetActorForwardVector().Y, Camera->GetForwardVector().Z);
			}
			// Get the physics handle's target location
			FVector HandleTargetLocation = Location
				+ FwdVector * (FMath::Clamp(PositionsFromChar[i].X, 100, PositionsFromChar[i].X))
				+ GetActorRightVector() * PositionsFromChar[i].Y;
			// Set target location and rotation for the grabbed component's physics handle
			PhysicsHandles[i]->SetTargetLocationAndRotation(HandleTargetLocation, SpringArm->GetTargetRotation());
		}
	}

	// Activate the emitter when grabbing at least 1 object, deactivate when not
	if (CastEmitter) {
		if (IsGrabbingObject() && !AimEmitter->IsActive()) {
			if (!CastEmitter->IsActive()) {
				CastEmitter->Activate();
			}
		} else {
			CastEmitter->Deactivate();
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("No particle effect on MuzzleCast!"));
	}

	// Keep grabbing as long as the "Grab" action input is pressed
	if (IsGrabbing) {
		Grab();
	}
	// Unfreeze and unaim if not grabbing an object anymore
	if (!IsGrabbingObject()) {
		CancelAim();
	}
	// Freeze character and set velocity to 0 while aiming
	if (IsFrozen) {
		SetActorLocation(FreezeLocation);
		GetCharacterMovement()->Velocity = FVector(0.f);
	}
	// If aiming without a target, zoom-in spring-arm/camera
	if (IsFrozen && !Target.GetActor()) {
		SpringArm->TargetArmLength = 100.f;
		SpringArm->SocketOffset = FVector(0.f, 60.f, 0.f);
	} else {
		// Scale SpringArm's target arm length to ControllerRotation's Vector's Z axis
		if (GetController()) {
			SpringArm->TargetArmLength = 1200.f * (1 - GetController()->GetControlRotation().Vector().Z);
		}
		SpringArm->SocketOffset = FVector(0.f, 0.f, 0.f);
	}

	// If no Target, if not frozen, if not grabbing, setting movement back to normal
	if (!IsFrozen && !Target.GetActor() && !IsGrabbing) {
		GetCharacterMovement()->bOrientRotationToMovement = true;
	} else {
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}

	// Continuously substract from JumpTimer with delta seconds
	// JumpTimer constantly gets set to JumpTime in OnMovementModeChanged()
	if (JumpTimer > 0.f) {
		JumpTimer -= GetWorld()->GetDeltaSeconds();
	}
	// Whenever JumpTimer runs out, set JumpCount to 0
	if (!GetCharacterMovement()->IsFalling() && JumpTimer <= 0.f) {
		JumpCount = 0;
	}

	// Continuously substract from AimTimer with delta seconds
	// AimTimer gets set to AimTime in ThrowAim()
	// If timer runs out, and the character is frozen, but not aiming, then ThrowAim()
	if (AimTimer > 0.f && AimByHolding) {
		AimTimer -= GetWorld()->GetDeltaSeconds();
	} else if (AimTimer <= 0.f && IsFrozen && !IsAiming) {
		ThrowAim();
	}
	// Cancel Aim and Stop Aiming when not targeting, aiming or frozen
	if (!Target.GetActor() && !IsAiming && !IsFrozen) {
		CancelAim();
		StopAiming();
	}
}

// Called to bind functionality to player input
void AESPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Telekinesis player input binds
	PlayerInputComponent->BindAction(TEXT("Grab"), EInputEvent::IE_Pressed, this, &AESPCharacter::StartGrabbing);
	PlayerInputComponent->BindAction(TEXT("Grab"), EInputEvent::IE_Released, this, &AESPCharacter::StopGrabbing);
	PlayerInputComponent->BindAction(TEXT("Release"), EInputEvent::IE_Pressed, this, &AESPCharacter::Release);
	PlayerInputComponent->BindAction(TEXT("Throw"), EInputEvent::IE_Pressed, this, &AESPCharacter::ThrowAim);
	PlayerInputComponent->BindAction(TEXT("Throw"), EInputEvent::IE_Released, this, &AESPCharacter::Throw);
	PlayerInputComponent->BindAction(TEXT("CancelAim"), EInputEvent::IE_Pressed, this, &AESPCharacter::CancelAim);
	// Jumping and camera movement player input bind
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AESPCharacter::Jumping);
	
}

// Called when character's movement mode changes
void AESPCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) {
	Super::OnMovementModeChanged(PrevMovementMode, PrevCustomMode);

	// If previous movement was falling, as such returns true when landing
	if (PrevMovementMode == EMovementMode::MOVE_Falling) {
		// Reset jumptimer
		JumpTimer = JumpTime;
		// Deactivate jump fx
		if (SecondJumpFX) {
			JumpEmitterLeft1->Deactivate();
			JumpEmitterRight1->Deactivate();
		}
		if (ThirdJumpFX) {
			JumpEmitterLeft2->Deactivate();
			JumpEmitterRight2->Deactivate();
		}
	}
}

/**
* Sphere sweep to find objects the character can grab.
* We don't want the sweep to stop at the first blocking hit, so instead we
* use the overlaps to detect the grabbable objects.
* Grabbable objects are those that overlap with the Telekinesis collision trace channel.
* Uses GrabRange and GrabRadius for the sweep.
* 
* Returns true if at least 1 object/overlap is found.
*/
bool AESPCharacter::GetGrabbableObjectsInReach(TArray<FHitResult>& OutHitResults) const {
	/**
	* Start from the character's location + the GrabRadius in the forward direction of the camera.
	* This is so that the sphere always starts from the front of the character instead of within (if aiming forward).
	* We also add the CapsuleHalfHeight in proportion to how far we aim the camera up or down. Like this,
	* the sphere starts from the top of our head if we aim upwards, from the bottom of our feet if we aim downwards, and anything in-between.
	*/
	float CapsuleHalfHeight = this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector Start(0.f);
	FVector End(0.f);
	
	// Start and end vectors depend on whether or not there's a target
	if (Target.GetActor()) {
		// GrabRadius divided by Target's Position's X axis
		float XRatio = TelekinesisConfig.GrabRadius / PositionFromChar(Target.GetComponent()).X;
		// Will become Location's Z axis
		float PosZ = FMath::Clamp(XRatio, XRatio, 1.f) * PositionFromChar(Target.GetComponent()).Z;
		// CompPosZ calculation changes depending on if it's negative or not
		if (PosZ < 0) {
			PosZ = GetActorLocation().Z - FMath::Clamp(-PosZ, PositionFromChar(Target.GetComponent()).Z, GetActorLocation().Z);
		} else {
			PosZ = FMath::Clamp(PosZ, GetActorLocation().Z, PositionFromChar(Target.GetComponent()).Z);
		}
		FVector Location = FVector(GetActorLocation().X, GetActorLocation().Y, PosZ);
		Start = Location + GetActorForwardVector() * (TelekinesisConfig.GrabRadius + CapsuleHalfHeight * FMath::Abs(PositionFromChar(Target.GetComponent()).ForwardVector.Z));
		End = Location + GetActorForwardVector() * (TelekinesisConfig.GrabRange + CapsuleHalfHeight * FMath::Abs(PositionFromChar(Target.GetComponent()).ForwardVector.Z));
		FCollisionShape Sphere = FCollisionShape::MakeSphere(TelekinesisConfig.GrabRadius);
	} else {
		Start = GetActorLocation() + Camera->GetForwardVector() * (TelekinesisConfig.GrabRadius + CapsuleHalfHeight * FMath::Abs(Camera->GetForwardVector().Z));
		End = GetActorLocation() + Camera->GetForwardVector() * (TelekinesisConfig.GrabRange + CapsuleHalfHeight * FMath::Abs(Camera->GetForwardVector().Z));
	}
	
	// These parameters will make the sphere sweep inclube overlaps.
	FCollisionQueryParams Params = FCollisionQueryParams();
	Params.bFindInitialOverlaps = true;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TelekinesisConfig.GrabRadius);

	// Sphere sweep
	//DrawDebugSphere(GetWorld(), End, TelekinesisConfig.GrabRadius, 20, FColor::Red, false, 3.f); // For a visual on the sweep
	GetWorld()->SweepMultiByChannel(
		OutHitResults,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		Sphere,
		Params
	);
	// Since we're not trying to get any blocking hits, the sweep will always return false.
	// So instead, we check if we have at least one hit result since it includes overlaps thanks to our Params.
	if (OutHitResults.Num() > 0) return true;
	return false;
}

/**
* Sorts the results from GetGrabbableObjectsInReach in ascending distance from the character.
* This is so then the character always grabs the nearest objects first.
*/
void AESPCharacter::SortHitResults(TArray<FHitResult>& OutHitResults) const {
	for (int i = 0; i < OutHitResults.Num(); i++) {
		FHitResult Temp = OutHitResults[i];
		FVector ComponentLocation = Temp.GetComponent()->GetComponentLocation();
		float Distance = ComponentLocation.Distance(GetActorLocation(), ComponentLocation);
		for (int y = 0; y < i; y++) {
			FVector OtherComponentLocation = OutHitResults[y].GetComponent()->GetComponentLocation();
			float OtherDistance = OtherComponentLocation.Distance(GetActorLocation(), OtherComponentLocation);
			if (Distance < OtherDistance) {
				OutHitResults[i] = OutHitResults[y];
				OutHitResults[y] = Temp;
				Temp = OutHitResults[i];
			}
		}
	}
}

/*
* Set IsGrabbing to true.
* Center camera behind player.
* Use controller rotation yaw and don't orient rotation to movement.
*/ 
void AESPCharacter::StartGrabbing() {
	IsGrabbing = true;
	if (!Target.GetActor()) {
		GetController()->SetControlRotation(GetActorRotation());
	}
	if (GlowEmitter) {
		GlowEmitter->Activate();
	}
}

/*
* Set IsGrabbing to true.
* Orient rotation to movement and don't use controller rotation yaw.
*/
void AESPCharacter::StopGrabbing() {
	IsGrabbing = false;
	if (GlowEmitter) {
		GlowEmitter->Deactivate();
	}
}

/**
* Grab objects within GrabRange and GrabRadius.
* The character can grab as many objects as they have physics handle components.
* The character will grab the closest objects first.
*/
void AESPCharacter::Grab() {
	// Can't grab while throwing
	if (!IsFrozen) {
		TArray<FHitResult> HitResults;
		// Sphere sweep on the Telekinesis channel for any overlap hits
		if (GetGrabbableObjectsInReach(HitResults)) {
			// Sort hit results in ascending distance from the character
			SortHitResults(HitResults);
			// Iterate through hit results and get the hit result, component and actor
			for (int i = 0; i < HitResults.Num(); i++) {
				FHitResult HitResult = HitResults[i];
				UPrimitiveComponent* HitComponent = HitResult.GetComponent();
				AActor* HitActor = HitResult.GetActor();

				// For each hit result/object, iterate through the physics handle components
				for (int y = 0; y < PhysicsHandles.Num(); y++) {
					// Make sure that the physics handle is available and that the hit result/object is not already being grabbed
					if (!HitActor->ActorHasTag("Grabbed") && !PhysicsHandles[y]->GetGrabbedComponent()) {
						// Enable physics and wake up the object to make sure we can grab and manipulate it
						HitComponent->SetSimulatePhysics(true);
						HitComponent->SetNotifyRigidBodyCollision(true);
						HitComponent->WakeAllRigidBodies();
						// Detach it from any attached actor such as a trigger or an actor composed of many actors
						HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
						HitActor->Tags.Add("Grabbed"); // Useful for tracking the objects that are currently being grabbed
						HitActor->SetOwner(this);
						// Grab the component
						PhysicsHandles[y]->GrabComponentAtLocationWithRotation(
							HitComponent,
							NAME_None,
							HitResult.ImpactPoint,
							Camera->GetComponentRotation()
						);

						// Remove TrailFX and hit events if it's a ShooterProjectile
						if (AShooterProjectile* Projectile = Cast<AShooterProjectile>(HitActor)) {
							if (UParticleSystemComponent* Particles = Projectile->GetTrailFX()) {
								Particles->DestroyComponent();
							}
							Projectile->GetMesh()->SetNotifyRigidBodyCollision(false);
						}
						// Disable gravity
						HitComponent->SetEnableGravity(false);
						// Attach decal component
						AttachTelekinesisDecal(HitComponent, y);
						// Make the character ignore the collision of the object so the character doesn't get pushed around by the objects it's manipulating
						this->MoveIgnoreActorAdd(HitActor);
						// Make the grabbed objects overlap with the camera channel so the spring arm doesn't retract for grabbed objects that end up behind the character
						HitComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Overlap);
						// Record object positions relative to the character upon grabbing them
						PositionsFromChar[i] = PositionFromChar(HitComponent);
						break;
					}
				}
			}
		}
	}
}

/**
* Simply drops all the objects the character is currently manipulating.
* Checks all physics handle components for grabbed objects and releases them.
*/
void AESPCharacter::Release() {
	for (int i = 0; i < PhysicsHandles.Num(); i++) {
		if (UPrimitiveComponent* GrabbedComponent = PhysicsHandles[i]->GetGrabbedComponent()) {
			GrabbedComponent->WakeAllRigidBodies(); // In case the object is sleeping
			GrabbedComponent->GetOwner()->Tags.Remove("Grabbed");
			// Re-enable gravity
			GrabbedComponent->SetEnableGravity(true);
			PhysicsHandles[i]->ReleaseComponent();
			TelekinesisDecals[i]->DestroyComponent();
		}
	}
	CancelAim();
}

/*
* Set IsFrozen to true and get the character's location.
* Center camera behind the character.
* Use controller rotation yaw and don't orient rotation to movement.
* 
* This will be used to set the character in place and rotate it with the camera when aiming to throw.
*/ 
void AESPCharacter::ThrowAim() {
	if (IsGrabbingObject()) {
		if (!IsFrozen) {
			// Start AimTimer and AimByHolding
			AimTimer = AimTime;
			AimByHolding = true;
			// Center camera if no target
			FreezeLocation = GetActorLocation();
			if (!Target.GetActor()) {
				GetController()->SetControlRotation(GetActorRotation());
			}
			// Freeze character movement
			IsFrozen = true;
			// Deactivate jump fx
			if (SecondJumpFX) {
				JumpEmitterLeft1->Deactivate();
				JumpEmitterRight1->Deactivate();
			}
			if (ThirdJumpFX) {
				JumpEmitterLeft2->Deactivate();
				JumpEmitterRight2->Deactivate();
			}
			// Reset triple jump
			GetCharacterMovement()->JumpZVelocity = 1260.f;
			JumpMaxHoldTime = 0.3f;
			JumpCount = 0;
		} else {
			IsAiming = true;
			if (AimEmitter) {
				if (!AimEmitter->IsActive()) {
					AimEmitter->Activate();
				}
			}
			StartAiming();
		}
	}
}

/**
* Aim for an enemy to throw a grabbed object at.
* Similar to GetGrabbableObjectsInReach, but this time we want a single blocking hit,
* instead of overlapping hits.
* The sweep is otherwise the same and goes much farther.
*/
bool AESPCharacter::ThrowAimTrace(FHitResult& OutHitResult) const {
	float CapsuleHalfHeight = this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector Start = GetActorLocation() + Camera->GetForwardVector() * (TelekinesisConfig.ThrowAimRadius + CapsuleHalfHeight * FMath::Abs(Camera->GetForwardVector().Z));
	FVector End = GetActorLocation() + Camera->GetForwardVector() * (TelekinesisConfig.ThrowAimRange + CapsuleHalfHeight * FMath::Abs(Camera->GetForwardVector().Z));
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TelekinesisConfig.ThrowAimRadius);
	//DrawDebugLine(GetWorld(), GetActorLocation(), End, FColor::Purple, false, 5.f);
	//DrawDebugSphere(GetWorld(), Start, TelekinesisConfig.ThrowAimRadius, 30, FColor::Blue, false, 5.f);
	//DrawDebugSphere(GetWorld(), End, TelekinesisConfig.ThrowAimRadius, 30, FColor::Blue, false, 5.f);
	GetWorld()->SweepSingleByChannel(
		OutHitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel2,
		Sphere
	);
	return OutHitResult.bBlockingHit;
}

// Get object that's closest to the target enemy the character is aiming at
int AESPCharacter::GetClosestGrabbedObject(FHitResult* HitResult) const {
	float Distance = NULL;
	int Index = NULL;

	// Iterate through each physics handle component and check all the objects currently being grabbed
	for (int i = 0; i < PhysicsHandles.Num(); i++) {
		if (UPrimitiveComponent* Component = PhysicsHandles[i]->GetGrabbedComponent()) {
			// Eventually gets us the physics handle component that has the object that's closest to the target enemy the character is aiming at
			if (FVector::Dist(HitResult->GetActor()->GetTargetLocation(), Component->GetComponentLocation()) < Distance || !Distance) {
				Index = i;
				Distance = FVector::Dist(HitResult->GetActor()->GetTargetLocation(), Component->GetComponentLocation());
			}
		}
	}
	return Index;
}

// Get grabbed object that's farthest forward from the Character
int AESPCharacter::GetFarthestGrabbedObject() const {
	float Distance = NULL;
	int Index = NULL;
	// Iterate through each physics handle component and check all the objects currently being grabbed
	for (int i = 0; i < PhysicsHandles.Num(); i++) {
		if (UPrimitiveComponent* Component = PhysicsHandles[i]->GetGrabbedComponent()) {
			// Get the grabbed object's *current* location relative to the character.
			// Because the object may be in a different position from the one recorded in the TArray that we constantly move the object to in Tick
			FVector CurrentPosFromChar = PositionFromChar(Component);
			// Eventually gets us the physics handle component that has the object that's farthest frontwards from the character's aim
			if (CurrentPosFromChar.X - FMath::Abs(CurrentPosFromChar.Y) > Distance || !Distance) {
				Index = i;
				Distance = CurrentPosFromChar.X - FMath::Abs(CurrentPosFromChar.Y);
			}
		}
	}
	return Index;
}

/**
* Throws one of the objects the character is currently manipulating.
* In particular, it either throws the object farthest frontwards from the character's aim, or the object closest to the target enemy.
* The reason we do this is to prevent a grabbed object being thrown towards another grabbed object as much as possible.
*/
void AESPCharacter::Throw() {
	// Check if there's currently at least one object being grabbed and if we're aiming
	// Otherwise, stop AimByHolding
	if (IsGrabbingObject() && IsAiming) {
		int ThrowIndex;
		FHitResult HitResult;
		// If there's a Target, get object closest to the target.
		// If no Target, throw aim trace and get object closest to the HitResult.
		// If no HitResult, throw object farthest from character.
		if (Target.GetActor()) {
			ThrowIndex = GetClosestGrabbedObject(&Target);
		} else if (ThrowAimTrace(HitResult)) {
			ThrowIndex = GetClosestGrabbedObject(&HitResult);
		} else {
			ThrowIndex = GetFarthestGrabbedObject();	
		}

		// If ShooterProjectile, re-enable generated hit events
		UPrimitiveComponent* Component = PhysicsHandles[ThrowIndex]->GetGrabbedComponent();
		if (AShooterProjectile* Projectile = Cast<AShooterProjectile>(Component->GetAttachmentRootActor())) {
			Projectile->GetMesh()->SetNotifyRigidBodyCollision(true);
		}
		Component->WakeAllRigidBodies(); // In case the object is sleeping
		Component->GetOwner()->Tags.Remove("Grabbed");
		// Unlike in release, we only release one object and add an impulse
		PhysicsHandles[ThrowIndex]->ReleaseComponent();
		// remove telekinesis decal
		TelekinesisDecals[ThrowIndex]->DestroyComponent();

		// If there's a Target, throw at Target.
		// If no target and there' was's a blocking hit from ThrowAimTrace, throw at HitResult.
		// Otherwise, throw in the forward direction of the camera.
		if (Target.GetActor()) {
			FVector TargetDirection = Target.GetActor()->GetTargetLocation() - Component->GetComponentLocation();
			Component->AddImpulse(TargetDirection.Rotation().Vector() * TelekinesisConfig.ThrowForce, NAME_None, true);
		} else if (HitResult.bBlockingHit) {
			FVector TargetDirection = HitResult.GetActor()->GetTargetLocation() - Component->GetComponentLocation();
			Component->AddImpulse(TargetDirection.Rotation().Vector() * TelekinesisConfig.ThrowForce, NAME_None, true);
		} else {
			Component->AddImpulse(Camera->GetForwardVector() * TelekinesisConfig.ThrowForce, NAME_None, true);
		}
		if (!IsGrabbingObject()) {
			// Unfreeze character
			IsFrozen = false;
			IsAiming = false;
		}
		if (AimEmitter) {
			AimEmitter->Deactivate();
		}
	} else {
		// Stop AimByHolding
		AimByHolding = false;
	}
}

// Stop freezing and aiming
void AESPCharacter::CancelAim() {
	if (IsFrozen) {
		IsFrozen = false;
		IsAiming = false;
		if (AimEmitter) {
			AimEmitter->Deactivate();
		}
	}
}

// Specific TargetLockOn functionality for ESPCharacter
void AESPCharacter::TargetLockOn() {
	Super::TargetLockOn();
	// And no target and not aiming, cancel aim
	if(!Target.GetActor() && !IsAiming) {
		CancelAim();
	}
}

// Returns true if character is grabbing at least 1 object
bool AESPCharacter::IsGrabbingObject() {
	for (UPhysicsHandleComponent* PhysicsHandle : PhysicsHandles) {
		if (PhysicsHandle->GetGrabbedComponent()) {
			return true;
		}
	}
	return false;
}

/**
* Manage's character's jumping depending on JumpCount and JumpTimer.
* This makes it so the Character's jumping is like the Triple Jump from Super Mario 64.
*
* This method works in tandum with Tick() and OnMovementModeChanged().
* Thanks to OnMovementModeChanged(), each time the player lands, JumpTimer gets set to JumpTime.
* In the Tick() function, JumpTimer gets substracted by delta seconds, and JumpCount gets set to 0
* whenever JumpTimer reachers 0.
*/
void AESPCharacter::Jumping() {
	if (!GetCharacterMovement()->IsFalling()) {
		FVector RelativeVelocity = GetActorRotation().UnrotateVector(GetVelocity());
		/**
		* - After the first jump (JumpCount == 1), if the character jumps before JumpTimer reaches 0,
		* perform the second jump, which allows a higher JumpMaxHoldTime.
		* - After the second jump (JumpCount == 2), if the character jumps before JumpTimer reaches 0 AND
		* if the character has enough velocity, perform the third jump. For the third jump, there's no control for the height,
		* but it allows for a much higher jump.
		* - Second and third jumps activate their respective Jump FX
		* - Otherwise, perform the first jump.
		*/
		if (JumpCount == 1 && JumpTimer > 0.f) {
			if (SecondJumpFX) {
				JumpEmitterLeft1->Activate();
				JumpEmitterRight1->Activate();
			}
			JumpMaxHoldTime = 0.41f;
		} else if (JumpCount == 2 && JumpTimer > 0.f && FMath::Abs(RelativeVelocity.X) + FMath::Abs(RelativeVelocity.Y) >= 600.f) {
			if (ThirdJumpFX) {
				JumpEmitterLeft2->Activate();
				JumpEmitterRight2->Activate();
			}
			GetCharacterMovement()->JumpZVelocity = 4200.f;
			JumpMaxHoldTime = 0.f;
			JumpCount = -1;
		} else {
			GetCharacterMovement()->JumpZVelocity = 1260.f;
			JumpMaxHoldTime = 0.3f;
			JumpCount = 0;
		}
		JumpCount++;
	}
}

/**
* Attaches a decal to an object being grabbed
* 
* @param HitComponent, the component being grabbed
* @param Index, index of TelekinesisDecals which corresponds to the index of PhysicsHandles
*/
void AESPCharacter::AttachTelekinesisDecal(UPrimitiveComponent* HitComponent, int Index) {
	if (TelekinesisDecalMaterial) {
		HitComponent->SetReceivesDecals(true); // Make sure the grabbed object can receive decals
		// Get the placement extent's box of the component
		// Unlike the collision box extent, it doesn't change even after the object is rotated and isn't affected by scale
		FVector ComponentBox = HitComponent->GetPlacementExtent().BoxExtent;

		// Spawn, register and set the material instance for the decal component
		TelekinesisDecals[Index] = NewObject<UDecalComponent>(HitComponent, UDecalComponent::StaticClass(), TEXT("Telekinesis Decal"));
		TelekinesisDecals[Index]->RegisterComponent();
		TelekinesisDecals[Index]->SetDecalMaterial(TelekinesisDecalMaterial);
		// Make the Decal a little bit bigger than the component box
		// Divide the addition by the component's scale since DecalSize gets multiplied by it when attached to the component
		// That way, we're aways adding 1.f to the decal's size no matter the component's scale
		TelekinesisDecals[Index]->DecalSize = ComponentBox + FVector(1.f) / HitComponent->GetComponentScale();
		// Attach decal component to the grabbed component
		TelekinesisDecals[Index]->AttachToComponent(HitComponent, FAttachmentTransformRules::KeepRelativeTransform);
	} else {
		UE_LOG(LogTemp, Error, TEXT("No decal material set!"));
	}
}

// Allows Blueprint implementation of these CPP functions
void AESPCharacter::StartAiming_Implementation() {
}

void AESPCharacter::StopAiming_Implementation() {
	
}
