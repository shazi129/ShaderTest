#include "ShaderTestLibrary.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "ComputerShader/MyComputeShader.h"

TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;

static void DrawCheckerBoard_RenderThread(
	FRHICommandListImmediate& RHICmdList,
	FTextureRenderTargetResource* TextureRenderTargetResource,
	ERHIFeatureLevel::Type FeatureLevel
)
{
	//Render Thread Assertion
	check(IsInRenderingThread());

	//Unbind the previously bound render targets
	//UnbindRenderTargets(RHICmdList);

	FTexture2DRHIRef RenderTargetTexture = TextureRenderTargetResource->GetRenderTargetTexture();
	
	if (!ComputeShaderOutput.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid"));
		FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(RenderTargetTexture->GetSizeXY(), RenderTargetTexture->GetFormat(), FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_UAV, false));
		ComputeShaderOutputDesc.DebugName = TEXT("WhiteNoiseCS_Output_RenderTarget");
		GRenderTargetPool.FindFreeElement(RHICmdList, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("WhiteNoiseCS_Output_RenderTarget"));
	}

	//Specify the resource transition, we're executing this in post scene rendering so we set it to Graphics to Compute
	RHICmdList.TransitionResource(EResourceTransitionAccess::ERWBarrier, EResourceTransitionPipeline::EGfxToCompute, ComputeShaderOutput->GetRenderTargetItem().UAV);

	

	//Fill the shader parameters structure with tha cached data supplied by the client
	FMyComputeShader::FParameters PassParameters;
	PassParameters.OutputSurface = ComputeShaderOutput->GetRenderTargetItem().UAV;
	PassParameters.TextureSize = RenderTargetTexture->GetSizeXY();

	//Get a reference to our shader type from global shader map
	TShaderMapRef<FMyComputeShader> MyComputeShader(GetGlobalShaderMap(FeatureLevel));

	//Dispatch the compute shader
	FComputeShaderUtils::Dispatch(RHICmdList, MyComputeShader, PassParameters,
		FIntVector(FMath::DivideAndRoundUp(PassParameters.TextureSize.X, (float)32),
					FMath::DivideAndRoundUp(PassParameters.TextureSize.Y, (float)32), 1));

	//Copy shader's output to the render target provided by the client
	RHICmdList.CopyTexture(ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture, RenderTargetTexture, FRHICopyTextureInfo());

}

void UShaderTestLibrary::MyComputerShaderDraw(const UObject* WorldContextObject, UTextureRenderTarget2D* OutputRenderTarget)
{
	check(IsInGameThread());

	if (!OutputRenderTarget)
	{
		return;
	}

	FTextureRenderTargetResource* TextureRenderTargetResource = OutputRenderTarget->GameThread_GetRenderTargetResource();
	ERHIFeatureLevel::Type FeatureLevel = WorldContextObject->GetWorld()->Scene->GetFeatureLevel();

	ENQUEUE_RENDER_COMMAND(CaptureCommand)
		(
			[TextureRenderTargetResource, FeatureLevel](FRHICommandListImmediate& RHICmdList)
			{
				DrawCheckerBoard_RenderThread
				(
					RHICmdList,
					TextureRenderTargetResource,
					FeatureLevel
				);
			}
	);
}