#pragma once

namespace nsK2EngineLow {
	namespace raytracing {
		
		/// <summary>
		/// レイトレーシングで使用するディスクリプタヒープ
		/// </summary>
		class DescriptorHeaps : public Noncopyable {
		public:
			/// <summary>
			/// 各種リソースをディスクリプタヒープに登録していく。
			/// </summary>
			/// <param name="world">レイトレワールド</param>
			/// <param name="outputBuffer">レイトレの出力先のバッファ</param>
			/// <param name="rayGeneCB">レイジェネレーションシェーダーの定数バッファ。</param>
			/// <param name="skycubeBox">スカイキューブボックス。</param>
			/// <param name="expandSRV">拡張SRV。</param>
			void Init(
				int bufferNo,
				World& world,
				GPUBuffer& outputBuffer,
				ConstantBuffer& rayGeneCB,
				Texture& skycubeBox,
				StructuredBuffer& expandSRV
			);
			/// <summary>
			/// シェーダーリソース、アンオーダーアクセスシェーダーリソース、定数バッファの
			/// ディスクリプタが登録されているディスクリプタヒープを取得。
			/// </summary>
			/// <returns></returns>
			const DescriptorHeap& GetSrvUavCbvDescriptorHeap() const
			{
				return m_srvUavCbvHeap;
			}
			/// <summary>
			/// サンプラのディスクリプタが登録されているディスクリプタヒープを取得。
			/// </summary>
			/// <returns></returns>
			const DescriptorHeap& GetSamplerDescriptorHeap() const
			{
				return m_samplerDescriptorHeap;
			}
		private:
			DescriptorHeap m_srvUavCbvHeap;			//SRV_UAV_CBV用のディスクリプタヒープ。
			DescriptorHeap m_samplerDescriptorHeap;	//サンプラ用のディスクリプタヒープ。
		};
	}
}