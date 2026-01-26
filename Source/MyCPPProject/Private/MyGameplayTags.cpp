// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameplayTags.h"


namespace MyGameplayTags
{
	// 여기서 실제 문자열과 매칭하여 정의합니다.
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_Spawned, "Message.Boss.Spawned");
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_HealthChanged, "Message.Boss.HealthChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_Request_BossInfo, "Message.Request.BossInfo");
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_Dead, "Message.Boss.Dead");
}
