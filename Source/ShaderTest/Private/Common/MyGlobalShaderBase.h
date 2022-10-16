#pragma once

#include "GlobalShader.h"

class FMyGlobalShaderBase : public FGlobalShader
{

public:
	FMyGlobalShaderBase();

	FMyGlobalShaderBase(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters,
		FShaderCompilerEnvironment& OutEnvironment);
};
