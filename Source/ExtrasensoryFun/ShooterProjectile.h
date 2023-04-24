// by Jason Hilani

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShooterProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class EXTRASENSORYFUN_API AShooterProjectile : public AActor {
	GENERATED_BODY()
	
public:	
	// Default constructor
	AShooterProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Getter methods
	UStaticMeshComponent* GetMesh() { return ProjectileMesh; }
	UProjectileMovementComponent* GetMovementComp() { return MovementComp; }
	UParticleSystemComponent* GetTrailFX() { return TrailFX; }
	float GetDamage() const { return Damage; }
	
private:
	//-----Projectile properties and components-----
	// Components
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovementComp;
	// Particles
	UPROPERTY(VisibleAnywhere, Category = "Particles")
	UParticleSystemComponent* TrailFX;
	UPROPERTY(VisibleAnywhere, Category = "Particles")
	UParticleSystem* ExplosionFX;
	// Combat properties
	UPROPERTY(EditAnywhere, Category = "Combat")
	float Damage = 100.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ExplosionRadius = 300.f;
	// Sounds
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* LaunchSound;
	UPROPERTY(EditAnywhere, Category = "Combat")
	USoundBase* HitSound;

	// OnHit event
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
