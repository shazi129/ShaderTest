
#include "ShaderTestLibrary.h"
#include "TextureShader/TestTextureShader.h"

class FMyTextureVertexDeclaration : public FRenderResource
{
public:
	FVertexDeclarationRHIRef VertexDeclarationRHI;

	virtual void InitRHI() override
	{
		FVertexDeclarationElementList Elements;
		uint32 Stride = sizeof(FMyTextureVertex);
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FMyTextureVertex, Position), VET_Float4, 0, Stride));
		Elements.Add(FVertexElement(0, STRUCT_OFFSET(FMyTextureVertex, UV), VET_Float2, 1, Stride));
		VertexDeclarationRHI = RHICreateVertexDeclaration(Elements);
	}

	virtual void ReleaseRHI() override
	{
		VertexDeclarationRHI.SafeRelease();
	}
};

static void DrawTestTextureShaderRenderTarget_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* OutTextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel,
	FName RenderTargetName,
	FTestTextureShaderStructData StructData,
	FTextureReferenceRHIRef TextureReferenceRHI)
{
	check(IsInRenderingThread());

	FRHITexture2D* RenderTargetTexture = OutTextureRenderTargetResource->GetRenderTargetTexture();

	FRHIRenderPassInfo RPInfo(RenderTargetTexture, ERenderTargetActions::DontLoad_Store, OutTextureRenderTargetResource->TextureRHI);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("DrawTestShader"));

	FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
	TShaderMapRef<FTestTextureShaderVS> VertexShader(GlobalShaderMap);
	TShaderMapRef<FTestTextureShaderPS> PixelShader(GlobalShaderMap);

	FMyTextureVertexDeclaration VertexDec;
	VertexDec.InitRHI();

	// Set the graphic pipeline state.
	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.PrimitiveType = PT_TriangleList;
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = VertexDec.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	//设置视口
	FIntPoint DrawTargetResolution(OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY());
	RHICmdList.SetViewport(0, 0, 0.0f, DrawTargetResolution.X, DrawTargetResolution.Y, 1.0f);

	//设置参数
	FSimpleUniformStruct SimpleUniformStruct;
	SimpleUniformStruct.ColorOne = StructData.ColorOne;
	SimpleUniformStruct.ColorTwo = StructData.ColorTwo;
	SimpleUniformStruct.ColorThree = StructData.ColorThree;
	SimpleUniformStruct.ColorFour = StructData.ColorFour;
	SimpleUniformStruct.ColorIndex = StructData.ColorIndex;

	FTestTextureShaderPS::FParameters Parameters;
	Parameters.MyTextrue = TextureReferenceRHI;
	Parameters.MyTextureSampler = TStaticSamplerState<SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp>::GetRHI();
	Parameters.SimpleUniformStruct = TUniformBufferRef<FSimpleUniformStruct>::CreateUniformBufferImmediate(SimpleUniformStruct, UniformBuffer_MultiFrame);
	SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), Parameters);

	//顶点和UI信息
	TArray<FMyTextureVertex> VertexList;
	VertexList.Add(FMyTextureVertex(FVector4(1, 1, 0, 1), FVector2D(1, 1)));
	VertexList.Add(FMyTextureVertex(FVector4(-1, 1, 0, 1), FVector2D(0, 1)));
	VertexList.Add(FMyTextureVertex(FVector4(1, -1, 0, 1), FVector2D(1, 0)));
	VertexList.Add(FMyTextureVertex(FVector4(-1, -1, 0, 1), FVector2D(0, 0)));
	FVertexBufferRHIRef VertexBufferRHI = UTestShaderUtils::CreateVertexBuffer(VertexList);

	const uint16 Indices[] = { 0, 1, 2, 2, 1, 3 };
	FIndexBufferRHIRef IndexBufferRHI = UTestShaderUtils::CreateIndexBuffer(Indices, 6);

	RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);
	RHICmdList.DrawIndexedPrimitive(IndexBufferRHI, 0, 0, 4, 0, 2, 1);

	RHICmdList.EndRenderPass();
}

void UShaderTestLibrary::DrawTestTextureShaderRenderTarget(AActor* ContextActor,  //能获取World的Actor
	UTextureRenderTarget2D* RenderTarget,  //渲染目标
	FTestTextureShaderStructData StructData,          //颜色
	UTexture* Texture                  //纹理
)
{
	check(IsInGameThread());

	if (!RenderTarget || !ContextActor)
	{
		return;
	}

	FTextureRenderTargetResource* TextureRenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FTextureReferenceRHIRef TextureReferenceRHI = Texture->TextureReference.TextureReferenceRHI;

	UWorld* World = ContextActor->GetWorld();
	ERHIFeatureLevel::Type RHIFeatureLevel = World->Scene->GetFeatureLevel();

	FName RenderTargetName = RenderTarget->GetFName();

	ENQUEUE_RENDER_COMMAND(CaptureCommand)(
		[TextureRenderTargetResource, RHIFeatureLevel, StructData, RenderTargetName, TextureReferenceRHI]
		(FRHICommandListImmediate& RHICmdList)
		{
			DrawTestTextureShaderRenderTarget_RenderThread(
				RHICmdList,
				TextureRenderTargetResource,
				RHIFeatureLevel,
				RenderTargetName,
				StructData,
				TextureReferenceRHI);
		}
	);
}

