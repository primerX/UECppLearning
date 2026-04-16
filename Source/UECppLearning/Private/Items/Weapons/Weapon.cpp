// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Character/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
    if (InParent)
    {
        AttachMeshToSocket(InParent, InSocketName);
        ItemState = EItemState::EIS_Equipped;
        if (EquipSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this, 
                EquipSound,
                GetActorLocation()
            );
        }
    }

    if (Sphere)
    {
        Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
    ItemMesh->AttachToComponent(InParent, AttachmentRules, InSocketName);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
