#pragma once

namespace nsK2EngineLow {
	namespace raytracing {
		using ID3D12StateObjectPtr = CComPtr<ID3D12StateObject>;
		using ID3D12RootSignaturePtr = CComPtr<ID3D12RootSignature>;
		class DescriptorHeaps;
		/// <summary>
		/// ���C�g���p�̃p�C�v���C���X�e�[�g�I�u�W�F�N�g�B
		/// </summary>
		class PSO : public Noncopyable {
		private:
			/// <summary>
			/// ���[�g�V�O�l�`����`�B
			/// </summary>
			struct RootSignatureDesc
			{
				D3D12_ROOT_SIGNATURE_DESC desc = {};
				std::vector<D3D12_DESCRIPTOR_RANGE> range;
				std::vector<D3D12_ROOT_PARAMETER> rootParams;
			};
		public:
			/// <summary>
			/// �p�C�v���C���X�e�[�g�I�u�W�F�N�g���������B
			/// </summary>
			void Init(const DescriptorHeaps& descriptorHeaps);
			void QueryInterface(CComPtr< ID3D12StateObjectProperties >& props) const
			{
				m_pipelineState->QueryInterface(&props);
			}
			/// <summary>
			/// �O���[�o�����[�g�V�O�l�`�����擾�B
			/// </summary>
			/// <returns></returns>
			ID3D12RootSignaturePtr GetGlobalRootSignature()
			{
				return m_emptyRootSignature;
			}
			ID3D12StateObjectPtr Get()
			{
				return m_pipelineState;
			}
		private:
			RootSignatureDesc CreateRayGenRootSignatureesc();
			RootSignatureDesc CreatePBRMatterialHitRootSignatureDesc();
		private:
			const DescriptorHeap* m_srvUavCbvHeap = nullptr;		//SRV_UAV_CBV�p�̃f�B�X�N���v�^�q�[�v�B
			ID3D12StateObjectPtr m_pipelineState;					//�p�C�v���C���X�e�[�g
			ID3D12RootSignaturePtr m_emptyRootSignature;
		};
	}
}