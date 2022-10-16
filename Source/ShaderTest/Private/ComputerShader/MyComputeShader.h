#pragma once

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"


class FMyComputeShader : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FMyComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FMyComputeShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_UAV(RWTexture2D<uint>, OutputSurface)
		SHADER_PARAMETER(FVector2D, TextureSize) 
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}

	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	}
};
