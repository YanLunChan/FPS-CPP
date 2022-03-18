// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

class UBoxComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;
UCLASS()
class FPS_PORT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere)
		UBoxComponent* DamageCollision;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* hitCom, AActor* other, UPrimitiveComponent* otherCom, int32 bodyIndex, bool bFromSweep, const FHitResult& hit);

	//ai perception
	UPROPERTY(VisibleDefaultsOnly, Category = Enemy)
		UAIPerceptionComponent* AIPerComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Enemy)
		UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
		void OnSensed(const TArray<AActor*>& UpdatedActors);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		FVector BaseLocal;
	UPROPERTY(VisibleAnywhere, Category = Movement)
		float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Health = 100.0f;

	UPROPERTY(EditAnywhere)
		float DamageValue = 5.0f;

public:
	void DealDamage(float Damage);

	UFUNCTION(BlueprintNativeEvent)
	void BBSettingBool(bool spotted);
	void BBSettingBool_Implementation(bool spotted);

	UFUNCTION(BlueprintCallable)
	void SetHitBoxCol(bool hitting);

	UPROPERTY(BlueprintReadWrite)
		bool doOnce;
};
