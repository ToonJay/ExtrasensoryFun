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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Getter methods
	UStaticMeshComponent* GetMesh() { return ProjectileMesh; }
	UProjectileMovementComponent* GetMovementComponent() { return ProjectileMovementComponent; }
	float GetDamage() const { return Damage; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	// Projectile components and FX
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* TrailParticles;
	UPROPERTY(EditAnywhere)
	UParticleSystem* ExplosionFX;

	// Projectile properties
	UPROPERTY(EditAnywhere, Category = "Combat")
	float Damage = 100.f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ExplosionRadius = 300.f;

	// OnHit event
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
