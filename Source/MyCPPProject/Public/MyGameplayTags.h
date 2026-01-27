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
	
	// 피격 관련 태그 추가
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Hit_Stun);
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Hit_Knockdown);
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Hit_Knockback);
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Effect_Hit_Light);
	
	// 보스 패턴 태그 선언
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Basic_Cast);				// 일반: 장판 생성
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Basic_SwingCombo);			// 일반: 3연타
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Basic_Speedburst);			// 일반: 돌진
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Basic_Clap);				// 일반: 박수
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Basic_Launchpad);			// 일반: 보스주변장판
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Special_Pull);				// 특수: 강제 끌어오기
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Special_Subjugation);		// 특수: 플레이어 다운 시
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Special_Pointing);			// 특수(70%): 광역 장판
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Special_Siphon);			// 특수(40%): 저주
	MYCPPPROJECT_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Boss_Pattern_Special_Ultimate);			// 특수(10%): 필살기
}
