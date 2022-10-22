#include "TextureShader/TestTextureShader.h"

IMPLEMENT_UNIFORM_BUFFER_STRUCT(FSimpleUniformStruct, "SimpleUniformStruct");
IMPLEMENT_SHADER_TYPE(, FTestTextureShaderVS, TEXT("/Plugin/ShaderTest/Private/TestTextureShader.usf"), TEXT("MainVS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FTestTextureShaderPS, TEXT("/Plugin/ShaderTest/Private/TestTextureShader.usf"), TEXT("MainPS"), SF_Pixel);