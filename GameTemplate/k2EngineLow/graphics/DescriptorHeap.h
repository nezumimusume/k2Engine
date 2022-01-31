#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �f�B�X�N���v�^�q�[�v�B
	/// </summary>
	class DescriptorHeap : public Noncopyable {
	public:
		/// <summary>
		/// �R���X�g���N�^
		/// </summary>
		DescriptorHeap();
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~DescriptorHeap();
		/// <summary>
		/// ������
		/// </summary>
		/// <param name="maxSRV">�f�B�X�N���v�^�q�[�v�ɓo�^�ł���SRV�̍ő吔</param>
		/// <param name="maxUAV">�f�B�X�N���v�^�q�[�v�ɓo�^�ł���UAV�̍ő吔</param>
		/// <param name="maxConstantBuffer">�f�B�X�N���v�^�q�[�v�ɓo�^�ł���萔�o�b�t�@�̍ő吔</param>
		/// <param name="maxSamplerState">�f�B�X�N���v�^�q�[�v�ɓo�^�ł���T���v���X�e�[�g�̍ő吔</param>
		/// <param name=""></param>
		void Init(
			int maxSRV, 
			int maxUAV, 
			int maxConstantBuffer, 
			int maxSamplerState
		);
		/// <summary>
		/// 
		/// </summary>
		/// <returns></returns>
		ID3D12DescriptorHeap* Get() const;
		/// <summary>
		/// �V�F�[�_�[���\�[�X���L�����邽�߂̗̈�����T�C�Y
		/// </summary>
		/// <remark>
		/// �f�t�H���g�ő���Ȃ��Ȃ��� or �œK�ȃT�C�Y�ɂ������ꍇ�ɗ��p���Ă��������B
		/// </remark>
		void ResizeShaderResource(int numSRV)
		{
			m_shaderResources.resize(numSRV);
		}
		/// <summary>
		/// �萔�o�b�t�@���L�����邽�߂̗̈���m��
		/// </summary>
		/// <remark>
		/// �f�t�H���g�ő���Ȃ��Ȃ��� or �œK�ȃT�C�Y�ɂ������ꍇ�ɗ��p���Ă��������B
		/// </remark>
		/// <param name="numCB"></param>
		void ResizeConstantBuffer(int numCB)
		{
			m_constantBuffers.resize(numCB);
		}
		/// <summary>
		/// UAV���L�����邽�߂̗̈�����T�C�Y
		/// </summary>
		/// <remark>
		/// �f�t�H���g�ő���Ȃ��Ȃ��� or �œK�ȃT�C�Y�ɂ������ꍇ�ɗ��p���Ă��������B
		/// </remark>
		void ResizeUnorderAccessResource(int numUAV)
		{
			m_uavResoruces.resize(numUAV);
		}
		/// <summary>
		/// �V�F�[�_�[���\�[�X���f�B�X�N���v�^�q�[�v�ɓo�^�B
		/// </summary>
		/// <param name="registerNo">
		/// ���W�X�^�ԍ��B-1���w�肳�ꂽ��A���ݓo�^����Ă��郊�\�[�X���̎��̃��W�X�^���g�p�����B
		/// </param>
		/// <param name="sr">�V�F�[�_�[���\�[�X</param>
		void RegistShaderResource(int registerNo, IShaderResource& sr)
		{
			RegistResource(
				registerNo,
				&sr,
				&m_shaderResources.front(),
				m_numSRV,
				static_cast<int>(m_shaderResources.size()),
				L"DescriptorHeap::RegistShaderResource() ���W�X�^�ԍ����͈͊O�ł��B"
			);
		}

		/// <summary>
		/// �A���I�[�_�[�A�N�Z�X���\�[�X��o�^�B
		/// </summary>
		/// <param name="registerNo">
		/// ���W�X�^�ԍ��B-1���w�肳�ꂽ��A���ݓo�^����Ă��郊�\�[�X���̎��̃��W�X�^���g�p�����B
		/// </param>
		/// <param name="sr">�A���I�[�_�[���\�[�X</param>
		void RegistUnorderAccessResource(int registerNo, IUnorderAccessResrouce& sr)
		{
			RegistResource(
				registerNo,
				&sr,
				&m_uavResoruces.front(),
				m_numUAV,
				static_cast<int>(m_uavResoruces.size()),
				L"DescriptorHeap::RegistUnorderAccessResource() ���W�X�^�ԍ����͈͊O�ł��B"
			);
		}
		/// <summary>
		/// �萔�o�b�t�@���f�B�X�N���v�^�q�[�v�ɓo�^�B
		/// </summary>
		/// <param name="registerNo">
		/// ���W�X�^�ԍ��B-1���w�肳�ꂽ��A���ݓo�^����Ă��郊�\�[�X���̎��̃��W�X�^���g�p�����B
		/// </param>
		/// <param name="cb">�萔�o�b�t�@</param>
		void RegistConstantBuffer(int registerNo, ConstantBuffer& cb)
		{
			RegistResource(
				registerNo,
				&cb,
				&m_constantBuffers.front(),
				m_numConstantBuffer,
				static_cast<int>(m_constantBuffers.size()),
				L"DescriptorHeap::RegistConstantBuffer() ���W�X�^�ԍ����͈͊O�ł��B"
			);
		}
		/// <summary>
		/// �T���v����`���f�B�X�N���v�^�q�[�v�ɒǉ��B
		/// </summary>
		/// <param name="registerNo">
		/// ���W�X�^�ԍ��B-1���w�肳�ꂽ��A���ݓo�^����Ă��郊�\�[�X���̎��̃��W�X�^���g�p�����B
		/// </param>
		/// <param name="desc">
		/// �T���v����`
		/// </param>
		void RegistSamplerDesc(int registerNo, const D3D12_SAMPLER_DESC& desc)
		{
			RegistResource(
				registerNo,
				desc,
				&m_samplerDescs.front(),
				m_numSamplerDesc,
				static_cast<int>(m_samplerDescs.size()),
				L"DescriptorHeap::RegistSamplerDesc() ���W�X�^�ԍ����͈͊O�ł��B"
			);
		}
		/// <summary>
		/// �f�B�X�N���v�^�q�[�v�ւ̓o�^���m��B
		/// </summary>
		void Commit(bool isDoubleBuffer = true);
		/// <summary>
		/// �T���v���X�e�[�g�p�̃f�B�X�N���v�^�q�[�v�ւ̓o�^�B
		/// </summary>
		void CommitSamperHeap(bool isDoubleBuffer = true);
		/// <summary>
		/// �o�b�N�o�b�t�@�̔ԍ����擾�B
		/// </summary>
		/// <returns></returns>
		int GetBackBufferNo() const;
		/// <summary>
		/// �萔�o�b�t�@�̃f�B�X�N���v�^�̊J�n�n���h�����擾�B
		/// </summary>
		/// <returns></returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetConstantBufferGpuDescritorStartHandle() const;
		/// <summary>
		/// �V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�̊J�n�n���h�����擾�B
		/// </summary>
		/// <returns></returns>
		D3D12_GPU_DESCRIPTOR_HANDLE GetShaderResourceGpuDescritorStartHandle() const;
		/// <summary>
		/// Unorder Access ���\�[�X�̃f�B�X�N���v�^�̊J�n�n���h�����擾�B
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetUavResourceGpuDescritorStartHandle() const;
		/// <summary>
		/// Sampler�̃f�B�X�N���v�^�̊J�n�n���h�����擾�B
		/// </summary>
		D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerResourceGpuDescritorStartHandle() const;
		/// <summary>
		/// �V�F�[�_�[���\�[�X����ł��o�^����Ă��邩����B
		/// </summary>
		/// <returns></returns>
		bool IsRegistShaderResource() const
		{
			return m_numSRV != 0;
		}
		/// <summary>
		/// �萔�o�b�t�@����ł��o�^����Ă��邩����B
		/// </summary>
		/// <returns></returns>
		bool IsRegistConstantBuffer() const
		{
			return m_numConstantBuffer != 0;
		}
		/// <summary>
		/// UAV���\�[�X����ł��o�^����Ă��邩����B
		/// </summary>
		/// <returns></returns>
		bool IsRegistUavResource() const
		{
			return m_numUAV != 0;
		}
		/// <summary>
		/// UAV�f�B�X�N���v�^���n�܂�z��ԍ����擾����B
		/// </summary>
		/// <remarks>
		/// UAV�f�B�X�N���v�^��SRV�f�B�X�N���v�^�̎�����o�^����Ă���B
		/// �Ȃ̂ŁASRV�f�B�X�N���v�^��10�o�^����Ă���΁A
		/// UAV�f�B�X�N���v�^�͔z���10�Ԗڂ���o�^����Ă��邱�ƂɂȂ�B
		/// ���̊֐��͌��݃��C�g���G���W���Ŏg�p����Ă���B
		/// </remarks>
		/// <returns></returns>
		int GetOffsetUAVDescriptorFromTableStart() const
		{
			return m_numSRV + m_numConstantBuffer;
		}
		/// <summary>
		/// SRV�f�B�X�N���v�^���n�܂�z��ԍ����擾����B
		/// </summary>
		/// <returns></returns>
		int GetOffsetSRVDescriptorFromTableStart() const
		{
			return m_numConstantBuffer;
		}
		/// <summary>
		/// �萔�o�b�t�@�f�B�X�N���v�^���n�܂�z��ԍ����擾����B
		/// </summary>
		/// /// <remarks>
		/// �萔�o�b�t�@�f�B�X�N���v�^��SRV�f�B�X�N���v�^��UAV�f�B�X�N���v�^�̎�����o�^����Ă���B
		/// �Ȃ̂ŁASRV�f�B�X�N���v�^��10�AUVA�f�B�X�N���v�^��5�o�^����Ă���΁A
		/// �萔�o�b�t�@�f�B�X�N���v�^�͔z���15�Ԗڂ���o�^����Ă��邱�ƂɂȂ�B
		/// ���̊֐��͌��݃��C�g���G���W���Ŏg�p����Ă���B
		/// <returns></returns>
		int GetOffsetConstantBufferDescriptorFromTableStart() const
		{
			return m_numSRV + m_numUAV;
		}
	private:
		/// <summary>
		/// ���
		/// </summary>
		void Release();
		/// <summary>
		/// ���\�[�X���f�B�X�N���v�^�q�[�v�ɓo�^�B
		/// </summary>
		/// <param name="registerNo">�o�^�ԍ�</param>
		/// <param name="res">�o�^���郊�\�[�X</param>
		/// <param name="resTbl">���\�[�X�e�[�u���B���̃e�[�u���Ƀ��\�[�X���ǉ�����܂��B</param>
		/// <param name="numRes">�o�^����Ă��郊�\�[�X�̐��B�{�֐����Ăяo���ƁA���̐����P�C���N�������g����܂��B</param>
		template<class T>
		void RegistResource(
			int registerNo,
			T res,
			T resTbl[],
			int& numRes,
			const int MAX_RESOURCE,
			const wchar_t* errorMessage
		)
		{
			if (registerNo == -1) {
				//-1���w�肳��Ă�����A���ݓo�^����Ă��閖���̃��\�[�X�̎��ɓo�^�����B
				registerNo = numRes;
			}
			if (registerNo < MAX_RESOURCE) {
				resTbl[registerNo] = res;
				if (numRes < registerNo + 1) {
					numRes = registerNo + 1;
				}
			}
			else {
				MessageBox(nullptr, errorMessage, L"�G���[", MB_OK);
				std::abort();
			}
		}
	private:
		enum {
			DEFAULT_MAX_SRV = 128,				// �V�F�[�_�[���\�[�X�̍ő吔�̃f�t�H���g�l�B
			DEFAULT_MAX_UAV = 128,				// UAV�̍ő吔�̃f�t�H���g�l
			DEFAULT_MAX_CONSTANT_BUFFER = 32,	// �萔�o�b�t�@�̍ő吔�̃f�t�H���g�l�B
			DEFAULT_MAX_SAMPLER_STATE = 16,		// �T���v���X�e�[�g�̍ő吔�̃f�t�H���g�l�B
		};
		int m_maxSRV = DEFAULT_MAX_SRV;							// SRV�̍ő吔�B
		int m_maxUAV = DEFAULT_MAX_UAV;							// UAV�̍ő吔�B
		int m_maxConstantBuffer = DEFAULT_MAX_CONSTANT_BUFFER;	// �萔�o�b�t�@�̍ő吔
		int m_maxSamplerState = DEFAULT_MAX_SAMPLER_STATE;		// �T���v���X�e�[�g�̍ő吔�B
		int m_numSRV = 0;										// �V�F�[�_�[���\�[�X�̐��B
		int m_numConstantBuffer = 0;							// �萔�o�b�t�@�̐��B
		int m_numUAV = 0;										// �A���I�[�_�[�A�N�Z�X���\�[�X�̐��B
		int m_numSamplerDesc = 0;								// �T���v���̐��B
		bool m_isDoubleBuffer = true;							// �_�u���o�b�t�@�H
		ID3D12DescriptorHeap* m_descriptorHeap = { nullptr };	//�f�B�X�N���v�^�q�[�v�B
		std::vector<IShaderResource*> m_shaderResources;		//�V�F�[�_�[���\�[�X�B
		std::vector < IUnorderAccessResrouce*> m_uavResoruces;	//UAV���\�[�X�B
		std::vector < ConstantBuffer*> m_constantBuffers;		//�萔�o�b�t�@�B
		std::vector< D3D12_SAMPLER_DESC > m_samplerDescs;		//�T���v���X�e�[�g�B
		D3D12_GPU_DESCRIPTOR_HANDLE m_cbGpuDescriptorStart[2];						//�萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h���B
		D3D12_GPU_DESCRIPTOR_HANDLE m_srGpuDescriptorStart[2];						//�V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h���B
		D3D12_GPU_DESCRIPTOR_HANDLE m_uavGpuDescriptorStart[2];						//UAV���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h���B
		D3D12_GPU_DESCRIPTOR_HANDLE m_samplerGpuDescriptorStart[2];					//Sampler�̂ŃX�N���v�^�q�[�v�̊J�n�n���h���B
	};
}