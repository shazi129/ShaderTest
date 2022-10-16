#include "ComputerShader/MyComputeShader.h"

IMPLEMENT_SHADER_TYPE(, FMyComputeShader, TEXT("/Plugin/ShaderTest/Private/TestComputeShader.usf"), TEXT("MainCS"), SF_Compute);