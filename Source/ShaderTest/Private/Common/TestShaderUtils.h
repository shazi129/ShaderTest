#pragma once

#include "TestShaderUtils.generated.h"


struct FMyTextureVertex
{
	FVector4 Position;
	FVector2D UV;

	FMyTextureVertex(FVector4 ParamPos, FVector2D ParamUV)
	{
		Position = ParamPos;
		UV = ParamUV;
	}
};


UCLASS()
class UTestShaderUtils : public UObject
{
	GENERATED_BODY()

public:

	static FVertexBufferRHIRef CreateVertexBuffer(const TArray<FVector4>& VertexList);

	static FVertexBufferRHIRef CreateVertexBuffer(const TArray<FMyTextureVertex>& VertexList);

	static FIndexBufferRHIRef CreateIndexBuffer(const uint16* Indices, uint16 NumIndices);
};
