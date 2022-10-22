#include "ShaderTestLibrary.h"
#include "RenderGraphUtils.h"
#include "FirstShader/FirstShader.h"
#include "Common/TestShaderUtils.h"

static void ExecuteFirstShader(FRHICommandListImmediate& RHICmdList, ERHIFeatureLevel::Type FeatureLevel, FLinearColor MyColor)
{
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

	FFirstShaderPS::FParameters Parameters;
	Parameters.SimpleColor = MyColor;
	SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), Parameters);

	static const uint32 VERTEX_SIZE = sizeof(FVector4f) * 4;
	FRHIResourceCreateInfo CreateInfo(TEXT("FirstShader"));
	FBufferRHIRef VertexBufferRHI = RHICreateVertexBuffer(VERTEX_SIZE, BUF_Static, CreateInfo);
	FVector4f* VerticesPtr = reinterpret_cast<FVector4f*>(RHILockBuffer(VertexBufferRHI, 0, VERTEX_SIZE, RLM_WriteOnly));

	VerticesPtr[0].Set(-1.0f, 1.0f, 0, 1.0f);
	VerticesPtr[1].Set(1.0f, 1.0f, 0, 1.0f);
	VerticesPtr[2].Set(-1.0f, -1.0f, 0, 1.0f);
	VerticesPtr[3].Set(1.0f, -1.0f, 0, 1.0f);

	RHIUnlockBuffer(VertexBufferRHI);
	RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);

	//顶点索引
	TArray<uint16, TInlineAllocator<6> > IndexList({ 0, 1, 2, 2, 1, 3 });
	int IndexSize = sizeof(uint16) * IndexList.Num();

	FBufferRHIRef IndexBufferRHI = RHICreateIndexBuffer(sizeof(uint16), IndexSize, BUF_Volatile, CreateInfo);
	void* VoidPtr2 = RHILockBuffer(IndexBufferRHI, 0, IndexSize, RLM_WriteOnly);
	FPlatformMemory::Memcpy(VoidPtr2, IndexList.GetData(), IndexSize);
	RHIUnlockBuffer(IndexBufferRHI);

	RHICmdList.DrawIndexedPrimitive(
		IndexBufferRHI,
		0, /*BaseVertexIndex*/
		0, /*MinIndex*/
		4, /*NumVertices*/
		0, /*StartIndex*/
		2, /*NumPrimitives*/
		1  /*NumInstances*/
	);
}

static void FirstShader_RenderThread(
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

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("FirstShader_Pass"));
	{
		FIntPoint DisplacementMapResolution(OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY());

		// Update viewport.
		RHICmdList.SetViewport(0, 0, 0.f, DisplacementMapResolution.X, DisplacementMapResolution.Y, 1.f);

		ExecuteFirstShader(RHICmdList, FeatureLevel, MyColor);
	}

	RHICmdList.EndRenderPass();
}

static void FirstShader_RDG_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	FName TextureRenderTargetName,
	FLinearColor MyColor
)
{
	FRDGBuilder GraphBuilder(RHICmdList);

	TRefCountPtr<IPooledRenderTarget> PooledRenderTarget = CreateRenderTarget(OutTextureRenderTargetResource->GetRenderTargetTexture(), TEXT("First_RDG_RT"));
	FRDGTextureRef RDGRenderTarget = GraphBuilder.RegisterExternalTexture(PooledRenderTarget);

	FFirstShaderPS::FParameters* Parameters = GraphBuilder.AllocParameters<FFirstShaderPS::FParameters>();
	Parameters->SimpleColor = MyColor;
	//Parameters->RenderTargets[0] = FRenderTargetBinding(RDGRenderTarget, ERenderTargetLoadAction::ENoAction);

	// Get shaders.
	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
	TShaderMapRef<FFirstShaderVS> VertexShader(GlobalShaderMap);
	TShaderMapRef<FFirstShaderPS> PixelShader(GlobalShaderMap);

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("RDGDraw"),
		Parameters,
		ERDGPassFlags::Raster,
		[Parameters, VertexShader, PixelShader, GlobalShaderMap](FRHICommandList& RHICmdList) 
		{

		});

	GraphBuilder.QueueTextureExtraction(RDGRenderTarget, &PooledRenderTarget);
	GraphBuilder.Execute();
}

void UShaderTestLibrary::FirstShaderDrawRenderTarget(UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget, FLinearColor MyColor, bool UsingRDG)
{
	check(IsInGameThread());

	if (!OutputRenderTarget || !WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("UShaderTestLibrary::FirstShaderDrawRenderTarget, param error"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("UShaderTestLibrary::FirstShaderDrawRenderTarget, sizeof(FVector4):%d, sizeof(FVector4f):%d"), sizeof(FVector4), sizeof(FVector4f));

	FTextureRenderTargetResource* TextureRenderTargetResource = OutputRenderTarget->GameThread_GetRenderTargetResource();
	UWorld* World = WorldContextObject->GetWorld();
	ERHIFeatureLevel::Type FeatureLevel = World->Scene->GetFeatureLevel();
	FName TextureRenderTargetName = OutputRenderTarget->GetFName();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetResource, FeatureLevel, MyColor, TextureRenderTargetName, UsingRDG](FRHICommandListImmediate& RHICmdList)
		{
			if (UsingRDG)
			{
				FirstShader_RDG_RenderThread(RHICmdList, TextureRenderTargetResource, FeatureLevel, TextureRenderTargetName, MyColor);
			}
			else
			{
				FirstShader_RenderThread(RHICmdList, TextureRenderTargetResource, FeatureLevel, TextureRenderTargetName, MyColor);
			}
		}
	);
}

