#include "k2EnginePreCompile.h"
#include "dbg/dbgVectorRenderer.h"

namespace nsK2Engine {
	namespace nsDbg {
		
		bool VectorRenderer::Start()
		{
			m_modelRender.Init(
				"Assets/modelData/preset/vector.tkm",
				nullptr,
				0,
				enModelUpAxisZ,
				true,
				MAX_VECTOR
			);
			return true;
		}
		bool VectorRenderer::IsRenderVector(const SRenderData& renderData) const
		{
			if (m_renderVectorNameList.empty() == true) {
				//描画するベクトルの名前が指定されていない。
				return true;
			}
			//空ではない。
			auto it = std::find(
				m_renderVectorNameList.begin(),
				m_renderVectorNameList.end(),
				renderData.name
			);
			//終端まで行かなかった。
			return it != m_renderVectorNameList.end();
		}
		void VectorRenderer::Update()
		{
			if (m_isRender == false) {
				return;
			}
			// インスタンシングデータを更新。
			for (const auto& renderData : m_renderData) {
				if (IsRenderVector(renderData) ){
					Quaternion rot;
					rot.SetRotation(g_vec3AxisZ, renderData.vector);
					float s = renderData.vector.Length();
					m_modelRender.UpdateInstancingData(
						renderData.origin,
						rot,
						{ s * 0.2f, s * 0.2f, s }
					);
				}
			}
		}
		
		void VectorRenderer::Render(RenderContext& rc)
		{
			if (m_isRender) {
				m_modelRender.Draw(rc);
			}
			// todo フォトの描画処理はいったん無効。
			// 描画データはクリア。
			m_renderData.clear();
			
		}
	}
}


