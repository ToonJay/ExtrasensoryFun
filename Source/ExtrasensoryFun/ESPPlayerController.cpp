// Fill out your copyright notice in the Description page of Project Settings.


#include "ESPPlayerController.h"
#include "Blueprint/UserWidget.h"

void AESPPlayerController::BeginPlay() {
	Super::BeginPlay();
	HUD = CreateWidget(this, HUDClass);
	if (HUD) {
		HUD->AddToViewport();
	}
}
