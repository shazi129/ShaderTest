#include "ShaderTestLibrary.h"
#include "FirstShader/FirstShader.h"
#include "Common/TestShaderUtils.h"

static void FirstShader_RenderThread
(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	FName TextureRenderTargetName,
	FLinearColor MyColor
)
{
	check(IsInRenderingThread());

#if WANTS_DRAW_MESH_EVENTS
	FString EventName;
	TextureRenderTargetName.ToString(EventName);
	SCOPED_DRAW_EVENTF(RHICmdList, SceneCapture, TEXT("FirstShader_RenderThread %s"), *EventName);
#else
	SCOPED_DRAW_EVENT(RHICmdList, FirstShader_RenderThread);
#endif

	FRHITexture2D* RenderTargetTexture = OutTextureRenderTargetResource->GetRenderTargetTexture();

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store, OutTextureRenderTargetResource->TextureRHI);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawTestShader"));

	FIntPoint DrawTargetResolution(OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY());
	RHICmdList.SetViewport(0, 0, 0.0f, DrawTargetResolution.X, DrawTargetResolution.Y, 1.0f);

	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
	TShaderMapRef<FFirstShaderVS> VertexShader(GlobalShaderMap);
	TShaderMapRef<FFirstShaderPS> PixelShader(GlobalShaderMap);

	// Set the graphic pipeline state.  
	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	PixelShader->SetColor(RHICmdList, PixelShader.GetPixelShader(), MyColor);

	//顶点数据
	TArray<FVector4> VertexList;
	VertexList.Add(FVector4(-1.0f, 1.0f, 0, 1.0f));
	VertexList.Add(FVector4(1.0f, 1.0f, 0, 1.0f));
	VertexList.Add(FVector4(-1.0f, -1.0f, 0, 1.0f));
	VertexList.Add(FVector4(1.0f, -1.0f, 0, 1.0f));

	int VertexSize = sizeof(FVector4) * VertexList.Num();

	FRHIResourceCreateInfo CreateInfo(TEXT("FirstShader"));
	FBufferRHIRef VertexBufferRHI = RHICreateVertexBuffer(VertexSize, BUF_Volatile, CreateInfo);
	void* VoidPtr = RHILockBuffer(VertexBufferRHI, 0, VertexSize, RLM_WriteOnly);
	FPlatformMemory::Memcpy(VoidPtr, VertexList.GetData(), VertexSize);
	RHIUnlockBuffer(VertexBufferRHI);

	//顶点索引
	TArray<uint16, TInlineAllocator<6> > IndexList({0, 1, 2, 2, 1, 3});
	int IndexSize = sizeof(uint16) * IndexList.Num();

	FBufferRHIRef IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16), IndexSize, BUF_Volatile, CreateInfo);
	void* VoidPtr2 = RHILockBuffer(IndexBufferRHI, 0, IndexSize, RLM_WriteOnly);
	FPlatformMemory::Memcpy(VoidPtr2, IndexList.GetData(), IndexSize);
	RHIUnlockBuffer(IndexBufferRHI);

	RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);
	RHICmdList.DrawIndexedPrimitive(
		IndexBufferRHI,
		/*BaseVertexIndex=*/ 0,
		/*MinIndex=*/ 0,
		/*NumVertices=*/ 4 ,
		/*StartIndex=*/ 0,
		/*NumPrimitives=*/2, 
		/*NumInstances=*/ 1
	);

	RHICmdList.EndRenderPass();
}

static void DrawUVDisplacementToRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	FName TextureRenderTargetName,
	FLinearColor MyColor)
{
	check(IsInRenderingThread());

#if WANTS_DRAW_MESH_EVENTS
	// 	FString EventName;
	// 	TextureRenderTargetName.ToString(EventName);
	// 	SCOPED_DRAW_EVENTF(RHICmdList, SceneCapture, TEXT("LensDistortionDisplacementGeneration %s"), *EventName);
#else
	SCOPED_DRAW_EVENT(RHICmdList, DrawUVDisplacementToRenderTarget_RenderThread);
#endif

	FRHITexture2D* RenderTargetTexture = OutTextureRenderTargetResource->GetRenderTargetTexture();

	RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::SRVMask, ERHIAccess::RTV));

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawUVDisplacement"));
	{
		FIntPoint DisplacementMapResolution(OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY());

		// Update viewport.
		RHICmdList.SetViewport(
			0, 0, 0.f,
			DisplacementMapResolution.X, DisplacementMapResolution.Y, 1.f);

		// Get shaders.
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef<FFirstShaderVS> VertexShader(GlobalShaderMap);
		TShaderMapRef<FFirstShaderPS> PixelShader(GlobalShaderMap);

		// Set the graphic pipeline state.
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

		// Update viewport.
		RHICmdList.SetViewport(
			0, 0, 0.f,
			OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY(), 1.f);

		PixelShader->SetColor(RHICmdList, PixelShader.GetPixelShader(), MyColor);

		// Update shader uniform parameters.
		//VertexShader->SetParameters(RHICmdList, VertexShader.GetVertexShader(), CompiledCameraModel, DisplacementMapResolution);
		//PixelShader->SetParameters(RHICmdList, PixelShader.GetPixelShader(), CompiledCameraModel, DisplacementMapResolution);


		//顶点数据

		TArray<FVector> VertexList;
		VertexList.Add(FVector(-1.0f, 1.0f, 0));
		VertexList.Add(FVector(1.0f, 1.0f, 0));
		VertexList.Add(FVector(-1.0f, -1.0f, 0));
		VertexList.Add(FVector(1.0f, -1.0f, 0));

		int VertexSize = sizeof(FVector) * VertexList.Num();

		FRHIResourceCreateInfo CreateInfo(TEXT("FirstShader"));
		FBufferRHIRef VertexBufferRHI = RHICreateVertexBuffer(VertexSize, BUF_Volatile, CreateInfo);
		void* VoidPtr = RHILockBuffer(VertexBufferRHI, 0, VertexSize, RLM_WriteOnly);
		FPlatformMemory::Memcpy(VoidPtr, VertexList.GetData(), VertexSize);
		RHIUnlockBuffer(VertexBufferRHI);
		RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);

		RHICmdList.DrawPrimitive(0, 2, 1);

		//顶点索引
// 		TArray<uint16, TInlineAllocator<6> > IndexList({ 0, 1, 2, 2, 1, 3 });
// 		int IndexSize = sizeof(uint16) * IndexList.Num();
// 
// 		FBufferRHIRef IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16), IndexSize, BUF_Volatile, CreateInfo);
// 		void* VoidPtr2 = RHILockBuffer(IndexBufferRHI, 0, IndexSize, RLM_WriteOnly);
// 		FPlatformMemory::Memcpy(VoidPtr2, IndexList.GetData(), IndexSize);
// 		RHIUnlockBuffer(IndexBufferRHI);
// 
// 		
// 		RHICmdList.DrawIndexedPrimitive(
// 			IndexBufferRHI,
// 			0, //*BaseVertexIndex
// 			0, //*MinIndex
// 			4, //*NumVertices
// 			0, //*StartIndex
// 			2, //*NumPrimitives
// 			1  //*NumInstances
// 		);
	}

	RHICmdList.EndRenderPass();
	RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::RTV, ERHIAccess::SRVMask));
}

void UShaderTestLibrary::FirstShaderDrawRenderTarget(UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget, FLinearColor MyColor)
{
	check(IsInGameThread());

	if (!OutputRenderTarget || !WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UShaderTestLibrary::FirstShaderDrawRenderTarget, param error"));
		return;
	}

	FTextureRenderTargetResource* TextureRenderTargetResource = OutputRenderTarget->GameThread_GetRenderTargetResource();
	UWorld* World = WorldContextObject->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	FName TextureRenderTargetName = OutputRenderTarget->GetFName();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetResource, FeatureLevel, MyColor, TextureRenderTargetName](FRHICommandListImmediate& RHICmdList)
		{
			DrawUVDisplacementToRenderTarget_RenderThread(RHICmdList, TextureRenderTargetResource, FeatureLevel, TextureRenderTargetName, MyColor);
		}
	);
}
