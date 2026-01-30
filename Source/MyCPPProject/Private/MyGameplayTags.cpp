// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameplayTags.h"


namespace MyGameplayTags
{
	// 여기서 실제 문자열과 매칭하여 정의합니다.
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_Spawned, "Message.Boss.Spawned");
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_HealthChanged, "Message.Boss.HealthChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_Request_BossInfo, "Message.Request.BossInfo");
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_Dead, "Message.Boss.Dead");
	
	UE_DEFINE_GAMEPLAY_TAG(Effect_Hit_Stun, "Effect.Hit.Stun");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Hit_Knockdown, "Effect.Hit.Knockdown");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Hit_Knockback, "Effect.Hit.Knockback");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Hit_Light, "Effect.Hit.Light");
	
	// 보스 패턴 태그 정의
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Basic_Cast, "Boss.Pattern.Basic.Cast");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Basic_SwingCombo, "Boss.Pattern.Basic.SwingCombo");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Basic_Speedburst, "Boss.Pattern.Basic.Speedburst");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Basic_Clap, "Boss.Pattern.Basic.Clap");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Basic_Launchpad, "Boss.Pattern.Basic.Launchpad");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Special_Pull, "Boss.Pattern.Special.Pull");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Special_Subjugation, "Boss.Pattern.Special.Subjugation");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Special_Pointing, "Boss.Pattern.Special.Pointing");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Special_Siphon, "Boss.Pattern.Special.Siphon");
	UE_DEFINE_GAMEPLAY_TAG(Boss_Pattern_Special_Ultimate, "Boss.Pattern.Special.UltimateSwing");
	
	// 상태 태그
	UE_DEFINE_GAMEPLAY_TAG(State_Boss_Dashing, "State.Boss.Dashing");
	UE_DEFINE_GAMEPLAY_TAG(Event_Boss_Hit_Speedburst, "Event.Boss.Hit.Speedburst");
	
	UE_DEFINE_GAMEPLAY_TAG(State_Boss_Staggered, "State.Boss.Staggered");
	UE_DEFINE_GAMEPLAY_TAG(Event_Boss_Staggered, "Event.Boss.Staggered");
	UE_DEFINE_GAMEPLAY_TAG(State_Boss_Immune_Stagger, "State.Boss.Immune.Stagger");
}
