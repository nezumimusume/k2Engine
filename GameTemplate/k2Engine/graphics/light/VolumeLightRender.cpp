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
        // �{�����[�����C�g��`��
        for (auto& volumeLig : m_volumeSpotLightArray) {
            // 1�p�X�A�O�ʂ�`��B
            rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapFront);
            rc.SetRenderTargetAndViewport(m_volumeLightMapFront);
            rc.ClearRenderTargetView(m_volumeLightMapFront);
            volumeLig->DrawToVolumeLightMapFront(rc);

            // 2�p�X�A�w�ʂ�`��
            rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapBack);
            rc.SetRenderTargetAndViewport(m_volumeLightMapBack);
            rc.ClearRenderTargetView(m_volumeLightMapBack);
            volumeLig->DrawToVolumeLightMapBack(rc);
            
            // ���Ǝ�O�̏������݊����҂�
            rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapBack);
            rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapFront);
            
            // 3�p�X�A�ŏI�`��
            rc.SetRenderTarget(rtvHandle, dsvHandle);
            rc.SetViewportAndScissor(g_graphicsEngine->GetFrameBufferViewport());
            volumeLig->DrawFinal(rc);

            // �Ō��SSR�̂��߂Ƀ{�����[�����C�g�̐[�x�l���������ށB
            // volumeLig->DrawDepth(rc);
            
        }
        EndGPUEvent();
	}
}