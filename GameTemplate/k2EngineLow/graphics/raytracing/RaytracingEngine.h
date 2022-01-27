#pragma once

#include "DescriptorHeap.h"
#include "RaytracingInstance.h"
#include "TLASBuffer.h"
#include "BLASBuffer.h"
#include "RaytracingPSO.h"
#include "RaytracingShaderTable.h"
#include "RaytracingDescriptorHeaps.h"

namespace nsK2EngineLow {
	class Model;
	namespace raytracing {


		class Engine : public Noncopyable
		{
		public:
			
			/// <summary>
			/// 初期化。
			/// </summary>
			void Init();
			/// <summary>
			/// レイトレーシングをディスパッチ。
			/// </summary>
			/// <param name="rc">レンダリングコンテキスト</param>
			void Dispatch(RenderContext& rc);
			/// <summary>
			/// ジオメトリを登録。
			/// </summary>
			/// <param name="model">モデル</param>
			void RegistGeometry(Model& model)
			{
				//レイトレワールドにジオメトリを登録。
				m_world.RegistGeometry(model);
				m_isDirty = true;
			}
			/// <summary>
			/// ジオメトリの登録を確定。
			/// </summary>
			void CommitRegistGeometry(RenderContext& rc);
		private:
			/// <summary>
			/// シェーダーテーブルを作成。
			/// </summary>
			/// <param name="rc"></param>
			void CreateShaderTable(RenderContext& rc);

			/// <summary>
			/// シェーダーリソースを作成。
			/// </summary>
			void CreateShaderResources();

		private:
			/// <summary>
			/// レイトレーシングで使用するカメラ構造体。
			/// この中身を変更したら、Assets/shader/raytracing.fxも変更するように。
			/// </summary>
			struct Camera {
				Matrix mViewProjInv;	// ビュープロジェクション行列の逆行列
				Vector3 pos;			// 視点。
				float aspect;			// アスペクト比。
				float fFar;				// 遠平面。
				float fNear;			// 近平面。
			};
			ConstantBuffer m_rayGenerationCB;			// レイジェネレーションの定数バッファ。
			World m_world;								// レイトレワールド。
			PSO m_pipelineStateObject[2];				// パイプラインステートオブジェクト
			ShaderTable m_shaderTable[2];				// シェーダーテーブル。
			DescriptorHeaps m_descriptorHeaps[2];		// レイトレで使用するディスクリプタヒープの管理者。
			GPUBuffer m_outputResource;					// レイトレースの結果の出力先。
			bool m_isReady = false;						// 準備完了フラグ。
			bool m_isDirty = false;						// ダーティフラグ。
		};
	}//namespace raytracing
}//namespace nsK2EngineLow 

