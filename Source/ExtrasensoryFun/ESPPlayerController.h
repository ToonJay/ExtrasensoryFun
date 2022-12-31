// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ESPPlayerController.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class EXTRASENSORYFUN_API AESPPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDClass;

	UPROPERTY()
	UUserWidget* HUD;
};
