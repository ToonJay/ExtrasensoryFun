// by Jason Hilani


#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>

// Default constructor
ABaseCharacter::ABaseCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the spring, attach it to the character's capsule, and set properties
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bEnableCameraRotationLag = true;

	// Create the camera and attach it to the spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	
	// Create health component
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	// Set to block the TelekinesisAttack trace channel
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	// Orient rotation to movement and set Yaw Rotation Rate
	if (Cast<APlayerController>(GetController())) {
		GetCharacterMovement()->bOrientRotationToMovement = true;
	} else {
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}
	GetCharacterMovement()->RotationRate.Yaw = 540.f;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();

}

// Get component's relative position from the Character
FVector ABaseCharacter::PositionFromChar(UPrimitiveComponent* Component) const {
	return Component->GetComponentTransform().GetRelativeTransform(GetActorTransform()).GetLocation();
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	/**
	* If there's a Target, rotate character's yaw towards the Target and set SpringArm's relative location to 
	* the middle of the distance between the Target and character (+ 90.f on the Z axis).
	* Otherwise, reset Target and set the SpringArm's relative location to FVector(0.f, 0.f, 90.f)
	*/
	if (Target.GetActor() && Cast<ABaseCharacter>(Target.GetActor())->GetController() && Cast<APlayerController>(GetController())) {
		FVector TargetDirection = Target.GetActor()->GetActorLocation() - GetActorLocation();
		SetActorRotation(FRotator(GetActorRotation().Pitch, TargetDirection.Rotation().Yaw, GetActorRotation().Roll));
		SpringArm->SetRelativeLocation(PositionFromChar(Target.GetComponent()) / 2 + FVector(0.f, 0.f, 90.f));
	} else {
		Target.Init();
		SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 90.f));
	}
}

// Called to bind functionality to player input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Character movement player input binds
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABaseCharacter::MoveRight);
	// Camera rotation player input binds for mouse
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	// Camera rotation player input binds for gamepad
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &ABaseCharacter::LookRightRate);
	// Camera input binds
	PlayerInputComponent->BindAction(TEXT("CenterCamera"), EInputEvent::IE_Pressed, this, &ABaseCharacter::CenterCameraBehindCharacter);
	PlayerInputComponent->BindAction(TEXT("TargetLock-on"), EInputEvent::IE_Pressed, this, &ABaseCharacter::TargetLockOn);
	PlayerInputComponent->BindAxis(TEXT("ZoomCamera"), this, &ABaseCharacter::ZoomCamera);
	PlayerInputComponent->BindAxis(TEXT("ZoomCameraRate"), this, &ABaseCharacter::ZoomCameraRate);
}

// Handle character death
void ABaseCharacter::HandleDeath() {
	// Detach controller
	DetachFromControllerPendingDestroy();
	// Remove collisions
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Set collision response to TelekinesisAttack tracing channel to ignore
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Ignore);
}

/** 
* Move character forwards or backwards based on AxisValue from player input
*
* @param AxisValue, Movement input to apply
*/
void ABaseCharacter::MoveForward(float AxisValue) {
	// Get the control rotation's yaw and zero-out the pitch and roll
	FRotator const ControlYawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
	// Use forward vector of ControlYawRotation and axis value to move character forwards or backwards
	AddMovementInput(UKismetMathLibrary::GetForwardVector(ControlYawRotation), AxisValue);
}

/**
* Move character left or right based on AxisValue from player input.
* 
* @param AxisValue, Movement input to apply
*/
void ABaseCharacter::MoveRight(float AxisValue) {
	// Get the control rotation's yaw and zero-out the pitch and roll
	FRotator const ControlYawRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
	// Use right vector of ControlYawRotation and axis value to move character left or right
	AddMovementInput(UKismetMathLibrary::GetRightVector(ControlYawRotation), AxisValue);
}

/**
* Rotate camera left/right at RotationRate per second.
* For gamepads only.
* 
* @param AxisValue, Rotation input to apply
*/
void ABaseCharacter::LookRightRate(float AxisValue) {
	// calculate delta for this frame from the rate information
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

/**
* Change Spring-Arm's pitch rotation and target arm length at the same time.
* Zoom-in when rotating upwards, Zoom-out when rotating downwards.
*/
void ABaseCharacter::ZoomCamera(float AxisValue) {
	FRotator ControllerRotation = GetController()->GetControlRotation();
	// Keep ControllerRotation's Vector's Z axis between -0.6f to 0.6f
	if ((AxisValue > 0 && ControllerRotation.Vector().Z > -0.6f) || (AxisValue < 0 && ControllerRotation.Vector().Z < 0.6f)) {
		AddControllerPitchInput(AxisValue);
	}
	// Prevent ControllerRotation's Vector's Z axis from going under -0.6f or above 0.6f for Pitch Rotation
	FVector ControllerVector = ControllerRotation.Vector();
	if (GetController()->GetControlRotation().Vector().Z < -0.6f) {
		GetController()->SetControlRotation(FRotator(FVector(ControllerVector.X, ControllerVector.Y, -0.6f).Rotation().Pitch, ControllerRotation.Yaw, ControllerRotation.Roll));
	} else if (GetController()->GetControlRotation().Vector().Z > 0.6f) {
		GetController()->SetControlRotation(FRotator(FVector(ControllerVector.X, ControllerVector.Y, 0.6f).Rotation().Pitch, ControllerRotation.Yaw, ControllerRotation.Roll));
	}
	// Scale SpringArm's target arm length to ControllerRotation's Vector's Z axis
	SpringArm->TargetArmLength = 1200.f * (1 - GetController()->GetControlRotation().Vector().Z);
}

/**
* Change Spring-Arm's pitch rotation and target arm length at the same time.
* Zoom-in when rotating upwards, Zoom-out when rotating downwards.
* Same as ZoomCamera, but using RotationRate and GetDeltaSeconds().
* For gamepads only.
*/
void ABaseCharacter::ZoomCameraRate(float AxisValue) {
	FRotator ControllerRotation = GetController()->GetControlRotation();
	// Keep ControllerRotation's Vector's Z axis between -0.6f to 0.6f
	if ((AxisValue > 0 && ControllerRotation.Vector().Z > -0.6f) || (AxisValue < 0 && ControllerRotation.Vector().Z < 0.6f)) {
		AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
	}
	// Prevent ControllerRotation's Vector's Z axis from going under -0.6f or above 0.6f for Pitch Rotation
	FVector ControllerVector = ControllerRotation.Vector();
	if (GetController()->GetControlRotation().Vector().Z < -0.6f) {
		GetController()->SetControlRotation(FRotator(FVector(ControllerVector.X, ControllerVector.Y, -0.6f).Rotation().Pitch, ControllerRotation.Yaw, ControllerRotation.Roll));
	} else if (GetController()->GetControlRotation().Vector().Z > 0.6f) {
		GetController()->SetControlRotation(FRotator(FVector(ControllerVector.X, ControllerVector.Y, 0.6f).Rotation().Pitch, ControllerRotation.Yaw, ControllerRotation.Roll));
	}
	// Scale SpringArm's target arm length to ControllerRotation's Vector's Z axis
	SpringArm->TargetArmLength = 1200.f * (1 - GetController()->GetControlRotation().Vector().Z);
}

// Centers the camera behind the character
void ABaseCharacter::CenterCameraBehindCharacter() {
	GetController()->SetControlRotation(GetActorRotation());
}

// Lock camera on a target that the character is facing
void ABaseCharacter::TargetLockOn() {
	// Start by centering the camera behind the character
	GetController()->SetControlRotation(GetActorRotation());
	// If no Target locked on, sphere sweep for one
	// Otherwise, reset target and set camera back to normal
	if (!Target.GetActor()) {
		float CapsuleHalfHeight = this->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		// + the CapsuleHalfHeight in proportion to how far we aim the camera up or down.
		FVector Start = GetActorLocation() + GetActorForwardVector() * (800.f + CapsuleHalfHeight);
		FVector End = GetActorLocation() + GetActorForwardVector() * (10000.f + CapsuleHalfHeight);
		FCollisionShape Sphere = FCollisionShape::MakeSphere(800.f);
		// Sweep with sphere in the TelekinesisAttack channel
		GetWorld()->SweepSingleByChannel(
			Target,
			Start, End,
			FQuat::Identity,
			ECC_GameTraceChannel2,
			Sphere
		);
		// If there's a Target, use controller rotation yaw and don't orient rotation to movement
		// Otherwise, do the opposite
		if (Target.GetActor()) {
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		} else {
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	} else {
		// Reset Target
		Target.Init();
		// Set camera back to normal
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}