// Copyright Epic Games, Inc. All Rights Reserved.


#include "RPGWeaponSpawner.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Equipment/RPGPickupDefinition.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryFragment_SetStats.h"
#include "Kismet/GameplayStatics.h"
#include "RPGLogChannels.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "TimerManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPGWeaponSpawner)

class FLifetimeProperty;
class UNiagaraSystem;
class USoundBase;
struct FHitResult;

// Sets default values
ARPGWeaponSpawner::ARPGWeaponSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));
	CollisionVolume->InitCapsuleSize(80.f, 80.f);
	CollisionVolume->OnComponentBeginOverlap.AddDynamic(this, &ARPGWeaponSpawner::OnOverlapBegin);

	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PadMesh"));
	PadMesh->SetupAttachment(RootComponent);

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponMeshRotationSpeed = 40.0f;
	CoolDownTime = 30.0f;
	CheckExistingOverlapDelay = 0.25f;
	bIsWeaponAvailable = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ARPGWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponDefinition && WeaponDefinition->InventoryItemDefinition)
	{
		CoolDownTime = WeaponDefinition->SpawnCoolDownSeconds;	
	}
	else if (const UWorld* World = GetWorld())
	{
		if (!World->IsPlayingReplay())
		{
			UE_LOG(LogLyra, Error, TEXT("'%s' does not have a valid weapon definition! Make sure to set this data on the instance!"), *GetNameSafe(this));	
		}
	}
}

void ARPGWeaponSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CoolDownTimerHandle);
		World->GetTimerManager().ClearTimer(CheckOverlapsDelayTimerHandle);
	}
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ARPGWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update the CoolDownPercentage property to drive respawn time indicators
	UWorld* World = GetWorld();
	if (World->GetTimerManager().IsTimerActive(CoolDownTimerHandle))
	{
		CoolDownPercentage = 1.0f - World->GetTimerManager().GetTimerRemaining(CoolDownTimerHandle)/CoolDownTime;
	}

	WeaponMesh->AddRelativeRotation(FRotator(0.0f, World->GetDeltaSeconds() * WeaponMeshRotationSpeed, 0.0f));
}

void ARPGWeaponSpawner::OnConstruction(const FTransform& Transform)
{
	if (WeaponDefinition != nullptr && WeaponDefinition->DisplayMesh != nullptr)
	{
		WeaponMesh->SetStaticMesh(WeaponDefinition->DisplayMesh);
		WeaponMesh->SetRelativeLocation(WeaponDefinition->WeaponMeshOffset);
		WeaponMesh->SetRelativeScale3D(WeaponDefinition->WeaponMeshScale);
	}	
}

void ARPGWeaponSpawner::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	APawn* OverlappingPawn = Cast<APawn>(OtherActor);
	if (GetLocalRole() == ROLE_Authority && bIsWeaponAvailable && OverlappingPawn != nullptr)
	{
		AttemptPickUpWeapon(OverlappingPawn);
	}
}

void ARPGWeaponSpawner::CheckForExistingOverlaps()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, APawn::StaticClass());

	for (AActor* OverlappingActor : OverlappingActors)
	{
		AttemptPickUpWeapon(Cast<APawn>(OverlappingActor));
	}
}

void ARPGWeaponSpawner::AttemptPickUpWeapon_Implementation(APawn* Pawn)
{
	if (GetLocalRole() == ROLE_Authority && bIsWeaponAvailable && UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn))
	{
		TSubclassOf<URPGInventoryItemDefinition> WeaponItemDefinition = WeaponDefinition ? WeaponDefinition->InventoryItemDefinition : nullptr;
		if (WeaponItemDefinition != nullptr)
		{
			//Attempt to grant the weapon
			if (GiveWeapon(WeaponItemDefinition, Pawn))
			{
				//Weapon picked up by pawn
				bIsWeaponAvailable = false;
				SetWeaponPickupVisibility(false);
				PlayPickupEffects();
				StartCoolDown();
			}
		}		
	}
}

void ARPGWeaponSpawner::StartCoolDown()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(CoolDownTimerHandle, this, &ARPGWeaponSpawner::OnCoolDownTimerComplete, CoolDownTime);
	}
}

void ARPGWeaponSpawner::ResetCoolDown()
{
	UWorld* World = GetWorld();

	if (World)
	{
		World->GetTimerManager().ClearTimer(CoolDownTimerHandle);
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		bIsWeaponAvailable = true;
		PlayRespawnEffects();
		SetWeaponPickupVisibility(true);

		if (World)
		{
			World->GetTimerManager().SetTimer(CheckOverlapsDelayTimerHandle, this, &ARPGWeaponSpawner::CheckForExistingOverlaps, CheckExistingOverlapDelay);
		}
	}

	CoolDownPercentage = 0.0f;
}

void ARPGWeaponSpawner::OnCoolDownTimerComplete()
{
	ResetCoolDown();
}

void ARPGWeaponSpawner::SetWeaponPickupVisibility(bool bShouldBeVisible)
{
	WeaponMesh->SetVisibility(bShouldBeVisible, true);
}

void ARPGWeaponSpawner::PlayPickupEffects_Implementation()
{
	if (WeaponDefinition != nullptr)
	{
		USoundBase* PickupSound = WeaponDefinition->PickedUpSound;
		if (PickupSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
		}

		UNiagaraSystem* PickupEffect = WeaponDefinition->PickedUpEffect;
		if (PickupEffect != nullptr)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, WeaponMesh->GetComponentLocation());
		}
	}
}

void ARPGWeaponSpawner::PlayRespawnEffects_Implementation()
{
	if (WeaponDefinition != nullptr)
	{
		USoundBase* RespawnSound = WeaponDefinition->RespawnedSound;
		if (RespawnSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, RespawnSound, GetActorLocation());
		}

		UNiagaraSystem* RespawnEffect = WeaponDefinition->RespawnedEffect;
		if (RespawnEffect != nullptr)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, RespawnEffect, WeaponMesh->GetComponentLocation());
		}
	}
}

void ARPGWeaponSpawner::OnRep_WeaponAvailability()
{
	if (bIsWeaponAvailable)
	{
		PlayRespawnEffects();
		SetWeaponPickupVisibility(true);
	}
	else
	{
		SetWeaponPickupVisibility(false);
		StartCoolDown();
		PlayPickupEffects();
	}	
}

void ARPGWeaponSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPGWeaponSpawner, bIsWeaponAvailable);
}

int32 ARPGWeaponSpawner::GetDefaultStatFromItemDef(const TSubclassOf<URPGInventoryItemDefinition> WeaponItemClass, FGameplayTag StatTag)
{
	if (WeaponItemClass != nullptr)
	{
		if (URPGInventoryItemDefinition* WeaponItemCDO = WeaponItemClass->GetDefaultObject<URPGInventoryItemDefinition>())
		{
			if (const UInventoryFragment_SetStats* ItemStatsFragment = Cast<UInventoryFragment_SetStats>( WeaponItemCDO->FindFragmentByClass(UInventoryFragment_SetStats::StaticClass()) ))
			{
				return ItemStatsFragment->GetItemStatByTag(StatTag);
			}
		}
	}

	return 0;
}
