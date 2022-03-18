// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class AExplosion;

UCLASS()
class FPS_PORT_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintREadOnly, Category = Movement)
		class UProjectileMovementComponent* ProjectileMovement;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* hitCom, AActor* other, UPrimitiveComponent* otherCom, int32 bodyIndex, bool bFromSweep, const FHitResult& hit);

	UPROPERTY(EditAnywhere)
		float damageVal = 20.0f;
	UPROPERTY(EditAnywhere)
		float timer = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
		TSubclassOf<AExplosion> Explosion;
};
