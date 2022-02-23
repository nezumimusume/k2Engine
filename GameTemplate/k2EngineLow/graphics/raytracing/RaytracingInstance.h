#pragma once


namespace nsK2EngineLow {
	class Material;
	class Model;
	namespace raytracing {
		using ID3D12DescriptorHeapPtr = CComPtr<ID3D12DescriptorHeap>;

		/// <summary>
		/// ���C�g���̃C���X�^���X�f�[�^�B
		/// </summary>
		struct Instance {
			~Instance()
			{
				Release();
			}
			void Release();
			
			D3D12_RAYTRACING_GEOMETRY_DESC geometoryDesc;			// �W�I���g�����B
			RWStructuredBuffer m_vertexBufferRWSB;					// ���_�o�b�t�@�B
			RWStructuredBuffer m_indexBufferRWSB;					// �C���f�b�N�X�o�b�t�@�B
			IndexBuffer* m_originalIndexBuffer = nullptr;			// �I���W�i���̃C���f�b�N�X�o�b�t�@�B
			ConstantBuffer m_worldMatrixCB;							// ���[���h�s��̒萔�o�b�t�@�B
			Material* m_material = nullptr;							// �}�e���A���B		
			AccelerationStructureBuffers m_blasStructuredBuffers;	// BLAS�\�z�p�̃o�b�t�@�B
			Model* m_model = nullptr;								// ���f���B
		};
	}//namespace raytracing
}//namespace nsK2EngineLow 