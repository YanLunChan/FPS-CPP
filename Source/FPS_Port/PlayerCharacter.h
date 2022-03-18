 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class AProjectile;
class USoundBase;
class UAnimMontage;
class UAnimInstance;
class UWorld;

UCLASS()
class FPS_PORT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//visual stuff
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* HandsM;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* GunM;
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USceneComponent* MuzzleLocal;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USceneComponent* LineLocal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = camera)
		UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = camera)
		float TurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = camera)
		float LookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<AProjectile> Projectile;
	UPROPERTY(VisibleDefaultsOnly, Category = Gameplay)
		float BulletRange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;
	UAnimInstance* AnimInstance;
	UWorld* World;
	FRotator SpawnRotation;
	FVector SpawnLocal;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Health = 100.0f;
	
	void DealDamage(float Damage);
protected:

	void MoveForward(float axis);
	void MoveRight(float axis);
	void OnFire();
	void OnAltFire();
	void TurningRate(float axis);
	void LookingRate(float axis);
	void CharCrouch();
	void CharUnCrouch();

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();

	FName DeathShot = TEXT("head");
	float damageVal = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
		FVector EndPoint;

	//cooldown per hit shared between left and right click
	UFUNCTION(BlueprintImplementableEvent)
	void SetCoolDown();

	UPROPERTY(BlueprintReadWrite)
	bool CoolDownOff;

	//ammo numbers (need blueprint read only for ui)
		UPROPERTY(BlueprintReadOnly)
		int Chamber_Line;
		UPROPERTY(BlueprintReadOnly)
		int Reserve_Line;
		UPROPERTY(BlueprintReadOnly)
		int Chamber_Projectile;
		UPROPERTY(BlueprintReadOnly)
		int Reserve_Projectile;

	UFUNCTION(BlueprintCallable)
		void Reload();
	void ReserveToChamber(int &Reserve, int &Chamber, int Amount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* ReloadAnimation;
	void PlayReloadAnim();
};
