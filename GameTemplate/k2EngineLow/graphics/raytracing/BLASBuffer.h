#pragma once
#include "RaytracingInstance.h"
namespace nsK2EngineLow {
	
	namespace raytracing {
		/// <summary>
		/// BLASBuffer
		/// </summary>
		/// <remark>
		/// BLAS( Bottom level acceleration structures )とは
		/// レイトレワールドに登録されているジオメトリのデータ構造です。
		/// BLASに3Dモデルのポリゴン情報が登録されることになります。
		/// </remark>
		class BLASBuffer : public Noncopyable
		{
		public:

			
			/// <summary>
			/// BLASのビルド。
			/// </summary>
			/// <remark>
			/// BLASの再構築が不要であれば、更新のみを行います。
			/// </remark>
			/// <param name="rc">レンダリングコンテキスト</param>
			/// <param name="instances">インスタンスの配列</param>
			void Build(RenderContext& rc, const std::vector<InstancePtr>& instances, bool isUpdate);
			
		private:

		};
	}//namespace raytracing
}//namespace nsK2EngineLow 