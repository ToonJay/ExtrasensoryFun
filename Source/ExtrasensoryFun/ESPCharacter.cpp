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

// Default constructor
AESPCharacter::AESPCharacter() {
	// Create physics handle components for the character according to ObjectGrabLimit and number them
	for (int i = 0; i < ObjectGrabLimit; i++) {
		CreateDefaultSubobject<UPhysicsHandleComponent>(FName("Physics Handle " + FString::FromInt(i + 1)));
	}
}

// Called to bind functionality to player input
void AESPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Telekinesis player input binds
	PlayerInputComponent->BindAction(TEXT("Grab"), EInputEvent::IE_Pressed, this, &AESPCharacter::Grab);
	PlayerInputComponent->BindAction(TEXT("Release"), EInputEvent::IE_Pressed, this, &AESPCharacter::Release);
	PlayerInputComponent->BindAction(TEXT("Throw"), EInputEvent::IE_Pressed, this, &AESPCharacter::Throw);
}

// Called every frame
void AESPCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	/**
	* Set the location and rotation of each grabbed object.
	* TargetLocation is always set to the relative position from the character assigned during grabbing.
	* Use the camera viewpoint for the direction of TargetLocation.
	* Rotation is set to the TargetRotation of the spring arm.
	*/
	for (int i = 0; i < PhysicsHandles.Num(); i++) {
		if (UPrimitiveComponent* Component = PhysicsHandles[i]->GetGrabbedComponent()) {
			FVector TargetLocation = GetActorLocation()
				+ Camera->GetForwardVector() * PositionsFromCharacter[i].X
				+ Camera->GetRightVector() * PositionsFromCharacter[i].Y;
			PhysicsHandles[i]->SetTargetLocationAndRotation(TargetLocation, SpringArm->GetTargetRotation());
		}
	}
	// Activate the emitter when grabbing at least 1 object, deactivate when not
	if (CastEmitter) {
		if (IsGrabbingObject()) {
			if (!CastEmitter->IsActive()) {
				CastEmitter->Activate();
			}
		} else {
			CastEmitter->Deactivate();
		}
	} else {
		UE_LOG(LogTemp, Error, TEXT("No particle effect on MuzzleCast!"));
	}
}

// Called when the game starts or when spawned
void AESPCharacter::BeginPlay() {
	Super::BeginPlay();
	// Assign all physics handle components to the PhysicsHandles TArray
	GetComponents(PhysicsHandles);
	// For each physics handle, set its interpolation speed and add an element to the PositionsFromCharacter and TelekinesisDecals TArrays
	for (UPhysicsHandleComponent* PhysicsHandle : PhysicsHandles) {
		PhysicsHandle->SetInterpolationSpeed(InterpolationSpeed);
		PositionsFromCharacter.Add(FVector(0.f));
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
	* the sphere starts from the top of our head if we aim upwards, or from the bottom of our feet if we aim downwards,
	* and anything in-between.
	*/
	float CapsuleHalfHeight = this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector Start = GetActorLocation() + Camera->GetForwardVector() * (GrabRadius + CapsuleHalfHeight * FMath::Abs(Camera->GetForwardVector().Z));
	// End at the GrabRange + the CapsuleHalfHeight in proportion to how far we aim the camera up or down.
	FVector End = GetActorLocation() + Camera->GetForwardVector() * (GrabRange + CapsuleHalfHeight * FMath::Abs(Camera->GetForwardVector().Z));
	FCollisionShape Sphere = FCollisionShape::MakeSphere(GrabRadius);
	// These parameters will make the sphere sweep inclube overlaps.
	FCollisionQueryParams Params = FCollisionQueryParams();
	Params.bFindInitialOverlaps = true;
	DrawDebugSphere(GetWorld(), End, GrabRadius, 20, FColor::Red, true); // For a visual on the sweep
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
* This is so then the character always grabs from the nearest to farthest objects.
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

/**
* Grab objects within GrabRange and GrabRadius.
* The character can grab as many objects as they have physics handle components.
* The character will grab the closest objects first.
*/
void AESPCharacter::Grab() {
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
					HitComponent->WakeAllRigidBodies();
					// Detach it from any attached actor such as a trigger or an actor composed of many actors
					HitActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
					HitActor->Tags.Add("Grabbed"); // Useful for tracking the objects that are currently being grabbed
					PhysicsHandles[y]->GrabComponentAtLocationWithRotation(
						HitComponent,
						NAME_None,
						HitResult.ImpactPoint,
						Camera->GetComponentRotation()
					);
					AttachTelekinesisDecal(HitComponent, y);
					// Make the character ignore the collision of the object so the character doesn't get pushed around by the objects it's manipulating
					this->MoveIgnoreActorAdd(HitActor);
					// Make the grabbed objects overlap with the camera channel so the spring arm doesn't retract for grabbed objects that end up behind the character
					HitComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Overlap);
					// Record object positions relative to the character upon grabbing them
					PositionsFromCharacter[i] = HitComponent->GetComponentTransform().GetRelativeTransform(GetActorTransform()).GetLocation();
					break;
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
			PhysicsHandles[i]->ReleaseComponent();
			TelekinesisDecals[i]->DestroyComponent();
		}
	}
}

/**
* Throws one of the objects the character is currently manipulating.
* In particular, throws the object farthest frontwards from the character's aim.
* The reason we do this is to prevent a grabbed object being thrown towards another grabbed object as much as possible.
*/
void AESPCharacter::Throw() {
	float farthestDistance = -10000; // A negative number since grabbed objects can end up behind the player
	float farthestIndex = 0;
	// Check if there's currently at least one object being grabbed
	if (IsGrabbingObject()) {
		UPrimitiveComponent* FarthestGrabbedComponent = nullptr;
		// Iterate through each physics handle component and check all the objects currently being grabbed
		for (int i = 0; i < PhysicsHandles.Num(); i++) {
			if (UPrimitiveComponent * GrabbedComponent = PhysicsHandles[i]->GetGrabbedComponent()) {
				// Get the grabbed object's *current* location relative to the character.
				// Because the object may be in a different position from the one recorded in the TArray that we constantly move the object to in Tick
				FVector PositionFromCharacter = GrabbedComponent->GetComponentTransform().GetRelativeTransform(GetActorTransform()).GetLocation();
				// Eventually gets us the physics handle component that has the object that's farthest frontwards from the character's aim
				if (PositionFromCharacter.X - FMath::Abs(PositionFromCharacter.Y) > farthestDistance) {
					FarthestGrabbedComponent = GrabbedComponent;
					farthestIndex = i;
					farthestDistance = PositionFromCharacter.X - FMath::Abs(PositionFromCharacter.Y);
				}
			}
		}
		FarthestGrabbedComponent->WakeAllRigidBodies(); // In case the object is sleeping
		FarthestGrabbedComponent->GetOwner()->Tags.Remove("Grabbed");
		// Unlike in release, we only release one object and add an impulse
		PhysicsHandles[farthestIndex]->ReleaseComponent();
		FarthestGrabbedComponent->AddImpulse(Camera->GetForwardVector() * ThrowForce, NAME_None, true);
		TelekinesisDecals[farthestIndex]->DestroyComponent();
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
		TelekinesisDecals[Index]->DecalSize = ComponentBox + (FVector(1.f) / HitComponent->GetComponentScale());
		// Decal's center is at the bottom of the object, so it needs to move by the size of the component's box's z axis
		TelekinesisDecals[Index]->AddRelativeLocation(FVector(0.f, 0.f, ComponentBox.Z));
		// Attach decal component to the grabbed component
		TelekinesisDecals[Index]->AttachToComponent(HitComponent, FAttachmentTransformRules::KeepRelativeTransform);
	} else {
		UE_LOG(LogTemp, Error, TEXT("No decal material set!"));
	}
}