#pragma once

/**
*   �����ⲿ�ӿ�  
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
	UFUNCTION(BlueprintCallable, Category = "ShaderTestPlugin", meta = (DefaultToSelf = "WorldContextObject"))
		static void FirstShaderDrawRenderTarget(UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget, FLinearColor MyColor, bool UsingRDG=false);


// 	UFUNCTION(BlueprintCallable, Category = "ShaderTestPlugin")
// 		static void DrawTestTextureShaderRenderTarget(AActor* ContextActor,  //�ܻ�ȡWorld��Actor
// 			UTextureRenderTarget2D* RenderTarget,  //��ȾĿ��
// 			FTestTextureShaderStructData StructData,          //��ɫ
// 			UTexture* Texture                  //����
// 		);
// 
// 	UFUNCTION(BlueprintCallable, Category = "ShaderTestPlugin")
// 		static void MyComputerShaderDraw(const UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget);
};
