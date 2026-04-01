// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Weapons/Weapon.h"

// Sets default values
ASlashCharacter::ASlashCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 400.0f, 0.f);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 300.f;

    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(CameraBoom);

    Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
    Hair->SetupAttachment(GetMesh());
    Hair->AttachmentName = FString("head");

    Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
    Eyebrows->SetupAttachment(GetMesh());
    Eyebrows->AttachmentName = FString("head");
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
    PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
    PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
    PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);

    PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
}

void ASlashCharacter::MoveForward(float Value)
{
    if (Controller != nullptr && Value != 0.0f)
    {
        const FRotator rotation = Controller->GetControlRotation();
        const FRotator yawRotation(0, rotation.Yaw, 0);
        const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(direction, Value);
    }
}

void ASlashCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void ASlashCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void ASlashCharacter::MoveRight(float Value)
{
    if (Controller != nullptr && Value != 0.0f)
    {
        const FRotator rotation = Controller->GetControlRotation();
        const FRotator yawRotation(0, rotation.Yaw, 0);
        const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(direction, Value);
    }
}

void ASlashCharacter::EKeyPressed()
{
    AWeapon* overlappingWeapon = Cast<AWeapon>(OverlappingItem);
    if (overlappingWeapon)
    {
        overlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
        CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
    }
}
