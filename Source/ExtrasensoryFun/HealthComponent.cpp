// by Jason Hilani


#include "HealthComponent.h"
#include "ExtrasensoryFunGameMode.h"
#include <Kismet/GameplayStatics.h>

// Default constructor
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UHealthComponent::BeginPlay() {
	Super::BeginPlay();
	// Set health to max health
	Health = MaxHealth;
	// Add function to delegate
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::DamageTaken);
	// Get game mode
	ExtrasensoryFunGameMode = Cast<AExtrasensoryFunGameMode>(UGameplayStatics::GetGameMode(this));

}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Take damage
void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser) {
	// Check if there's damage and if the actor can die
	if (Damage <= 0.f || !CanDie) return;
	UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), Damage);
	// Apply damage
	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Health Remaining: %f"), Health);
	// Check for death
	if (IsDead()) {
		ExtrasensoryFunGameMode->ActorDied(DamagedActor);
	}
}