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
#include "Materials/Material.h"
#include "Engine/World.h"


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
	CameraBoom->TargetArmLength = 800.f;
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
