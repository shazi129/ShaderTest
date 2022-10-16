#pragma once

#include "ShaderParameterStruct.h"
#include "Common/MyShaderTypes.h"
#include "Common/TestShaderUtils.h"
#include "Common/MyGlobalShaderBase.h"

class FTestTextureShaderVS : public FMyGlobalShaderBase
{
	DECLARE_SHADER_TYPE(FTestTextureShaderVS, Global);

public:
	FTestTextureShaderVS() {}

	FTestTextureShaderVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
		: FMyGlobalShaderBase(Initializer)
	{

	}
};

BEGIN_UNIFORM_BUFFER_STRUCT(FSimpleUniformStruct, )
	SHADER_PARAMETER(FVector4, ColorOne)
	SHADER_PARAMETER(FVector4, ColorTwo)
	SHADER_PARAMETER(FVector4, ColorThree)
	SHADER_PARAMETER(FVector4, ColorFour)
	SHADER_PARAMETER(int32, ColorIndex)
END_UNIFORM_BUFFER_STRUCT()

class FTestTextureShaderPS : public FMyGlobalShaderBase
{
public:
	DECLARE_GLOBAL_SHADER(FTestTextureShaderPS);
	SHADER_USE_PARAMETER_STRUCT(FTestTextureShaderPS, FMyGlobalShaderBase);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_TEXTURE(Texture2D, MyTextrue)
		SHADER_PARAMETER_SAMPLER(SamplerState, MyTextureSampler)
		SHADER_PARAMETER_STRUCT_REF(FSimpleUniformStruct, SimpleUniformStruct)
	END_SHADER_PARAMETER_STRUCT()
};