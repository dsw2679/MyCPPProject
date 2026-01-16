// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyCPPProjectGameState.h"
#include "Advanced/MyExperienceManagerComponent.h"

AMyCPPProjectGameState::AMyCPPProjectGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UMyExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}
