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
#include "Component/MyCinematicComponent.h"
#include "UI/MyUserWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/SViewport.h"

AMyCPPProjectPlayerController::AMyCPPProjectPlayerController()
{
	bIsTouch = false;
	bMoveToMouseCursor = false;

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	
	// create the path following comp
	PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(TEXT("Path Following Component"));
	PathFollowingComponent->Initialize();

	// configure the controller
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;

	// 데미지 텍스트 컴포넌트 생성
	DamageTextManagerComponent = CreateDefaultSubobject<UMyDamageTextManagerComponent>(TEXT("DamageTextManager"));
	
	// 시퀀서 재생 컴포넌트
	CinematicComponent = CreateDefaultSubobject<UMyCinematicComponent>(TEXT("CinematicComponent"));
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
	InitializeUI();
	
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
	// 게임 속도 늦추기 (타격감 및 연출)
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);

	// 1.5초(실제 시간 0.15초) 후 속도 복구 및 결과창 출력
	FTimerHandle ResultTimer;
	GetWorldTimerManager().SetTimer(ResultTimer, [this]() {
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		if (!ResultWidgetClass.IsNull())
		{
			PushWidgetToModalLayer(ResultWidgetClass);
		}
	}, 1.0f, false);
}

void AMyCPPProjectPlayerController::ShowLoadingScreen() 
{
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	// NextTick이 실행될 때 이미 로딩이 끝났다면 로딩창을 띄우지 않음!
	if (GI)
	{
		GI->StartLoadingScreen();
	}
}

void AMyCPPProjectPlayerController::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();
	InitializeUI();
}

void AMyCPPProjectPlayerController::InitializeUI()
{
	if (!IsLocalController() || !PrimaryLayoutClass) return;

	// 기존 레이아웃 정리 (심리스 트래블 시 이전 레벨의 위젯 잔해 제거)
	if (RootLayoutInstance)
	{
		RootLayoutInstance->RemoveFromParent();
		RootLayoutInstance = nullptr;
	}

	// 루트 레이아웃 생성 및 화면 부착
	RootLayoutInstance = CreateWidget<UPrimaryGameLayout>(this, PrimaryLayoutClass);
	if (RootLayoutInstance)
	{
		RootLayoutInstance->AddToPlayerScreen(0);

		FString MapName = GetWorld()->GetMapName();
		bool bIsTransitionMap = MapName.Contains(TEXT("L_Transition"));
		bool bIsBattleMap = MapName.Contains(TEXT("L_Battle")); // 전투 맵 체크

		// 로딩 상태 체크 (GI의 영구 UI 방식 호출)
		UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
		if (GI && GI->bIsLoadingInProgress)
		{
			// GI 기반 로딩창은 월드와 독립적이므로, 여기서 한 번 더 호출해도
			// GI 내부에서 "이미 떠있으면 무시"하는 로직이 있다면 안전합니다.
			ShowLoadingScreen();
		}
		// // 전투 맵이나 전환 맵이 아닐 때만 HUD를 즉시 생성
		// if (!bIsTransitionMap && !bIsBattleMap && HUDWidgetClass)
		// {
		// 	CreateHUD();
		// }
	}
	
}

void AMyCPPProjectPlayerController::OnExperienceLoadCompleted() 
{
	if (GetWorld()->GetMapName().Contains(TEXT("L_Shop")))
	{
		FinishLoading(); // 로딩창 끄기
		if (CinematicComponent)
		{
			CinematicComponent->PlayIntroSequence(); // 컴포넌트에 위임
		}
	}
	
	FString MapName = GetWorld()->GetMapName();
	// 목적지(L_Battle)에 도착했는지 확인
	if (MapName.Contains(TEXT("L_Dwarf_City")))
	{
		FinishLoading(); // 로딩창 끄기
		if (CinematicComponent)
		{
			CinematicComponent->PlayIntroSequence(); // 컴포넌트에 위임
		}
	}

	// 만약 L_Transition 맵이라면 로딩창을 유지하고 아무것도 하지 않음
	if (MapName.Contains(TEXT("L_Transition")))
	{
		return;
	}

	// 그 외의 경우(상점 등) 로딩창 즉시 종료
	FinishLoading();
	
}

void AMyCPPProjectPlayerController::PrepareLevelTransition(FString LevelName)
{
	PendingLevelPath = LevelName; // 이동할 곳 예약
	
	//ShowLoadingScreen();
	
	// 로딩 상태임을 게임 인스턴스에 기록 (안전장치)
	if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
	{
		GI->StartLoadingScreen();
	}
	
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

void AMyCPPProjectPlayerController::PlayBattleIntroCutscene()
{
	APawn* TargetPawn = GetPawn();
	if (!TargetPawn) return;

	// 입력 및 조작 차단
	SetIgnoreMoveInput(true);
	bShowMouseCursor = false;

	// 카메라 페이드 중단 (검은 화면 해제)
	if (PlayerCameraManager) PlayerCameraManager->StopCameraFade();
	
	// 시작 카메라 위치 설정 (플레이어로부터 멀리 떨어진 공중)
	FVector PawnLoc = TargetPawn->GetActorLocation();
	FVector StartLoc = PawnLoc + FVector(-3000.f, -3000.f, 3000.f);
	FRotator StartRot = (PawnLoc - StartLoc).Rotation();

	// 임시로 컨트롤러의 위치와 회전을 멀리 보낸 뒤 블렌딩 시작
	SetControlRotation(StartRot);
	
	// 임시 카메라 액터 생성 또는 계산된 위치에서 블렌딩 시작
	// 여기서는 간단히 PlayerCameraManager를 통해 시점을 강제 고정하거나
	// 전용 CameraActor를 활용하는 방식이 좋으나, C++에서는 다음과 같이 처리합니다.
	
	// 로딩창을 0.5초 후 페이드 아웃하며 풍경 노출 시작
	FTimerHandle IntroTimer;
	GetWorldTimerManager().SetTimer(IntroTimer, [this, TargetPawn]() {
		if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance()))
		{
			GI->StopLoadingScreen();
		}

		// 카메라 블렌딩 시작 (멀리서 캐릭터에게 3초 동안 다가감)
		// 주의: 이전에 멀리 있는 Actor를 ViewTarget으로 잡아야 함 (생략 시 현재 위치에서 시작)
		SetViewTargetWithBlend(TargetPawn, 3.0f, EViewTargetBlendFunction::VTBlend_Cubic);

		// 연출 종료 후 조작 복구
		FTimerHandle EndTimer;
		GetWorldTimerManager().SetTimer(EndTimer, [this]() {
			SetIgnoreMoveInput(false);
			bShowMouseCursor = true;
			FinishLoading();
		}, 3.0f, false);

	}, 0.5f, false);
}

void AMyCPPProjectPlayerController::FinishLoading()
{
	// 로딩창이 이미 없거나 로딩 중이 아니라면 무시
	UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance());
	if (!GI || !GI->bIsLoadingInProgress) return;

	// 텍스처 로딩을 위해 2초간 더 대기 후 로딩창 제거
	float MinimumHoldTime = 2.0f;

	FTimerHandle FinishTimer;
	GetWorldTimerManager().SetTimer(FinishTimer, [this, GI]() {
		GI->StopLoadingScreen();

		// 조작 권한 복구
		GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::SetInputFocusToGameViewport);

		UE_LOG(LogTemp, Log, TEXT("[Loading] Loading screen removed after minimum hold time."));
	}, MinimumHoldTime, false);
	
	GetWorldTimerManager().SetTimerForNextTick(this, &ThisClass::SetInputFocusToGameViewport);
}

void AMyCPPProjectPlayerController::CreateHUD()
{
	// HUD 생성 (L_Transition 맵이 아닐 때만 생성하도록 제한)
	if (RootLayoutInstance && HUDWidgetClass)
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