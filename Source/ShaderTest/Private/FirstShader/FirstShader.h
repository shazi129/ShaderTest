#pragma once

#include "ShaderParameterStruct.h"
#include "Common/MyGlobalShaderBase.h"

class FFirstShaderVS : public FMyGlobalShaderBase
{
	DECLARE_SHADER_TYPE(FFirstShaderVS, Global);

public:
	FFirstShaderVS() {}

	FFirstShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer) : FMyGlobalShaderBase(Initializer){}
};

class FFirstShaderPS : public FMyGlobalShaderBase
{
public:
	DECLARE_GLOBAL_SHADER(FFirstShaderPS);

	SHADER_USE_PARAMETER_STRUCT(FFirstShaderPS, FMyGlobalShaderBase);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FVector4f, SimpleColor)
		RENDER_TARGET_BINDING_SLOTS()
	END_SHADER_PARAMETER_STRUCT()
};