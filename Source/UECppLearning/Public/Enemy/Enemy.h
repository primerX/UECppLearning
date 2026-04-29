// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"
#include "Enemy.generated.h"


class USoundBase;
class UWidgetComponent;
class UHealthBarComponent;
class AAIController;
class UPawnSensingComponent;

UCLASS()
class UECPPLEARNING_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
    void CheckPatrolTarget();
    void CheckCombatTarget();

    virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    virtual void Die() override;
    virtual void Destroyed() override;
    virtual void Attack() override;
    virtual void AttackEnd() override;

    virtual bool CanAttack() override;
    virtual void HandleDamage(float DamageAmount) override;
    virtual int32 PlayDeathMontage() override;

    UPROPERTY(EditAnywhere, Category = Combat)
    float DeathLifeSpan = 3.f;

    UPROPERTY(BlueprintReadOnly)
    TEnumAsByte<EDeathPose> DeathPose;

    UPROPERTY(BlueprintReadOnly)
    EEnemyState EnemyState = EEnemyState::EES_Patrolling;

    bool InTargetRange(AActor* Target, double Radius) const;

    void PatrolTimerFinished();
    void MoveToTarget(AActor* Target);
    AActor* ChoosePatrolTarget();

    UFUNCTION()
    void PawnSeen(APawn* SeenPawn);
private:

    UPROPERTY(VisibleAnywhere)
    UHealthBarComponent* HealthBarComponent;

    UPROPERTY(VisibleAnywhere)
    UPawnSensingComponent* PawnSensingComponent;

    UPROPERTY(EditAnywhere)
    TSubclassOf<class AWeapon> WeaponClass;

    UPROPERTY()
    AActor* CombatTarget;
    
    UPROPERTY(EditAnywhere)
    double CombatRadius = 500.f;

    UPROPERTY(EditAnywhere)
    double AttackRadius = 150.f;

    // AI Navigation
    UPROPERTY()
    AAIController* EnemyController;

    FTimerHandle PatrolTimer;

    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    double PatrolRadius = 200.0f;

    // 当前巡逻目标
    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    AActor* PotrayTarget;

    UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
    TArray<AActor*> PatrolTargets;

    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float PatrolWaitMin = 2.0f;

    UPROPERTY(EditAnywhere, Category = "AI Navigation")
    float PatrolWaitMax = 5.0f;
    
    // AI Behavior
    void HideHealthBar();
    void ShowHealthBar();
    void LoseInterest();
    void StartPatrolling();
    void ChaseTarget();

    bool IsOutsideCombatRadius() const;
    bool IsOutsideAttackRadius() const;
    bool IsInsideAttackRadius() const;

    bool IsChasing() const;
    bool IsAttacking() const;
    bool IsDead() const;
    bool IsEngaged() const;

    void ClearPatrolTimer();

    // Combat
    void StartAttackTimer();
    void ClearAttackTimer();

    FTimerHandle AttackTimer;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackMinTime = 0.5f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float AttackMaxTime = 1.5f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float PatrollingSpeed = 150.f;

    UPROPERTY(EditAnywhere, Category = "Combat")
    float ChasingSpeed = 300.f;
public:	

};
