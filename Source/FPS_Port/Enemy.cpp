// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DrawDebugHelpers.h"
// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DamageCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Damage Collision"));
	DamageCollision->SetupAttachment(RootComponent);

	AIPerComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Comp"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->SightRadius = 1250.0f;
	SightConfig->LoseSightRadius = 1280.0f;
	SightConfig->PeripheralVisionAngleDegrees = 90.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	//how long it retains 0 means never and 0.1 means after 0.1 seconds (Specifies age limit after stimuli generated by this sense become forgotten. 0 means "never")
	SightConfig->SetMaxAge(0.1f);

	AIPerComp->ConfigureSense(*SightConfig);
	AIPerComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerComp->OnPerceptionUpdated.AddDynamic(this, &AEnemy::OnSensed);

	MovementSpeed = 375.0f;
	DamageCollision->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	DamageCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnHit);
	DamageCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_r"));
	DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseLocal = this->GetActorLocation();
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnHit(UPrimitiveComponent* hitCom, AActor* other, UPrimitiveComponent* otherCom, int32 bodyIndex, bool bFromSweep, const FHitResult& hit)
{
	if(APlayerCharacter* Char = Cast<APlayerCharacter>(other))
	{
		if (doOnce) 
		{
			Char->DealDamage(DamageValue);
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Health Left: %f"), Char->Health));
			doOnce = !doOnce;
		}
	}
		
}

void AEnemy::OnSensed(const TArray<AActor*>& UpdatedActors)
{
	//change bool to true and act on behaviour tree

	for (int i = 0; i < UpdatedActors.Num(); i++)
	{
		FActorPerceptionBlueprintInfo info;
		AIPerComp->GetActorsPerception(UpdatedActors[i], info);

		if(Cast<APlayerCharacter>(UpdatedActors[i]))
			BBSettingBool(info.LastSensedStimuli[0].WasSuccessfullySensed());
	}
}

void AEnemy::DealDamage(float Damage)
{
	Health -= Damage;
	if(Health <= 0.0f)
	{
		//Play death animation

		//for now (maybe add an implement after)
		Destroy();
	}
}

void AEnemy::BBSettingBool_Implementation(bool spotted)
{
	//add additional things here (music on encounter change character speed etc...)
	if (spotted) 
	{
		//set speed?
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	}
	else
	{
		//set speed back to walking speed?
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * 0.5f;
	}
}

void AEnemy::SetHitBoxCol(bool hitting)
{
	hitting ? DamageCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly) : DamageCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

