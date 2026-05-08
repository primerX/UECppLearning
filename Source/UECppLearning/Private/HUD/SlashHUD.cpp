// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
    Super::BeginPlay();
    UWorld* world = GetWorld();
    if (world)
    {
        APlayerController* playerController = world->GetFirstPlayerController();
        if (playerController && SlashOverlayClass)
        {
            SlashOverlay = CreateWidget<USlashOverlay>(playerController, SlashOverlayClass);
            if (SlashOverlay) SlashOverlay->AddToViewport();
        }
    }
}
