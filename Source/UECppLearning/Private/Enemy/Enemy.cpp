// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "UECppLearning/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    GetMesh()->SetGenerateOverlapEvents(true);
    GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

    Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

    HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarComponent"));
    HealthBarComponent->SetupAttachment(GetRootComponent());

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
    if(HealthBarComponent)
    {
        HealthBarComponent->SetVisibility(false);
    }
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
    if (HitReactMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(HitReactMontage);
            AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
        }
    }
}

void AEnemy::Die()
{
    PlayDeathMontage();

    if(HealthBarComponent)
    {
        HealthBarComponent->SetVisibility(false);
    }
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetLifeSpan(3.0f);
}

void AEnemy::PlayDeathMontage()
{
    if (DeathMontage)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(DeathMontage);
            const int32 Section = FMath::RandRange(0, DeathMontage->CompositeSections.Num() - 1);
            FName SectionName = FName();
            switch (Section)
            {
                case 0:
                    SectionName = FName("Death1");
                    DeathPose = EDeathPose::EDP_Death1;
                    break;
                case 1:
                    SectionName = FName("Death2");
                    DeathPose = EDeathPose::EDP_Death2;
                    break;
                case 2: 
                    SectionName = FName("Death3");
                    DeathPose = EDeathPose::EDP_Death3;
                    break;
                case 3:
                    SectionName = FName("Death4");
                    DeathPose = EDeathPose::EDP_Death4;
                    break;
                case 4:
                    SectionName = FName("Death5");
                    DeathPose = EDeathPose::EDP_Death5;
                    break;
                default:
                    break;
            }
            AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
        }
    }
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (CombatTarget)
    {
        const double DistanceToTarget = FVector::Dist(GetActorLocation(), CombatTarget->GetActorLocation());
        if (DistanceToTarget > CombatRadius)
        {
            CombatTarget = nullptr;
            if(HealthBarComponent)
            {
                HealthBarComponent->SetVisibility(false);
            }
        }
    }
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
    //DRAW_SPHERE_COLOR(ImpactPoint, FColor::Red);
    if(HealthBarComponent)
    {
        HealthBarComponent->SetVisibility(true);
    }

    if(Attributes && Attributes->IsAlive())
    {
        DirectionalHitReact(ImpactPoint);
    }
    else
    {
        Die();
    }
    

    if(HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this, 
            HitSound, 
            ImpactPoint
        );
    }

    if(HitParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            HitParticles,
            ImpactPoint
        );
    }
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
    const FVector Forward = GetActorForwardVector();
    // We want to ignore the Z difference when calculating the angle, so we create a new vector that is the same as the impact point but with the Z value of the enemy's location.
    const FVector ImpactLowered = FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
    const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
    const double CosTheta = FVector::DotProduct(Forward, ToHit);
    double Theta = FMath::Acos(CosTheta);
    Theta = FMath::RadiansToDegrees(Theta);

    const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
    if (CrossProduct.Z < 0)
    {
        Theta *= -1.0f;
    }

    FName SectionName = FName("FromBack");
    if (Theta >= -45.0f && Theta < 45.0f)
    {
        SectionName = FName("FromFront");
    }
    else if (Theta >= -135.f && Theta < -45.f)
    {
        SectionName = FName("FromLeft");
    }
    else if (Theta >= 45.f && Theta < 135.f)
    {
        SectionName = FName("FromRight");
    }

    PlayHitReactMontage(SectionName);

    //UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.0f, 10.0f, FColor::Green, 5.0f);
    //UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 100.0f, 10.0f, FColor::Green, 5.0f);
    //UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 100.0f, 10.0f, FColor::Blue, 5.0f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if(Attributes)
    {
        Attributes->ReceiveDamage(DamageAmount);
        if(HealthBarComponent)
        {
            HealthBarComponent->SetHealthBarPercent(Attributes->GetHealthPercentage());
        }
    }

    CombatTarget = EventInstigator ? EventInstigator->GetPawn() : nullptr;

    return DamageAmount;
}

