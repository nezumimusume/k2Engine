#include "k2EnginePreCompile.h"
#include "VolumeLightMap.h"
#include "graphics/VolumeSpotLight.h"

namespace nsK2Engine {
    void VolumeLightMap::Init()
    {
        m_volumeLightMapFront.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_drawVolumeLightMapFrontFormat.colorBufferFormat,
            g_drawVolumeLightMapFrontFormat.depthBufferFormat
        );
        m_volumeLightMapBack.Create(
            FRAME_BUFFER_W,
            FRAME_BUFFER_H,
            1,
            1,
            g_drawVolumeLightMapBackFormat.colorBufferFormat,
            g_drawVolumeLightMapBackFormat.depthBufferFormat
        );
    }
	void VolumeLightMap::Render(RenderContext& rc)
	{
        // ボリュームライトの背面を描画
        rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapBack);
        rc.SetRenderTargetAndViewport(m_volumeLightMapBack);
        rc.SetRenderTarget(m_volumeLightMapBack);
        rc.ClearRenderTargetView(m_volumeLightMapBack);
        for (auto& volumeLig : m_volumeSpotLightArray) {
            volumeLig->DrawToVolumeLightMapBack(rc);
        }
        // ボリュームライトの前面を描画。
        rc.WaitUntilToPossibleSetRenderTarget(m_volumeLightMapFront);
        rc.SetRenderTarget(m_volumeLightMapFront);
        rc.ClearRenderTargetView(m_volumeLightMapFront);
        for (auto& volumeLig : m_volumeSpotLightArray) {
            volumeLig->DrawToVolumeLightMapFront(rc);
        }

        // 奥と手前の書き込み完了待ち
        rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapBack);
        rc.WaitUntilFinishDrawingToRenderTarget(m_volumeLightMapFront);

	}
}