#include "k2EnginePreCompile.h"
#include "VolumeLightRender.h"
#include "VolumeSpotLight.h"

namespace nsK2Engine {
    void VolumeLightRender::Init()
    {
        m_volumeLightMapFront.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_drawVolumeLightMapFormat.colorBufferFormat,
            g_drawVolumeLightMapFormat.depthBufferFormat
        );
        m_volumeLightMapBack.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_drawVolumeLightMapFormat.colorBufferFormat,
            g_drawVolumeLightMapFormat.depthBufferFormat
        );
    }
	void VolumeLightRender::Render(RenderContext& rc, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
	{
        BeginGPUEvent("VolumeLightRender");
        // ボリュームライトを描画
        for (auto& volumeLig : m_volumeSpotLightArray) {
            // 1パス、前面を描画。
            rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapFront);
            rc.SetRenderTargetAndViewport(m_volumeLightMapFront);
            rc.ClearRenderTargetView(m_volumeLightMapFront);
            volumeLig->DrawToVolumeLightMapFront(rc);

            // 2パス、背面を描画
            rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapBack);
            rc.SetRenderTargetAndViewport(m_volumeLightMapBack);
            rc.ClearRenderTargetView(m_volumeLightMapBack);
            volumeLig->DrawToVolumeLightMapBack(rc);
            
            // 奥と手前の書き込み完了待ち
            rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapBack);
            rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapFront);
            
            // 3パス、最終描画
            rc.SetRenderTarget(rtvHandle, dsvHandle);
            rc.SetViewportAndScissor(g_graphicsEngine->GetFrameBufferViewport());
            volumeLig->DrawFinal(rc);

            // 最後にSSRのためにボリュームライトの深度値を書き込む。
            // volumeLig->DrawDepth(rc);
            
        }
        EndGPUEvent();
	}
}