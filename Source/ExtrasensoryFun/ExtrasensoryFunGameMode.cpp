// Copyright Epic Games, Inc. All Rights Reserved.


#include "ExtrasensoryFunGameMode.h"
#include "BaseCharacter.h"
#include "ExtrasensoryFunPlayerController.h"

void AExtrasensoryFunGameMode::ActorDied(AActor* DeadActor) {
	if (ABaseCharacter* DeadCharacter = Cast<ABaseCharacter>(DeadActor)) {
		if (AExtrasensoryFunPlayerController* PlayerController = Cast<AExtrasensoryFunPlayerController>(DeadCharacter->GetController())) {
			PlayerController->GameOver();
		}
		DeadCharacter->HandleDeath();
	} else {
		DeadActor->Destroy();
	}
}