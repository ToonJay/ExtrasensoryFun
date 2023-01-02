// Copyright Epic Games, Inc. All Rights Reserved.


#include "ExtrasensoryFunGameMode.h"
#include "BaseCharacter.h"

void AExtrasensoryFunGameMode::ActorDied(AActor* DeadActor) {
	if (ABaseCharacter* DeadCharacter = Cast<ABaseCharacter>(DeadActor)) {
		DeadCharacter->HandleDeath();
	} else {
		DeadActor->Destroy();
	}
}
