#pragma once

namespace nsK2EngineLow {
	namespace raytracing {
		/// <summary>
		/// ���C�g���[�V���O�Ŏg�p����f�B�X�N���v�^�q�[�v
		/// </summary>
		class DescriptorHeaps : public Noncopyable {
		public:
			/// <summary>
			/// �e�탊�\�[�X���f�B�X�N���v�^�q�[�v�ɓo�^���Ă����B
			/// </summary>
			/// <param name="world"></param>
			/// <param name="outputBuffer"></param>
			/// <param name="rayGeneCB"></param>
			void Init(
				World& world,
				GPUBuffer& outputBuffer,
				ConstantBuffer& rayGeneCB
			);
			/// <summary>
			/// �V�F�[�_�[���\�[�X�A�A���I�[�_�[�A�N�Z�X�V�F�[�_�[���\�[�X�A�萔�o�b�t�@��
			/// �f�B�X�N���v�^���o�^����Ă���f�B�X�N���v�^�q�[�v���擾�B
			/// </summary>
			/// <returns></returns>
			const DescriptorHeap& GetSrvUavCbvDescriptorHeap() const
			{
				return m_srvUavCbvHeap;
			}
			/// <summary>
			/// �T���v���̃f�B�X�N���v�^���o�^����Ă���f�B�X�N���v�^�q�[�v���擾�B
			/// </summary>
			/// <returns></returns>
			const DescriptorHeap& GetSamplerDescriptorHeap() const
			{
				return m_samplerDescriptorHeap;
			}
		private:
			DescriptorHeap m_srvUavCbvHeap;			//SRV_UAV_CBV�p�̃f�B�X�N���v�^�q�[�v�B
			DescriptorHeap m_samplerDescriptorHeap;	//�T���v���p�̃f�B�X�N���v�^�q�[�v�B
		};
	}
}