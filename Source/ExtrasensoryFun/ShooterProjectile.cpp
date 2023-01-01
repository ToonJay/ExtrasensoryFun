// by Jason Hilani


#include "ShooterProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterWeapon.h"
#include "Particles/ParticleSystemComponent.h"

// Default constructor
AShooterProjectile::AShooterProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create projectile mesh, make it the root, and set it to overlap with the camera
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;
	ProjectileMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECR_Overlap);
	
	//Create projectile movement component and set properties
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	ProjectileMovementComponent->MaxSpeed = 2000.f;
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	// Create trail particles
	TrailParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Smoke Trail"));
	TrailParticles->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void AShooterProjectile::BeginPlay()
{
	Super::BeginPlay();
	// Add function to delegate
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AShooterProjectile::OnHit);

}

// Called every frame
void AShooterProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// On hit, apply damage event and destroy the projectile
void AShooterProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	// Get owner, destroy if there's no owner
	if (AShooterWeapon* MyOwner = Cast<AShooterWeapon>(GetOwner())) {
		// Get instigator from owner
		AController* MyOwnerInstigator = MyOwner->GetOwnerController();
		UClass* DamageTypeClass = UDamageType::StaticClass();

		// Apply damage and play FX if OtherActor exists and isn't the projectile itself, its owner or its owner's instigator
		if (OtherActor && OtherActor != this && OtherActor != MyOwner && OtherActor != MyOwnerInstigator) {

			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, ExplosionRadius, 20, FColor::Red, false, 5.f);
			// If there's an explosion radius, apply radial damage, otherwise apply regular damage
			if (ExplosionRadius > 0) {
				UGameplayStatics::ApplyRadialDamage(this, Damage, Hit.ImpactPoint, ExplosionRadius, DamageTypeClass, TArray<AActor*>(), this, MyOwnerInstigator);
			} else {
				UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeClass);
			}

			if (ExplosionFX) {
				UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionFX, Hit.ImpactPoint, GetActorRotation());
			}
		}

		Destroy();
	} else {
		Destroy();
	}
}