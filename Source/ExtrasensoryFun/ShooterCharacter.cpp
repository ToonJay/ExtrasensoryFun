// by Jason Hilani

#include "ShooterCharacter.h"
#include "ShooterWeapon.h"

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay() {
	Super::BeginPlay();

	// Spawn weapon and attach to the ShooterCharacter's right hand
	if (ShooterWeaponClass) {
		ShooterWeapon = GetWorld()->SpawnActor<AShooterWeapon>(ShooterWeaponClass);
		GetMesh()->HideBoneByName(TEXT("b_RightWeapon"), EPhysBodyOp::PBO_None);
		ShooterWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponPoint"));
		ShooterWeapon->SetOwner(this);
	}

}

// Called to bind functionality to player input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Shoot player input binds
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AShooterCharacter::Shoot);
}

// Use ShooterWeapon's FireWeapon function
void AShooterCharacter::Shoot() {
	if (ShooterWeapon) {
		ShooterWeapon->FireWeapon();
	}
}


