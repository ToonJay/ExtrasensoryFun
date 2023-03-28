// by Jason Hilani


#include "ExtrasensoryFunPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "ESPCharacter.h"

// Called every frame
void AExtrasensoryFunPlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (Aiming) {
		if (AESPCharacter* PlayerChar = Cast<AESPCharacter>(GetPawn())) {
			// Add aiming UI when aiming, remove it when not
			if (PlayerChar->GetIsFrozen() && !PlayerChar->GetTarget().GetActor()) {
				if (!Aiming->IsInViewport()) {
					Aiming->AddToViewport();
				}
			} else {
				if (Aiming->IsInViewport()) {
					Aiming->RemoveFromParent();
				}
			}
		}
	}
}

// When the player dies
void AExtrasensoryFunPlayerController::GameOver() {
	HUD->RemoveFromParent();
	if (UUserWidget* GameOverScreen = CreateWidget(this, GameOverScreenClass)) {
		GameOverScreen->AddToViewport();
	}
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}

// Called when the game starts or when spawned
void AExtrasensoryFunPlayerController::BeginPlay() {
	Super::BeginPlay();

	// Create and add the player's HUD
	HUD = CreateWidget(this, HUDClass);
	if (HUD) {
		HUD->AddToViewport();
	}
	// Create aiming UI widget
	Aiming = CreateWidget(this, AimingClass);
}