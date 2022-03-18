// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosion.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "DrawDebugHelpers.h"
#include "Enemy.h"
#include "PlayerCharacter.h"
// Sets default values
AExplosion::AExplosion()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Explosion = CreateDefaultSubobject<URadialForceComponent>(TEXT("Explosion Force"));
	RootComponent = Explosion;
	Explosion->Radius = Radius;
	Explosion->ImpulseStrength = Strength;
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AExplosion::BeginPlay()
{
	Super::BeginPlay();	
	//list things that are within the radius and then apply explosion damage
	TArray<FHitResult> Hits;
	const FVector Start = GetActorLocation();
	const FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	DrawDebugSphere(GetWorld(), Start, Radius, 50, FColor::Cyan, true);
	if (GetWorld()->SweepMultiByChannel(Hits, Start, Start, FQuat::Identity, ECC_Pawn, Sphere))
	{
		for (const FHitResult hit : Hits)
		{
			AEnemy* enemy = Cast<AEnemy>(hit.Actor);
			APlayerCharacter* player = Cast<APlayerCharacter>(hit.Actor);
			if (enemy)
			{
				enemy->DealDamage(damageVal);
			}
			else if (player)
			{
				player->DealDamage(damageVal * 0.5f);
			}
			//read that garbage collecter will collect it if it's null
			enemy = NULL;
			player = NULL;
		}
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("working"));
	}
	Explosion->FireImpulse();
}

// Called every frame
void AExplosion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

