// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCPPProjectCharacter.h"

#include "Advanced/MyHeroComponent.h"
#include "AbilitySystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Advanced/MyCPPProjectPlayerState.h"
#include "MotionWarpingComponent.h"
#include "NiagaraSystem.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Experience/MyPawnData.h"


void AMyCPPProjectCharacter::PreloadAssets()
{
	if (!PawnData) return;

	UE_LOG(LogTemp, Warning, TEXT("[Hero] Starting Preload for %s"), *GetName());

	// 1. VFX(Niagara) 프리로드
	for (auto VFX : PawnData->PreloadVFX)
	{
		if (VFX) {
			// 실제 사용은 안 하지만 포인터를 참조함으로써 메모리에 로드 상태 유지
			UE_LOG(LogTemp, Log, TEXT("[Hero] Preloaded VFX: %s"), *VFX->GetName());
		}
	}

	// 2. SFX(Sound) 프리로드
	for (auto SFX : PawnData->PreloadSFX)
	{
		if (SFX) {
			UE_LOG(LogTemp, Log, TEXT("[Hero] Preloaded SFX: %s"), *SFX->GetName());
		}
	}

	// 3. GameplayCues 프리로드 (클래스 로딩)
	for (auto CueClass : PawnData->PreloadGameplayCues)
	{
		if (CueClass) {
			// 클래스 디폴트 객체(CDO)를 참조하여 로드 보장
			CueClass->GetDefaultObject();
			UE_LOG(LogTemp, Log, TEXT("[Hero] Preloaded Cue: %s"), *CueClass->GetName());
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Hero] Preload Complete!"));
}

AMyCPPProjectCharacter::AMyCPPProjectCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create the camera boom component
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));

	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	// Create the camera component
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));

	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	// HeroComponent 생성
	HeroComponent = CreateDefaultSubobject<UMyHeroComponent>(TEXT("HeroComponent"));
	
	// Create Motion Warping Component
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

UAbilitySystemComponent* AMyCPPProjectCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void AMyCPPProjectCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AMyCPPProjectPlayerState* PS = GetPlayerState<AMyCPPProjectPlayerState>();
	if (PS)
	{
		// 캐릭터가 가진 약포인터 변수에 실제 ASC를 할당
		AbilitySystemComponent = PS->GetAbilitySystemComponent();

		// GAS에게 Owner(PS)와 Avatar(Character)를 알림
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		
		if (HeroComponent)
		{
			HeroComponent->InitializeAbilitySystem();
		}
	}
}

void AMyCPPProjectCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// PlayerState에서 ASC를 가져와 연결합니다. (클라이언트)
	AMyCPPProjectPlayerState* PS = GetPlayerState<AMyCPPProjectPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();
		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		if (HeroComponent)
		{
			HeroComponent->InitializeAbilitySystem();
		}
	}
	
}

void AMyCPPProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	PreloadAssets();
	// stub
}

void AMyCPPProjectCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	// stub
}

void AMyCPPProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 입력 초기화
	if (HeroComponent)
	{
		HeroComponent->InitializePlayerInput(PlayerInputComponent);
	}
	
}
