#pragma once

/**
*   所有外部接口  
*/

#include "Common/MyShaderTypes.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ShaderTestLibrary.generated.h"


UCLASS()
class UShaderTestLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ShaderTestPlugin")
		static void FirstShaderDrawRenderTarget(UTextureRenderTarget2D* OutputRenderTarget, AActor* Ac, FLinearColor MyColor);

	UFUNCTION(BlueprintCallable, Category = "ShaderTestPlugin")
		static void DrawTestTextureShaderRenderTarget(AActor* ContextActor,  //能获取World的Actor
			UTextureRenderTarget2D* RenderTarget,  //渲染目标
			FTestTextureShaderStructData StructData,          //颜色
			UTexture* Texture                  //纹理
		);

	UFUNCTION(BlueprintCallable, Category = "ShaderTestPlugin")
		static void MyComputerShaderDraw(const UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget);
};
