// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCPPProjectPlayerController.h"

#include "CommonActivatableWidget.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/LocalPlayer.h"
#include "PrimaryGameLayout.h"
#include "Blueprint/UserWidget.h"
#include "Advanced/MyPrimaryGameLayout.h"
#include "MyGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Advanced/ControllerComponent/MyDamageTextManagerComponent.h"
#include "UI/MyUserWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SViewport.h"

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
	
	// 컴포넌트 생성 (헤더에 전방 선언 class UMyDamageTextManagerComponent; 필요)
	DamageTextManagerComponent = CreateDefaultSubobject<UMyDamageTextManagerComponent>(TEXT("DamageTextManager"));
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
					HUDWidgetClass,
					[this](EAsyncWidgetLayerState State, UCommonUserWidget* Screen)
					{
						// 위젯 로딩 및 푸시가 완료된 시점(AfterPush)에 실행
						if (State == EAsyncWidgetLayerState::AfterPush)
						{
							if (UMyUserWidget* TargetHUD = Cast<UMyUserWidget>(Screen))
							{
								// 내 폰(캐릭터)에서 ASC를 찾아 HUD에 연결
								if (UAbilitySystemComponent* ASC =
									UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
								{
									TargetHUD->SetAbilitySystemComponent(ASC);
								}
								
								if (GetWorld())
								{
									GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::SetInputFocusToGameViewport);
								}
							}
						}
					}
				);
			}
		}
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

void AMyCPPProjectPlayerController::ToggleInventory()
{
	// 권한이 없으면 실행하지 않음
	if (!bCanOpenInventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UI] Inventory access denied in this area."));
		return;
	}
	
	if (!RootLayoutInstance || InventoryWidgetClass.IsNull()) return;
	
	// 위젯이 레이어에 푸시된 후 실행될 콜백 정의
	auto OnWidgetLoaded = [this](EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen)
	{
		if (State == EAsyncWidgetLayerState::AfterPush && Screen)
		{
			// [수정] OnDeactivated는 인자가 없는 void() 형태이므로 람다 인자를 ()로 비웁니다.
			Screen->OnDeactivated().AddWeakLambda(this, [this]()
			{
				if (UWorld* World = GetWorld())
				{
					// 다음 틱에 포커스 복구 로직 실행
					World->GetTimerManager().SetTimerForNextTick(this, &ThisClass::SetInputFocusToGameViewport);
				}
			});
		}
	};

	RootLayoutInstance->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(
		MyGameplayTags::UI_Layer_Menu,
		true,
		InventoryWidgetClass,
		OnWidgetLoaded // 콜백 전달
	);
}

void AMyCPPProjectPlayerController::SetInputFocusToGameViewport()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UGameViewportClient* ViewportClient = LocalPlayer->ViewportClient)
		{
			ViewportClient->SetMouseLockMode(EMouseLockMode::DoNotLock);
			ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::NoCapture);

			// TSharedPtr<SViewport>를 명시적으로 받고
			TSharedPtr<SViewport> ViewportWidget = ViewportClient->GetGameViewportWidget();

			// 유효성 검사 후
			if (ViewportWidget.IsValid())
			{
				FSlateApplication::Get().SetAllUserFocus(ViewportWidget.ToSharedRef(), EFocusCause::SetDirectly);
				FSlateApplication::Get().ReleaseAllPointerCapture();
			}
		}
	}

	// 입력 모드 갱신
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AMyCPPProjectPlayerController::SetHUDVisibility(bool bVisible)
{
	if (UMyPrimaryGameLayout* MyRoot = Cast<UMyPrimaryGameLayout>(RootLayoutInstance))
	{
		if (UCommonActivatableWidget* GameHUD = MyRoot->GetGameLayerWidget())
		{
			GameHUD->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
		}
	}
}
