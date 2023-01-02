// by Jason Hilani


#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "HealthComponent.h"
#include "Components/CapsuleComponent.h"

// Default constructor
ABaseCharacter::ABaseCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the spring, attach it to the character's capsule
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	// Create the camera and attach it to the spring arm
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Create health component
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

// Handle character death
void ABaseCharacter::HandleDeath() {
	DetachFromControllerPendingDestroy();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

// Called to bind functionality to player input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Character movement player input binds
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ABaseCharacter::MoveRight);
	// Camera rotation player input binds for mouse
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	// Camera rotation player input binds for gamepad
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &ABaseCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &ABaseCharacter::LookRightRate);
}

/** 
* Move character forwards or backwards based on AxisValue from player input
*
* @param AxisValue, Movement input to apply
*/
void ABaseCharacter::MoveForward(float AxisValue) {
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

/**
* Strafe character left/right
* 
* @param AxisValue, Movement input to apply
*/
void ABaseCharacter::MoveRight(float AxisValue) {
	AddMovementInput(GetActorRightVector() * AxisValue);
}

/**
* Rotate camera up/down at RotationRate per second.
* For gamepads only.
* 
* @param AxisValue, Rotation input to apply
*/
void ABaseCharacter::LookUpRate(float AxisValue) {
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

/**
* Rotate camera left/right at RotationRate per second.
* For gamepads only.
* 
* @param AxisValue, Rotation input to apply
*/
void ABaseCharacter::LookRightRate(float AxisValue) {
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}