#pragma once

namespace nsK2EngineLow {
	class Shader : public Noncopyable {
	public:
		/// <summary>
		/// �s�N�Z���V�F�[�_�[�����[�h�B
		/// </summary>
		/// <param name="filePath">�t�@�C���p�X�B</param>
		/// <param name="entryFuncName">�G���g���[�|�C���g�̊֐����B</param>
		void LoadPS(const char* filePath, const char* entryFuncName);
		/// <summary>
		/// ���_�V�F�[�_�[�����[�h�B
		/// </summary>
		/// <param name="filePath">�t�@�C���p�X</param>
		/// <param name="entryFuncName">�G���g���[�|�C���g�̊֐����B</param>
		void LoadVS(const char* filePath, const char* entryFuncName);
		/// <summary>
		/// �R���s���[�g�V�F�[�_�[�����[�h�B
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="entryFuncName"></param>
		void LoadCS(const char* filePath, const char* entryFuncName);
		/// <summary>
		/// ���C�g���[�V���O�p�̃V�F�[�_�[�����[�h�B
		/// </summary>
		void LoadRaytracing(const wchar_t* filePath);
		/// <summary>
		/// �R���p�C���ς݃V�F�[�_�[�f�[�^���擾�B
		/// </summary>
		/// <returns></returns>
		ID3DBlob* GetCompiledBlob() const
		{
			return m_blob;
		}
		IDxcBlob* GetCompiledDxcBlob() const
		{
			return m_dxcBlob;
		}
		/// <summary>
		/// �������ς݁H
		/// </summary>
		/// <returns></returns>
		bool IsInited() const
		{
			return m_isInited;
		}

		~Shader();
		
	private:
		/// <summary>
		/// �V�F�[�_�[�����[�h�B
		/// </summary>
		/// <param name="filePath">�t�@�C���p�X</param>
		/// <param name="entryFuncName">�G���g���[�|�C���g�̊֐����B</param>
		/// <param name="shaderModel">�V�F�[�_�[���f��</param>
		void Load(const char* filePath, const char* entryFuncName, const char* shaderModel);
		/// <summary>
		/// ���
		/// </summary>
		void Release();
	private:
		ID3DBlob* m_blob = nullptr;	//�R���p�C���ς݂̃V�F�[�_�[�f�[�^�B
		IDxcBlob* m_dxcBlob = nullptr;	//DXC�R���p�C�����g�p�����Ƃ��̃V�F�[�_�[�f�[�^�B
		bool m_isInited = false;		//�������ς݁H
	};
}
