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
public:
	// Game over method
	void GameOver();

	// Getter method
	UFUNCTION(BlueprintCallable)
	float GetRestartDelay() { return RestartDelay; }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// Hud class and instance
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> HUDClass;
	UPROPERTY()
	UUserWidget* HUD;

	// Game over class
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> GameOverScreenClass;
	UPROPERTY(EditAnywhere)
	float RestartDelay = 5;
	FTimerHandle RestartTimer;
};
