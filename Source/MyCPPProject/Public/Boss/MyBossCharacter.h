// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "MyBossCharacter.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UMyPawnData;

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API AMyBossCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMyBossCharacter();

	// IAbilitySystemInterface 구현
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void PossessedBy(AController* NewController) override;
	
	// overlap
	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	// Hit 이벤트 처리 함수
	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 충돌 처리 로직 분리
	void ProcessCollision(AActor* OtherActor, FVector DebugLoc);
	
	// 돌진 중 이미 충돌한 액터 목록 (중복 피격 방지용)
	UPROPERTY()
	TSet<AActor*> SpeedburstHitActors;

	// 태그 변경 감지 함수 (돌진 시작될 때 목록 초기화하려고 씀)
	virtual void OnDashTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	// 이 보스가 사용할 데이터 에셋
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	TObjectPtr<const UMyPawnData> PawnData;

	// 강제 로딩을 수행할 함수
	void PreloadAssets();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> InitStatEffect;
	
	// 메시지 리스너 핸들
	FGameplayMessageListenerHandle BossInfoRequestListenerHandle;

	// 요청이 오면 실행될 함수
	void OnBossInfoRequested(FGameplayTag Channel, const struct FMyBossMessageStruct& Payload);
};


