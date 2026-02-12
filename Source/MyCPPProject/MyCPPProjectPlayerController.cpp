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
#include "Advanced/MyGameInstance.h"
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

void AMyCPPProjectPlayerController::TravelToLevel(FString LevelName)
{
	if (UWorld* World = GetWorld())
	{
		// Seamless Travel 실행
		World->ServerTravel(LevelName, false);
	}
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

			FString MapName = GetWorld()->GetMapName();
			bool bIsTransitionMap = MapName.Contains(TEXT("L_Transition"));
			
			// 레이아웃이 성공적으로 생성된 직후에 로딩창 체크 및 호출
			UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
			if (bIsTransitionMap || (GI && GI->bIsLoadingInProgress))
			{
				ShowLoadingScreen();
			}
			
			// HUD Push 로직
			if (!bIsTransitionMap && HUDWidgetClass)
			{
				RootLayoutInstance->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(
					MyGameplayTags::UI_Layer_Game,
					false,
					HUDWidgetClass,
					[this](EAsyncWidgetLayerState State, UCommonUserWidget* Screen)
					{
						if (State == EAsyncWidgetLayerState::AfterPush)
						{
							if (UMyUserWidget* TargetHUD = Cast<UMyUserWidget>(Screen))
							{
								if (UAbilitySystemComponent* ASC =
									UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPawn()))
								{
									TargetHUD->SetAbilitySystemComponent(ASC);
								}

								if (GetWorld())
								{
									GetWorld()->GetTimerManager().SetTimerForNextTick(
										this, &ThisClass::SetInputFocusToGameViewport);
								}
							}
						}
					}
				);
			}
		}
	}
	
	// 보스 사망 메시지 리스너 등록
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.RegisterListener(MyGameplayTags::Message_Boss_Dead, this, &AMyCPPProjectPlayerController::OnBossDeadReceived);
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
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator,
		                                               FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
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
			// OnDeactivated는 인자가 없는 void() 형태이므로 람다 인자를 ()로 비웁니다.
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

void AMyCPPProjectPlayerController::PushWidgetToModalLayer(TSoftClassPtr<UCommonActivatableWidget> WidgetClass)
{
	if (!RootLayoutInstance || WidgetClass.IsNull()) return;

	RootLayoutInstance->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(
		MyGameplayTags::UI_Layer_Modal,
		true, // Suspend Input (마우스 활성화 및 조작 차단)
		WidgetClass,
		[this](EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen)
		{
			if (State == EAsyncWidgetLayerState::AfterPush && Screen)
			{
				// 위젯이 닫힐 때 포커스 복구 콜백 등록
				Screen->OnDeactivated().AddWeakLambda(this, [this]() {
					this->SetInputFocusToGameViewport();
				});
			}
		}
	);
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

// 메시지 수신 시 호출될 콜백 함수
void AMyCPPProjectPlayerController::OnBossDeadReceived(FGameplayTag Channel, const FMyBossMessageStruct& Payload)
{
	// 보스가 죽었으므로 결과창 UI 푸시
	// 이전에 만든 PushWidgetToModalLayer 함수를 활용합니다.
	if (!ResultWidgetClass.IsNull())
	{
		PushWidgetToModalLayer(ResultWidgetClass);
	}
}

void AMyCPPProjectPlayerController::ShowLoadingScreen() 
{
	if (!RootLayoutInstance || LoadingWidgetClass.IsNull()) return;
	if (LoadingWidgetInstance) return;
	
	// 비동기가 아닌 동기식으로 클래스를 즉시 로드합니다.
	UClass* LoadedClass = LoadingWidgetClass.LoadSynchronous();
	if (LoadedClass)
	{
		LoadingWidgetInstance = RootLayoutInstance->PushWidgetToLayerStack<UCommonActivatableWidget>(
			MyGameplayTags::UI_Layer_Modal,
			LoadedClass
		);
		UE_LOG(LogTemp, Log, TEXT("[Loading] Loading Screen Visible."));
	}
	
	
	// // 비동기 푸시 시 콜백을 통해 인스턴스를 저장합니다.
	// RootLayoutInstance->PushWidgetToLayerStackAsync<UCommonActivatableWidget>(
	// 	MyGameplayTags::UI_Layer_Modal,
	// 	true,
	// 	LoadingWidgetClass,
	// 	[this](EAsyncWidgetLayerState State, UCommonActivatableWidget* Screen)
	// 	{
	// 		if (State == EAsyncWidgetLayerState::AfterPush)
	// 		{
	// 			LoadingWidgetInstance = Cast<UCommonActivatableWidget>(Screen); // 인스턴스 저장
	// 			UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	// 			if (GI && !GI->bIsLoadingInProgress)
	// 			{
	// 				OnExperienceLoadCompleted(); // 다시 호출하여 닫기 수행
	// 			}
	// 		}
	// 	}
	// );
}

void AMyCPPProjectPlayerController::OnExperienceLoadCompleted() 
{
	FString MapName = GetWorld()->GetMapName();
	if (MapName.Contains(TEXT("L_Transition")))
	{
		UE_LOG(LogTemp, Log, TEXT("[Loading] Still in Transition Map. Keep Loading Screen."));
		return;
	}
	// 로딩 플래그 초기화
	if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
	{
		GI->bIsLoadingInProgress = false;
	}
	
	// 저장해둔 로딩 위젯 인스턴스를 비활성화합니다.
	if (LoadingWidgetInstance)
	{
		LoadingWidgetInstance->DeactivateWidget();
		LoadingWidgetInstance = nullptr; // 참조 해제
		UE_LOG(LogTemp, Error, TEXT("[Loading] Final Destination Reached. Closing Loading Screen."));
	}
	else
	{
		// 위젯이 아직 생성 전이라면, 다음 프레임에 다시 시도하거나 로그를 남깁니다.
		UE_LOG(LogTemp, Warning, TEXT("[Loading] Experience ready but LoadingWidgetInstance not yet created."));
	}
	// 입력 포커스 강제 복구
	// DeactivateWidget이 완료된 직후(NextTick)에 호출하는 것이 가장 안전합니다.
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::SetInputFocusToGameViewport);
}

void AMyCPPProjectPlayerController::PrepareLevelTransition(FString LevelName)
{
	PendingLevelPath = LevelName; // 이동할 곳 예약

	// 카메라 페이드 아웃 (0.5초 동안 검은색으로)
	if (PlayerCameraManager) {
		PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 0.5f, FLinearColor::Black, false, true);
	}

	// 페이드가 진행되는 동안 로딩창을 띄웁니다.
	// 0.5초 페이드 후 이동하도록 타이머 설정
	FTimerHandle TravelTimerHandle;
	GetWorldTimerManager().SetTimer(TravelTimerHandle, [this]() {
		if (UWorld* World = GetWorld()) {
			// 이제 로딩창이 완벽히 준비된 상태에서 이동합니다.
			World->ServerTravel(PendingLevelPath, false);
		}
	}, 0.5f, false);
}