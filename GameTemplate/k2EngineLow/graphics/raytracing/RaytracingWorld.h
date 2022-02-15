#pragma once

#include "graphics/DescriptorHeap.h"
#include "RaytracingInstance.h"
#include "TLASBuffer.h"
#include "BLASBuffer.h"



namespace nsK2EngineLow {
	class Model;
	class RenderContext;
	namespace raytracing {

		/// <summary>
		/// レイトレワールド
		/// </summary>
		class World : public Noncopyable
		{
		public:
			/// <summary>
			/// ジオメトリを登録。
			/// </summary>
			/// <param name="model">ジオメトリの元となるモデル</param>
			void RegistGeometry(Model& model);
			/// <summary>
			/// ジオメトリを削除
			/// </summary>
			/// <param name="model">ジオメトリの元となったモデル</param>
			void RemoveGeometry(Model& model);
			/// <summary>
			/// ジオメトリの登録を確定。
			/// </summary>
			void CommitRegistGeometry(RenderContext& rc);
			
			/// <summary>
			/// レイトレワールドを構築。
			/// </summary>
			void Build( RenderContext& rc );

			/// <summary>
			/// レイトレワールドのインスタンスに対してクエリを行う。
			/// </summary>
			/// <param name="bufferNo">バッファの番号</param>
			/// <param name="queryFunc">クエリ関数</param>
			void QueryInstances(int bufferNo, std::function<void(Instance&)> queryFunc) const;
			
			/// <summary>
			/// インスタンスの数を取得。
			/// </summary>
			/// <returns></returns>
			int GetNumInstance() const;
			
			/// <summary>
			/// BLASバッファーを取得。
			/// </summary>
			/// <returns></returns>
			const BLASBuffer& GetBLASBuffer(int bufferNo);
			
			/// <summary>
			/// TLASバッファーを取得。
			/// </summary>
			/// <returns></returns>
			TLASBuffer& GetTLASBuffer(int bufferNo);
			
		private:
			/// <summary>
			/// レイトレのインスタンスを作成。
			/// </summary>
			void CreateRaytracingInstance(Model& model, int bufferNo);
		private:
			/// <summary>
			/// カメラ
			/// </summary>
			struct Camera {
				Matrix mRot;	//回転行列
				Vector3 pos;	//視点。
				float aspect;	//アスペクト比。
				float fFar;		//遠平面。
				float fNear;	//近平面。
			};
			
			Camera m_camera;								// レイトレワールドのカメラ。
			std::vector<InstancePtr> m_instances[2];		// レイトレワールドのインスタンの配列。
			BLASBuffer m_blasBuffer[2];						// BLAS
			TLASBuffer m_topLevelASBuffers[2];				// TLAS
		};
	}//namespace raytracing
}
