// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"


/**
 * 프레젝트의 게임플레이 태그를 관리하는 싱글톤
 */

namespace MyGameplayTags
{
	// 외부에서 접근할 수 있도록 태그를 선언만 합니다.
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Boss_Spawned);
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Boss_HealthChanged);
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Request_BossInfo);
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Message_Boss_Dead);
}
