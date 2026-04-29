// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "UECppLearning/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Engine/Engine.h"


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

    HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarComponent"));
    HealthBarComponent->SetupAttachment(GetRootComponent());

    GetCharacterMovement()->bOrientRotationToMovement = true;
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
    PawnSensingComponent->SetPeripheralVisionAngle(50.f);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
    if(HealthBarComponent)
    {
        HealthBarComponent->SetVisibility(false);
    }

    EnemyController = Cast<AAIController>(GetController());
    MoveToTarget(PotrayTarget);

    if(PawnSensingComponent)
    {
        PawnSensingComponent->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
    }

    UWorld* World = GetWorld();
    if (World && WeaponClass)
    {
        AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
        DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
        EquippedWeapon = DefaultWeapon;
    }
}


void AEnemy::Die()
{
    EnemyState = EEnemyState::EES_Dead;
    PlayDeathMontage();
    ClearAttackTimer();
    HideHealthBar();
    DisableCapsule();
    SetLifeSpan(DeathLifeSpan);
    GetCharacterMovement()->bOrientRotationToMovement = false;
}

int32 AEnemy::PlayDeathMontage()
{
    const int32 Selection = Super::PlayDeathMontage();
    TEnumAsByte<EDeathPose> Pose(Selection);
    if (Pose < EDeathPose::EDP_MAX)
    {
        DeathPose = Pose;
    }

    return Selection;
}

void AEnemy::Destroyed()
{
    if(EquippedWeapon)
    {
        EquippedWeapon->Destroy();
    }
}

void AEnemy::Attack()
{
    EnemyState = EEnemyState::EES_Engaged;
    Super::Attack();
    PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
    bool bCanAttack =
        IsInsideAttackRadius() &&
        !IsAttacking() && 
        !IsEngaged() &&
        !IsDead();
    return bCanAttack;
}

void AEnemy::AttackEnd()
{
    EnemyState = EEnemyState::EES_NoState;
    CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
    Super::HandleDamage(DamageAmount);
    if(Attributes && HealthBarComponent)
    {
        HealthBarComponent->SetHealthBarPercent(Attributes->GetHealthPercentage());
    }

}

bool AEnemy::InTargetRange(AActor* Target, double Radius) const
{
    if(Target == nullptr)
    {
        return false;
    }
    const double DistanceToTarget = FVector::Dist(Target->GetActorLocation(), GetActorLocation());
    return DistanceToTarget <= Radius;
}

void AEnemy::PatrolTimerFinished()
{
    MoveToTarget(PotrayTarget);
}

void AEnemy::MoveToTarget(AActor* Target)
{
    if(EnemyController == nullptr || Target == nullptr)
    {
        return;
    }

    FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(Target);
    MoveRequest.SetAcceptanceRadius(50.f);
    EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
    TArray<AActor*> ValidTargets;
    for(AActor* Target: PatrolTargets)
    {
        if(Target != PotrayTarget)
        {
            ValidTargets.AddUnique(Target);
        }
    }
    const int32 ValidTargetNum = ValidTargets.Num();
    if (ValidTargetNum > 0)
    {
        const int32 TargetIndex = FMath::RandRange(0, ValidTargetNum - 1);
        return ValidTargets[TargetIndex];
    }

    return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
    bool bShouldChaseTarget =
        EnemyState != EEnemyState::EES_Dead &&
        EnemyState != EEnemyState::EES_Chasing && 
        EnemyState < EEnemyState::EES_Attacking &&
        SeenPawn->ActorHasTag("SlashCharacter");

    if (bShouldChaseTarget)
    {
        CombatTarget = SeenPawn;
        ClearPatrolTimer();
        ChaseTarget();
    }
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if(IsDead()) return;

    if (EnemyState > EEnemyState::EES_Patrolling)
    {
        CheckCombatTarget();
    }
    else
    {
        CheckPatrolTarget();
    }

}

void AEnemy::CheckPatrolTarget()
{
    if (InTargetRange(PotrayTarget, PatrolRadius))
    {
        PotrayTarget = ChoosePatrolTarget();
        
        //MoveToTarget(PotrayTarget);
        const float PatrolWaitTime = FMath::FRandRange(PatrolWaitMin, PatrolWaitMax);
        GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, PatrolWaitTime);
    }
}

void AEnemy::CheckCombatTarget()
{
    if (IsOutsideCombatRadius())
    {
        ClearAttackTimer();
        LoseInterest();
        if (!IsEngaged()) StartPatrolling();
    }
    else if (IsOutsideAttackRadius() && !IsChasing())
    {
        ClearAttackTimer();
        if (!IsEngaged()) ChaseTarget();
    }
    else if(CanAttack())
    {
        StartAttackTimer();
    }
}


void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
    ShowHealthBar();

    if(IsAlive())
    {
        DirectionalHitReact(ImpactPoint);
    }
    else
    {
        Die();
    }
    
    PlayHitSound(ImpactPoint);
    SpawnHitParticles(ImpactPoint);
}


float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    HandleDamage(DamageAmount);
    CombatTarget = EventInstigator ? EventInstigator->GetPawn() : nullptr;
    ChaseTarget();

    return DamageAmount;
}

void AEnemy::HideHealthBar()
{
    if(HealthBarComponent)
    {
        HealthBarComponent->SetVisibility(false);
    }
}

void AEnemy::ShowHealthBar()
{
    if(HealthBarComponent)
    {
        HealthBarComponent->SetVisibility(true);
    }
}

void AEnemy::LoseInterest()
{
    CombatTarget = nullptr;
    HideHealthBar();
}

void AEnemy::StartPatrolling()
{
    EnemyState = EEnemyState::EES_Patrolling;
    GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
    MoveToTarget(PotrayTarget);
}

void AEnemy::ChaseTarget()
{
    EnemyState = EEnemyState::EES_Chasing;
    GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
    MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius() const
{
    return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius() const
{
    return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius() const
{
    return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing() const
{
    return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking() const
{
    return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead() const
{
    return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged() const
{
    return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
    GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
    EnemyState = EEnemyState::EES_Attacking;
    const float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
    GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
    GetWorldTimerManager().ClearTimer(AttackTimer);
}
