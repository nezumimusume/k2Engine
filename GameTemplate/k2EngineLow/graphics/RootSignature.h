#pragma once

namespace nsK2EngineLow {
	class Shader;
	class RootSignature : public Noncopyable {
	public:
		~RootSignature();

		/// <summary>
		/// ������
		/// </summary>
		/// <param name="samplerFilter">�T���v���t�B���^</param>
		/// <param name="textureAdressModeU">�e�N�X�`���̃A�h���b�V���O���[�h(U����)</param>
		/// <param name="textureAdressModeV">�e�N�X�`���̃A�h���b�V���O���[�h(V����)</param>
		/// <param name="textureAdressModeW">�e�N�X�`���̃A�h���b�V���O���[�h(W����)</param>
		/// <param name="maxCbvDescriptor">�萔�o�b�t�@�p�̃f�B�X�N���v�^�̍ő吔</param>
		/// <param name="maxSrvDescriptor">�V�F�[�_�[���\�[�X�p�̃f�B�X�N���v�^�̍ő吔</param>
		/// <param name="maxUavDescritor">UAV�p�̃f�B�X�N���v�^�̍ő吔</param>
		bool Init(
			D3D12_FILTER samplerFilter,
			D3D12_TEXTURE_ADDRESS_MODE textureAdressModeU,
			D3D12_TEXTURE_ADDRESS_MODE textureAdressModeV,
			D3D12_TEXTURE_ADDRESS_MODE textureAdressModeW,
			UINT maxCbvDescriptor = 8,
			UINT maxSrvDescriptor = 32,
			UINT maxUavDescritor = 8
		);
		/// <summary>
		/// ������
		/// </summary>
		/// <param name="samplerDescArray"></param>
		/// <param name="numSampler">�T���v���̐�</param>
		/// <param name="maxCbvDescriptor">�萔�o�b�t�@�p�̃f�B�X�N���v�^�̍ő吔</param>
		/// <param name="maxSrvDescriptor">�V�F�[�_�[���\�[�X�p�̃f�B�X�N���v�^�̍ő吔</param>
		/// <param name="maxUavDescritor">UAV�p�̃f�B�X�N���v�^�̍ő吔</param>
		/// <returns></returns>
		bool Init(
			D3D12_STATIC_SAMPLER_DESC* samplerDescArray,
			int numSampler,
			UINT maxCbvDescriptor = 8,
			UINT maxSrvDescriptor = 32,
			UINT maxUavDescritor = 8,
			UINT offsetInDescriptorsFromTableStartCB = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
			UINT offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
			UINT offsetInDescriptorsFromTableStartUAV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
		);
		bool Init(Shader& shader);
		ID3D12RootSignature* Get()
		{
			return m_rootSignature;
		}
	private:
		/// <summary>
		/// ����B
		/// </summary>
		void Release();
	private:
		ID3D12RootSignature* m_rootSignature = nullptr;	//���[�g�V�O�l�`���B
	};
}