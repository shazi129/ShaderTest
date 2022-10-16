#pragma once

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
	DECLARE_SHADER_TYPE(FFirstShaderPS, Global);

private:
	LAYOUT_FIELD(FShaderParameter, SimpleColorVal);

public:
	FFirstShaderPS() {}

	FFirstShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	//template<typename TShaderRHIParamRef>
	void SetColor(FRHICommandListImmediate& RHICmdList, const FPixelShaderRHIRef ShaderRHI, const FLinearColor& MyColor);
};