// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Explosion.generated.h"

class URadialForceComponent;

UCLASS()
class FPS_PORT_API AExplosion : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
		URadialForceComponent* Explosion;
protected:
	UPROPERTY(EditDefaultsOnly,Category = "Explosion")
	float Radius = 400.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Explosion")
	float Strength = 1000.0f;
	float damageVal = 50.0f;
};
