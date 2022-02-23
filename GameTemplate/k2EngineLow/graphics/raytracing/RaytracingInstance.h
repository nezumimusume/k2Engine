#pragma once


namespace nsK2EngineLow {
	class Material;
	class Model;
	namespace raytracing {
		using ID3D12DescriptorHeapPtr = CComPtr<ID3D12DescriptorHeap>;

		/// <summary>
		/// レイトレのインスタンスデータ。
		/// </summary>
		struct Instance {
			~Instance()
			{
				Release();
			}
			void Release();
			
			D3D12_RAYTRACING_GEOMETRY_DESC geometoryDesc;			// ジオメトリ情報。
			RWStructuredBuffer m_vertexBufferRWSB;					// 頂点バッファ。
			RWStructuredBuffer m_indexBufferRWSB;					// インデックスバッファ。
			IndexBuffer* m_originalIndexBuffer = nullptr;			// オリジナルのインデックスバッファ。
			ConstantBuffer m_worldMatrixCB;							// ワールド行列の定数バッファ。
			Material* m_material = nullptr;							// マテリアル。		
			AccelerationStructureBuffers m_blasStructuredBuffers;	// BLAS構築用のバッファ。
			Model* m_model = nullptr;								// モデル。
		};
	}//namespace raytracing
}//namespace nsK2EngineLow 