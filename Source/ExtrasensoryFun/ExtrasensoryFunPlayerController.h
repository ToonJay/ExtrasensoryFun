// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ExtrasensoryFunPlayerController.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class EXTRASENSORYFUN_API AExtrasensoryFunPlayerController : public APlayerController
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
