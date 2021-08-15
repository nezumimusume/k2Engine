#pragma once


namespace nsK2EngineLow {
	class Material;
	namespace raytracing {
		using ID3D12DescriptorHeapPtr = CComPtr<ID3D12DescriptorHeap>;

		/// <summary>
		/// ���C�g���̃C���X�^���X�f�[�^�B
		/// </summary>
		struct Instance {
			D3D12_RAYTRACING_GEOMETRY_DESC geometoryDesc;	//�W�I���g�����B
			RWStructuredBuffer m_vertexBufferRWSB;			//���_�o�b�t�@�B
			RWStructuredBuffer m_indexBufferRWSB;			//�C���f�b�N�X�o�b�t�@�B
			Material* m_material = nullptr;					//�}�e���A���B		
		};
	}//namespace raytracing
}//namespace nsK2EngineLow 