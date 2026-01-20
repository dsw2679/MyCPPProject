// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffect.h"
#include "MyAreaEffectActor.generated.h"

class USphereComponent;
class UNiagaraComponent;

UCLASS()
class MYCPPPROJECT_API AMyAreaEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyAreaEffectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 타이머
	UFUNCTION()
	void OnTimerTick();

public:	
	// 범위 설정용 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AreaEffect")
	TObjectPtr<USphereComponent> OverlapSphere;

	// 시각효과용 컴포넌트 (BP에서 넣어도 되지만 C++ 선언 추천)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AreaEffect")
	TObjectPtr<UNiagaraComponent> NiagaraFX;

	// 적용할 GameplayEffect 클래스 (에디터에서 설정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AreaEffect")
	TSubclassOf<UGameplayEffect> EffectClass;

	// 틱 간격 (초)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaEffect", meta = (ExposeOnSpawn = "true"))
	float TickInterval = 0.5f;

	// 액터 지속 시간 (초) - 0이면 무한
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaEffect" ,meta = (ExposeOnSpawn = "true"))
	float LifeTime = 3.0f;
	
	// 틱당 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaEffect", meta = (ExposeOnSpawn = "true"))
	float DamagePerTick = 10.0f;
	
	// 총 몇 번 터질 것인가?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AreaEffect", meta = (ExposeOnSpawn = "true"))
	int32 MaxTickCount = 3;

	// 현재 터진 횟수 (내부 카운트용)
	int32 CurrentTickCount = 0;

};
