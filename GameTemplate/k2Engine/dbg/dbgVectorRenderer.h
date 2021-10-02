
#pragma once

#include "graphics/MyRenderer.h"

namespace nsK2Engine {
	namespace nsDbg {
		/// <summary>
		/// ベクトルレンダラー
		/// </summary>
		class VectorRenderer : public IGameObject{
		public:
			/// <summary>
			/// 描画データ。
			/// </summary>
			struct SRenderData {
				std::string name;						//名前
				Vector3 vector;		//ベクトル。
				Vector3 origin;		//基点。
			};
			/// <summary>
			/// 描画するベクトルを追加。
			/// </summary>
			/// <param name="renderData">描画するベクトルの情報。</param>
			void AddVector(SRenderData& renderData)
			{
				if (m_renderData.size() < MAX_VECTOR) {
					m_renderData.emplace_back(renderData);
				}
			}
			/// <summary>
			/// 描画を有効にする。
			/// </summary>
			void EnableRender()
			{
				m_isRender = true;
			}
			/// <summary>
			/// 描画を無効にする。
			/// </summary>
			void DisableRender()
			{
				m_isRender = false;
			}
			/// <summary>
			/// 描画するベクトルの名前リストを設定。
			/// </summary>
			void SetRenderVectorNameList(const std::vector<std::string>& list)
			{
				m_renderVectorNameList = list;
			}
			/// <summary>
			/// Update()が実行される直前に一度だけ呼ばれる処理。
			/// </summary>
			/// <returns></returns>
			bool Start() override;
			/// <summary>
			/// 更新
			/// </summary>
			void Update() override;
			/// <summary>
			/// 描画処理
			/// </summary>
			/// <param name="rc"></param>
			void Render(RenderContext& rc) override;
		private:
			/// <summary>
			/// 描画するベクトルか判定する。
			/// </summary>
			/// <param name="renderData">描画データ</param>
			/// <returns>trueが返ってきたら描画する。</returns>
			bool IsRenderVector(const SRenderData& renderData) const;
		private:
			static const int MAX_VECTOR = 1000;	// 描画可能なベクトルの最大数。
			std::vector<SRenderData> m_renderData;			// 描画データ。
			bool m_isRender = true;								// 描画フラグ
			std::vector<std::string> m_renderVectorNameList;// 描画するベクトルの名前リスト
			ModelRender m_modelRender;						// モデルレンダラー。
		};
	}
}