// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "MyHeroComponent.generated.h"

class UMyPawnData;
class UMyExperienceDefinition;
struct FGameplayTag;
struct FInputActionValue;

UCLASS(BlueprintType, Meta = (BlueprintSpawnableComponent))
class MYCPPPROJECT_API UMyHeroComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UMyHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 캐릭터가 컨트롤러에 의해 빙의되었을 때 초기화 수행
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	// GAS 시스템 초기화 및 기본 스킬 부여
	void InitializeAbilitySystem();

	// Experience 로드 완료 시 호출
	void OnExperienceLoaded(const UMyExperienceDefinition* Experience);

protected:
	// 입력 핸들러 (나중에 캐릭터 클래스에서 호출하거나 여기서 직접 처리)
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	// 마우스 우클릭 이동 핸들러
	void Input_MoveToCursor(const FInputActionValue& InputActionValue);
	void Input_Move_Triggered(const FInputActionValue& InputActionValue);
	void Input_Move_Released(const FInputActionValue& InputActionValue);

	// 이동 관련 변수
	FVector CachedDestination;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.3f;

private:
	// 현재 사용 중인 PawnData (Experience에서 받아옴)
	UPROPERTY(VisibleInstanceOnly, Category = "Lyra|Hero")
	TObjectPtr<const UMyPawnData> PawnData;

	// 입력 바인딩 핸들들을 저장
	TArray<uint32> AbilityBindHandles;
	
	// 초기화가 이미 되었는지 확인하는 플래그
	bool bHasInitialized = false;
	
	// 입력 초기화 플래그
	bool bInputInitialized = false;
};
