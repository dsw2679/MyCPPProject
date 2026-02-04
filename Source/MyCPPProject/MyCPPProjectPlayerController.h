// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerController.h"
#include "Message/MyBossMessageStruct.h"
#include "MyCPPProjectPlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class UPathFollowingComponent;
class UMyDamageTextManagerComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Player controller for a top-down perspective game.
 *  Implements point and click based controls
 */
UCLASS(abstract)
class AMyCPPProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Component used for moving along a NavMesh path. */
	UPROPERTY(VisibleDefaultsOnly, Category = AI)
	TObjectPtr<UPathFollowingComponent> PathFollowingComponent;

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, Category="Input")
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UNiagaraSystem> FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> SetDestinationTouchAction;

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	/** Set to true if we're using touch input */
	uint32 bIsTouch : 1;

	/** Saved location of the character movement destination */
	FVector CachedDestination;

	/** Time that the click input has been pressed */
	float FollowTime = 0.0f;

	// 전체 UI 레이어 구조를 담당하는 클래스 (WBP_MyPrimaryLayout)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UPrimaryGameLayout> PrimaryLayoutClass;

	// Game 레이어에 띄울 메인 HUD 클래스 (WBP_MyHUDLayout)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftClassPtr<class UCommonActivatableWidget> HUDWidgetClass; // PushAsync를 위해 Soft Class 추천

	// 생성된 루트 레이아웃 인스턴스 보관용
	UPROPERTY()
	TObjectPtr<class UPrimaryGameLayout> RootLayoutInstance;
	
	// 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UMyDamageTextManagerComponent> DamageTextManagerComponent;

public:

	/** Constructor */
	AMyCPPProjectPlayerController();
	
	UFUNCTION(BlueprintCallable, Category = "Lyra|Character")
	void MoveToLocation(const FVector& Dest);

protected:

	/** Initialize input bindings */
	virtual void SetupInputComponent() override;
	
	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* aPawn) override;
	
	/** Input handlers */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

	/** Helper function to get the move destination */
	void UpdateCachedDestination();
	
};


