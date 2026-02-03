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
#include "MyGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "UI/MyUserWidget.h"
#include "GameFramework/HUD.h"

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
		// 생성 후 'RootLayoutInstance' 변수에 저장
		RootLayoutInstance = CreateWidget<UPrimaryGameLayout>(this, PrimaryLayoutClass);
		if (RootLayoutInstance)
		{
			RootLayoutInstance->AddToPlayerScreen(0);

			// HUD Push 로직
			if (HUDWidgetClass)
			{
				RootLayoutInstance->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(
					MyGameplayTags::UI_Layer_Game,
					false,
					HUDWidgetClass
				);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UI Debug] Setup Failed. Condition Check: Local=%d, ClassValid=%d"), IsLocalController(), (PrimaryLayoutClass != nullptr));
	}
}

void AMyCPPProjectPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
	// 로컬 컨트롤러이고 레이아웃이 준비되었다면
	if (IsLocalController() && RootLayoutInstance)
	{
		// RootLayoutInstance를 우리 클래스로 캐스팅
		if (UMyPrimaryGameLayout* MyRoot = Cast<UMyPrimaryGameLayout>(RootLayoutInstance))
		{
			// HUD 찾기
			if (UCommonActivatableWidget* HUD = MyRoot->GetGameLayerWidget())
			{
				// ASC 연결 (변수명 충돌 방지를 위해 'TargetHUD' 사용)
				if (UMyUserWidget* TargetHUD = Cast<UMyUserWidget>(HUD))
				{
					if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(aPawn))
					{
						TargetHUD->SetAbilitySystemComponent(ASC);
						UE_LOG(LogTemp, Warning, TEXT("[UI] OnPossess: HUD Connected Successfully!"));
					}
				}
			}
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
