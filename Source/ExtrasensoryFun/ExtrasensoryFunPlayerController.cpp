// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtrasensoryFunPlayerController.h"
#include "Blueprint/UserWidget.h"

void AExtrasensoryFunPlayerController::BeginPlay() {
	Super::BeginPlay();
	HUD = CreateWidget(this, HUDClass);
	if (HUD) {
		HUD->AddToViewport();
	}
}
