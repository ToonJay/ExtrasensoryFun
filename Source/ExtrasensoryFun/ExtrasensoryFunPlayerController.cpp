// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtrasensoryFunPlayerController.h"
#include "Blueprint/UserWidget.h"

void AExtrasensoryFunPlayerController::GameOver() {
	HUD->RemoveFromParent();
	if (UUserWidget* GameOverScreen = CreateWidget(this, GameOverScreenClass)) {
		GameOverScreen->AddToViewport();
	}
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}

void AExtrasensoryFunPlayerController::BeginPlay() {
	Super::BeginPlay();

	HUD = CreateWidget(this, HUDClass);
	if (HUD) {
		HUD->AddToViewport();
	}
}