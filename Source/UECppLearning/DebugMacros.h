#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location)                               if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 30.0f, 30, FColor::Red, true);
#define DRAW_SPHERE_SINGLE_FRAME(Location)                  if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 30.0f, 30, FColor::Red, false, -1.0f);

#define DRAW_LINE(StartLocation, EndLocation)               if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.0f, 0, 1.0f);
#define DRAW_LINE_SINGLE_FRAME(StartLocation, EndLocation)  if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.0f, 0, 1.0f);

#define DRAW_POINT(Location)                                if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.0f, FColor::Red, true);
#define DRAW_POINT_SINGLE_FRAME(Location)                   if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.0f, FColor::Red, false, -1.0f);

#define DRAW_VECTOR(StartLocation, EndLocation)             if(GetWorld()) \
    {\
        DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.0f, 0, 1.0f); \
        DrawDebugPoint(GetWorld(), EndLocation, 15.0f, FColor::Red, true); \
    }

#define DRAW_VECTOR_SINGLE_FRAME(StartLocation, EndLocation)    if(GetWorld()) \
    {\
        DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.0f, 0, 1.0f);    \
        DrawDebugPoint(GetWorld(), EndLocation, 15.0f, FColor::Red, false, -1.0f);      \
    }