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

	static FBufferRHIRef CreateVertexBuffer(const TArray<FVector4>& VertexList);

	static FBufferRHIRef CreateVertexBuffer(const TArray<FMyTextureVertex>& VertexList);

	static FBufferRHIRef CreateIndexBuffer(const uint16* Indices, uint16 NumIndices);
};
