#pragma once

#include "MyShaderTypes.generated.h"

USTRUCT(BlueprintType)
struct FTestTextureShaderStructData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor ColorOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor ColorTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor ColorThree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor ColorFour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 ColorIndex;
};