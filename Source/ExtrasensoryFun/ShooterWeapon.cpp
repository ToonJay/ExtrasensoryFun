// by Jason Hilani


#include "ShooterWeapon.h"
#include "ShooterProjectile.h"

// Default constructor
AShooterWeapon::AShooterWeapon() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Create weapon mesh, attach to root, and set it to overlap with the camera
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMesh->SetupAttachment(Root);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Overlap);
}

// Spawn/shoot projectile
void AShooterWeapon::FireWeapon() {
	if (AController* OwnerController = GetOwnerController()) {
		// Get shot direction from player's viewpoint's rotation
		FVector Location;
		FRotator ShotDirection;
		OwnerController->GetPlayerViewPoint(Location, ShotDirection);
		// Use weapon's Projectile Socket as the spawn location
		FVector ProjectileSpawnPoint = WeaponMesh->GetSocketLocation("ProjectileSocket");

		// Spawn projectile and set properties
		AShooterProjectile* Projectile = GetWorld()->SpawnActor<AShooterProjectile>(ShooterProjectileClass, ProjectileSpawnPoint, ShotDirection);
		Projectile->SetOwner(this);
		Projectile->SetActorRelativeRotation(ShotDirection + FRotator(-90.f, 0.f, -90.f)); // We add that FRotator because of the character's and weapon's rotation
	}
}

