#pragma once
#include "CoreMinimal.h"

#define AddSlateVertex(Verts, Pos, UV) \
Verts.AddZeroed();\
{\
	FSlateVertex& NewVert = Verts.Last();\
	NewVert.Position = Pos;\
	NewVert.TexCoords[0] = UV.X;\
	NewVert.TexCoords[1] = UV.Y;\
	NewVert.TexCoords[2] = NewVert.TexCoords[3] = 1.0f;\
	NewVert.Color = VertColor;\
}