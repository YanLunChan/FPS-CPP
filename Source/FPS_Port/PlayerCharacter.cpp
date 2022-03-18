// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"

#include "Projectile.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "FPS_PortGameModeBase.h"
#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "DrawDebugHelpers.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//connecting everything
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(40.0f, 95.0f);
	TurnRate = 45.0f;
	LookUpRate = 45.0f;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCam"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->AddRelativeLocation(FVector(-39.65f, 1.75f, 64.0f));
	Camera->bUsePawnControlRotation = true;

	HandsM = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Character"));

	//as name inplies only owner can see hands
	HandsM->SetOnlyOwnerSee(true);
	HandsM->SetupAttachment(Camera);
	HandsM->bCastDynamicShadow = false;
	HandsM->CastShadow = false;
	HandsM->AddRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	HandsM->AddRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	GunM = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	GunM->SetOnlyOwnerSee(true);
	GunM->bCastDynamicShadow = false;
	GunM->CastShadow = false;

	MuzzleLocal = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle Start"));
	MuzzleLocal->SetupAttachment(GunM);
	MuzzleLocal->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
	GunOffset = FVector(100.0f, 0.0f, 10.0f);
	BulletRange = 10000.0f;
	//for the line trace to appear at the muzzle of gun
	LineLocal = CreateDefaultSubobject<USceneComponent>(TEXT("LineTrace Start"));
	LineLocal->SetupAttachment(GunM);
	LineLocal->SetRelativeLocation(FVector(0.2f, 58.4f, 9.4f));
	
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	CoolDownOff = true;

	Chamber_Line = 6;
	Reserve_Line = 24;
	Chamber_Projectile = 1;
	Reserve_Projectile = 5;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//do it here, because skeletal mesh not yet created in the constructor (therefore doin it this way is safer)
	//PS:TEXT GripPoint is the skeletal name of the point you want to attach to...
	GunM->AttachToComponent(HandsM, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GripPoint"));

	World = GetWorld();
	AnimInstance = HandsM->GetAnimInstance();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::TurningRate);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookingRate);
	//inherited
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &APlayerCharacter::CharCrouch);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &APlayerCharacter::CharUnCrouch);

	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &APlayerCharacter::OnFire);
	PlayerInputComponent->BindAction(TEXT("Alt-Fire"), IE_Pressed, this, &APlayerCharacter::OnAltFire);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &APlayerCharacter::PlayReloadAnim);
}

void APlayerCharacter::DealDamage(float Damage)
{
	Health -= Damage;
	if(Health <= 0.0f)
	{
		//RagDoll / Restart afterwards

		//example
		Destroy();
	}
}

void APlayerCharacter::MoveForward(float axis)
{
	AddMovementInput(GetActorForwardVector() * axis);
	
}

void APlayerCharacter::MoveRight(float axis)
{
	AddMovementInput(GetActorRightVector() * axis);
	
}

void APlayerCharacter::OnFire()
{
	if (CoolDownOff) 
	{
		TraceForward();
		SetCoolDown();
	}
}

void APlayerCharacter::TraceForward_Implementation()
{
	if (World != NULL && Chamber_Line > 0)
	{
		FHitResult hit;
		FVector Start = LineLocal->GetComponentLocation();
		FVector End = Start + (Camera->GetForwardVector() * BulletRange);
		FCollisionQueryParams TraceParams;
		Chamber_Line--;
	
		//DrawDebugLine(World, Start, End, FColor::Red, false, 2.0f);
		bool hitting = World->LineTraceSingleByChannel(hit, Start, End, ECC_Visibility, TraceParams);
		DrawDebugBox(World, hit.ImpactPoint, FVector(5, 5, 5), FColor::Emerald, false, 2.0f);
		if (hitting)
		{
			EndPoint = hit.ImpactPoint;
			if(AEnemy* enemy = Cast<AEnemy>(hit.Actor))
			{
				(hit.BoneName == DeathShot) ? enemy->DealDamage(100.0f) : enemy->DealDamage(damageVal);
			}
		}
		else
		{
			EndPoint = hit.TraceEnd;
		}
		//play sounds and animation
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
		if (FireAnimation != NULL && AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.0f);
		}
		CoolDownOff = !CoolDownOff;
		SetCoolDown();
	}
}

void APlayerCharacter::OnAltFire()
{
	if (World != NULL && CoolDownOff && Chamber_Projectile > 0)
	{
		Chamber_Projectile--;
		SpawnRotation = GetControlRotation();
		//checking muzzle location if muzzle is not null then spawn local = to muzzle local if not then spawn local = actor local + spawn rotation.rotate vector etc...
		//used to make sure if muzzle local doesn't exist
		SpawnLocal = ((MuzzleLocal != nullptr) ? MuzzleLocal->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		AProjectile* placeholder = World->SpawnActor<AProjectile>(Projectile, SpawnLocal, SpawnRotation, ActorSpawnParams);
		if (placeholder != nullptr)
		{
			placeholder->SetOwner(this);

			if (FireSound != NULL)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
			if (FireAnimation != NULL && AnimInstance != NULL)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.0f);
			}
		}
		CoolDownOff = !CoolDownOff;
		ReserveToChamber(Reserve_Projectile, Chamber_Projectile, 1);
		SetCoolDown();
	}
}

void APlayerCharacter::TurningRate(float axis)
{
	AddControllerYawInput(axis * TurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookingRate(float axis)
{
	AddControllerPitchInput(axis * LookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::CharCrouch()
{
	Crouch();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Crouching"));
}

void APlayerCharacter::CharUnCrouch()
{
	UnCrouch();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("UnCrouching"));
}
void APlayerCharacter::Reload()
{
	ReserveToChamber(Reserve_Line, Chamber_Line, 6);
}

void APlayerCharacter::ReserveToChamber(int &Reserve, int &Chamber, int Amount)
{
	if(Reserve >= Amount)
	{
		//find difference between amound and chamber and remove that from reseve and add etc...
		Reserve -= (Amount - Chamber);
		Chamber = Amount;
	}
	else
	{
		Chamber += Reserve;
		Reserve = 0;
	}
}

void APlayerCharacter::PlayReloadAnim()
{
	if (ReloadAnimation != NULL && AnimInstance != NULL && Chamber_Line < 6)
	{
		AnimInstance->Montage_Play(ReloadAnimation, 1.0f);
	}
}

