// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/MyMyGameplayAbility_Grenade.h"

// void UMyMyGameplayAbility_Grenade::ActivateItemEffect()
// {
// 	APlayerController* PC = Cast<APlayerController>(GetOwningActorFromActorInfo());
// 	if (!PC) return;
//
// 	FHitResult Hit;
// 	if (PC->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
// 	{
// 		FVector TargetLocation = Hit.Location;
// 		FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation() + FVector(0, 0, 100);
//
// 		// 투사체 스폰 (ProjectileClass는 에디터에서 설정)
// 		// if (ProjectileClass)
// 		// {
// 		// 	FActorSpawnParameters SpawnParams;
// 		// 	SpawnParams.Owner = GetAvatarActorFromActorInfo();
// 		//
// 		// 	// 목표 지점을 향해 발사 로직 처리 (SpawnActor)
// 		// 	// AMyProjectile* Projectile = GetWorld()->SpawnActor<AMyProjectile>(ProjectileClass, SpawnLocation, Rotation, SpawnParams);
// 		// }
// 	}
// }
