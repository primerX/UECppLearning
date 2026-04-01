# UECppLearning

`UECppLearning` is a C++ learning project based on Unreal Engine 5.1.

## Overview

This repository focuses on implementing core gameplay features in C++:

- Third-person character movement and camera control
- Item overlap detection
- Weapon equipping flow
- Character state switching after equip
- Basic math helpers for animation/offset logic (`Sin`/`Cos` transforms)

## Current Gameplay Features

### Character (`ASlashCharacter`)

- Movement input:
  - `MoveForward`
  - `MoveRight`
  - `Turn`
  - `LookUp`
- Jump input binding
- `E` key equip interaction
- Spring arm + follow camera setup
- Groom components for hair and eyebrows

### Item System (`AItem`)

- Static mesh as root
- Sphere overlap trigger
- Begin/End overlap callbacks via dynamic delegates
- Notifies character of current overlapping item
- Utility math functions:
  - `TransformedSin()`
  - `TransformCos()`

### Weapon (`AWeapon`)

- Inherits from `AItem`
- Supports attaching to character mesh socket (`RightHandSocket`) via `Equip(...)`
- Uses overlap behavior inherited from `AItem`

## Tech Stack

- Unreal Engine `5.1`
- C++ (project/toolchain compatible with C++14 environment)
- Visual Studio 2022

## Project Structure

- `Source/UECppLearning/Public/Character/`
- `Source/UECppLearning/Private/Character/`
- `Source/UECppLearning/Public/Items/`
- `Source/UECppLearning/Private/Items/`

## How to Run

1. Open `UECppLearning.uproject` with Unreal Engine 5.1.
2. If needed, regenerate Visual Studio project files.
3. Build `Development Editor | Win64`.
4. Launch and test movement, overlap, and equip interactions.

## Input Notes

- `W/A/S/D`: Move
- Mouse: Turn/LookUp
- `Space`: Jump
- `E`: Equip overlapping weapon

## Repository

- GitHub: https://github.com/primerX/UECppLearning
