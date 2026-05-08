// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UECPPLEARNING_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    void RengenStamina(float DeltaTime);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
    //Current Health
    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float Health;
		
    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float MaxHealth;

    // Current Stamina
    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float Stamina;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float MaxStamina;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    int32 Gold;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    int32 Souls;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float DodgeCost;

    UPROPERTY(EditAnywhere, Category = "Actor Attributes")
    float StaminaRegenRate;
public:
    void ReceiveDamage(float DamageAmount);
    void UseStamina(float StaminaCost);
    float GetHealthPercentage() const { return Health / MaxHealth; }
    float GetStaminaPercentage() const { return Stamina / MaxStamina; }
    bool IsAlive() const { return Health > 0.f; }
    void AddSouls(int32 NumberOfSouls);
    void AddGold(int32 AmountOfGold);
    FORCEINLINE int32 GetGold() const { return Gold; }
    FORCEINLINE int32 GetSouls() const { return Souls; }
    FORCEINLINE float GetDodgeCost() const { return DodgeCost; }
    FORCEINLINE float GetStamina() const { return Stamina; }
};
