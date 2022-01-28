#pragma once
#include "RaytracingInstance.h"


namespace nsK2EngineLow {
	class RenderContext;
	namespace raytracing {
		/// <summary>
		/// TLASBuffer
		/// </summary>
		/// <remark>
		/// TLAS( Top level acceleration structures )とは
		/// レイトレワールドに登録されているインスタンスのデータ構造です。
		/// インスタンスの使用するジオメトリ、シェーダーID、ワールド行列などの
		/// データを保持します。
		/// </remark>
		class TLASBuffer : public IShaderResource
		{
		public:
			/// <summary>
			/// TLASを構築。
			/// </summary>
			/// <param name="rc"></param>
			/// <param name="instances"></param>
			void Build(
				RenderContext& rc,
				const std::vector<InstancePtr>& instances,
				bool isUpdate);
			/// <summary>
			/// SRVに登録。
			/// </summary>
			/// <param name="descriptorHandle"></param>
			void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) override;
			/// <summary>
			/// VRAM上の仮想アドレスを取得。
			/// </summary>
			/// <returns></returns>
			D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
			{
				return m_topLevelASBuffers.pResult->GetGPUVirtualAddress();
			}
		private:
			D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS m_inputs = {};
			AccelerationStructureBuffers m_topLevelASBuffers;
		};
	}//namespace raytracing
}