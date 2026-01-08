// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "MyExperienceManagerComponent.generated.h"

class UMyExperienceDefinition;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMyExperienceLoaded, const UMyExperienceDefinition*);

UCLASS(BlueprintType, Blueprintable)
class MYCPPPROJECT_API UMyExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public:
	UMyExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	//~End of UActorComponent interface

	// 현재 Experience가 로드되었는지 확인
	bool IsExperienceLoaded() const;

	// 현재 Experience 데이터 가져오기 (로드되지 않았으면 nullptr 반환)
	const UMyExperienceDefinition* GetCurrentExperienceChecked() const;

	// (Server Only) GameMode에서 Experience를 설정할 때 호출
	void ServerSetCurrentExperience(const UMyExperienceDefinition* Experience);

	// Experience 로딩 완료(변경) 시 호출되는 델리게이트
	FOnMyExperienceLoaded OnExperienceLoaded;

private:
	// 리플리케이션되는 현재 Experience 데이터
	UPROPERTY(ReplicatedUsing=OnRep_CurrentExperience)
	TObjectPtr<const UMyExperienceDefinition> CurrentExperience;

	UFUNCTION()
	void OnRep_CurrentExperience();
};
