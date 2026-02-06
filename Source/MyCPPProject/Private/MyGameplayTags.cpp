// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameplayTags.h"


namespace MyGameplayTags
{
	// UI 레이어 태그 정의
	UE_DEFINE_GAMEPLAY_TAG(UI_Layer_Game, "UI.Layer.Game");
	UE_DEFINE_GAMEPLAY_TAG(UI_Layer_Menu, "UI.Layer.Menu");
	UE_DEFINE_GAMEPLAY_TAG(UI_Layer_Modal, "UI.Layer.Modal");
	
	// 여기서 실제 문자열과 매칭하여 정의합니다.
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_Spawned, "Message.Boss.Spawned");
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_HealthChanged, "Message.Boss.HealthChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_Request_BossInfo, "Message.Request.BossInfo");
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_Dead, "Message.Boss.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_StaggerChanged, "Message.Boss.StaggerChanged");
	UE_DEFINE_GAMEPLAY_TAG(Message_Boss_InitInfo, "Message.Boss.InitInfo");
	UE_DEFINE_GAMEPLAY_TAG(Message_Damage, "Message.Damage");
	
	UE_DEFINE_GAMEPLAY_TAG(Data_Boss, "Data.Boss");

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
	
	// 플레이어 스킬 및 쿨타임 관련
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_Q, "Cooldown.Skill.Q");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_W, "Cooldown.Skill.W");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_E, "Cooldown.Skill.E");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_R, "Cooldown.Skill.R");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_A, "Cooldown.Skill.A");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_S, "Cooldown.Skill.S");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_D, "Cooldown.Skill.D");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_F, "Cooldown.Skill.F");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Skill_Space, "Cooldown.Skill.Space");
	
	// 아이템 쿨타임 정의
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Item_1, "Cooldown.Item.1");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Item_2, "Cooldown.Item.2");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Item_3, "Cooldown.Item.3");
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Item_4, "Cooldown.Item.4");
	
	// 인벤토리 메시지 정의
	UE_DEFINE_GAMEPLAY_TAG(Message_Inventory_Updated, "Message.Inventory.Updated");
	
	// 인풋 태그
	UE_DEFINE_GAMEPLAY_TAG(InputTag_UI_Inventory, "InputTag.UI.Inventory");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Item_1, "InputTag.Item.1");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Item_2, "InputTag.Item.2");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Item_3, "InputTag.Item.3");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Item_4, "InputTag.Item.4");
}
