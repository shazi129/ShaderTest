#include "FirstShader/FirstShader.h"

FFirstShaderPS::FFirstShaderPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FMyGlobalShaderBase(Initializer)
{
	SimpleColorVal.Bind(Initializer.ParameterMap, TEXT("SimpleColor"));
}

void FFirstShaderPS::SetColor(FRHICommandListImmediate& RHICmdList, const FPixelShaderRHIRef ShaderRHI, const FLinearColor& MyColor)
{
	SetShaderValue(RHICmdList, ShaderRHI, SimpleColorVal, MyColor);
}

IMPLEMENT_SHADER_TYPE(, FFirstShaderVS, TEXT("/Plugin/ShaderTest/Private/FirstShader.usf"), TEXT("MainVS"), SF_Vertex)
IMPLEMENT_SHADER_TYPE(, FFirstShaderPS, TEXT("/Plugin/ShaderTest/Private/FirstShader.usf"), TEXT("MainPS"), SF_Pixel)