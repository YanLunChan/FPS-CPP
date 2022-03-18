// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Enemy.h"
#include "Explosion.h"
#include "PlayerCharacter.h"
#include "DrawDebugHelpers.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	CollisionSphere->InitSphereRadius(20.0f);

	RootComponent = CollisionSphere;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectile::OnHit(UPrimitiveComponent* hitCom, AActor* other, UPrimitiveComponent* otherCom, int32 bodyIndex, bool bFromSweep, const FHitResult &hit)
{
	//do once only because if it hits multiple things it will crash due to it trying to read a destroyed object
	if (GetOwner() != other)
	{
		AEnemy* enemy = Cast<AEnemy>(other);
		APlayerCharacter* player = Cast<APlayerCharacter>(other);
		if (enemy)
		{
			enemy->DealDamage(damageVal);
		}
		else if (player)
		{
			player->DealDamage(damageVal);
		}
		//read that garbage collecter will collect it if it's null
		enemy = NULL;
		player = NULL;
		//add explosion if needed
		if (Explosion != NULL)
		{
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			GetWorld()->SpawnActor<AExplosion>(Explosion, this->GetActorLocation(), FRotator::ZeroRotator, ActorSpawnParams);
		}

		//add delay and then destroy
		Destroy();
	}
}

