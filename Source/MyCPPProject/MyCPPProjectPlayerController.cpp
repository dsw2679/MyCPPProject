// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCPPProjectPlayerController.h"

#include "CommonActivatableWidget.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "MyCPPProjectCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/LocalPlayer.h"
#include "MyCPPProject.h"
#include "PrimaryGameLayout.h"
#include "Blueprint/UserWidget.h"
#include "Advanced/MyPrimaryGameLayout.h"
#include "AbilitySystemComponent.h"

AMyCPPProjectPlayerController::AMyCPPProjectPlayerController()
{
	bIsTouch = false;
	bMoveToMouseCursor = false;

	// create the path following comp
	PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(TEXT("Path Following Component"));
	PathFollowingComponent->Initialize();

	// configure the controller
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}

void AMyCPPProjectPlayerController::MoveToLocation(const FVector& Dest)
{
	StopMovement();
	if (FollowTime <= ShortPressThreshold)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Dest);
	}
	FollowTime = 0.f;
}

void AMyCPPProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
	
}

void AMyCPPProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController() && PrimaryLayoutClass)
	{
		// 레이아웃 생성 및 뷰포트 추가
		UPrimaryGameLayout* RootLayout = CreateWidget<UPrimaryGameLayout>(this, PrimaryLayoutClass);
		RootLayout->AddToPlayerScreen(0);

		// HUD를 Game 레이어에 Push
		if (HUDWidgetClass)
		{
			// 태그로 스택 찾아서 Push
			RootLayout->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(
				FGameplayTag::RequestGameplayTag("UI.Layer.Game"),
				false,
				HUDWidgetClass
			);
		}
	}
}

void AMyCPPProjectPlayerController::OnInputStarted()
{
	StopMovement();

	// Update the move destination to wherever the cursor is pointing at
	UpdateCachedDestination();
}

void AMyCPPProjectPlayerController::OnSetDestinationTriggered()
{
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(this);
	if (ASI)
	{
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
		if (ASC && ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Locked"))))
		{
			return; // 스턴 상태면 입력 무시
		}
	}
	
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// Update the move destination to wherever the cursor is pointing at
	UpdateCachedDestination();
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AMyCPPProjectPlayerController::OnSetDestinationReleased()
{
	
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void AMyCPPProjectPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void AMyCPPProjectPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AMyCPPProjectPlayerController::UpdateCachedDestination()
{
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
}
