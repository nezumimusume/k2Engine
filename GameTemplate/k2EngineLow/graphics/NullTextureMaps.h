#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// Null�e�N�X�`���}�b�v��ێ����Ă���N���X�B
	/// </summary>
	/// <remarks>
	/// ���̃N���X���ێ����Ă���̂́A
	/// �X�y�L�����}�b�v�A�@���}�b�v�A���˗��}�b�v�Ȃǂ�
	/// �}�e���A���Ŏg�p����e�N�X�`���}�b�v�ł��B
	/// ���̃N���X�̃}�b�v�̓}�e���A���ɁA��L�̃}�b�v��
	/// �\��t�����Ă��Ȃ��ꍇ�ł��B
	/// �ł��̂ŁA���̃N���X���ێ����Ă���}�b�v�́A�X�y�L�����}�b�v�ł����
	/// ���ʔ��˗��O�A�@���}�b�v�ł���Γʉ��Ȃ��A���t���N�V�����}�b�v�ł���Δ��˗��O�ƂȂ�܂��B
	/// �}�e���A���Ƀ}�b�v���\��t�����Ă��Ȃ��ꍇ�ɁA���̃N���X�̃}�b�v�𗘗p���邱�ƂŁA
	/// �V�F�[�_�[�ł̓��I��������炷���Ƃ��ł��A�V�F�[�_�\�v���O�������V���v���ɂ��邱�Ƃ��ł���悤�ɂȂ��Ă��܂��B
	/// ���̃N���X�Ń��[�h����Ă���e�N�X�`���́AAssets/modelData/preset�̉��ɗp�ӂ���Ă��܂��B
	/// </remarks>
	class NullTextureMaps : public Noncopyable {
	public:
		/// <summary>
		/// �������B
		/// </summary>
		void Init();
		/// <summary>
		/// �@���}�b�v���擾�B
		/// </summary>
		/// <returns></returns>
		const std::unique_ptr<char[]>& GetNormalMap() const
		{
			return m_normalMap;
		}
		/// <summary>
		/// �@���}�b�v�̃T�C�Y���擾�B
		/// </summary>
		/// <returns></returns>
		unsigned int GetNormalMapSize() const
		{
			return m_normalMapSize;
		}
		const char* GetNormalMapFilePath() const
		{
			return m_normalMapFilePath;
		}
		/// <summary>
		/// �X�y�L�����}�b�v���擾�B
		/// </summary>
		/// <returns></returns>
		const std::unique_ptr<char[]>& GetSpecularMap() const
		{
			return m_specMap;
		}
		/// <summary>
		/// �X�y�L�����}�b�v�̃T�C�Y���擾�B
		/// </summary>
		/// <returns></returns>
		unsigned int GetSpecularMapSize() const
		{
			return m_specMapSize;
		}
		const char* GetSpecularMapFilePath() const
		{
			return m_specMapFilePath;
		}
		/// <summary> 
		/// �A���x�h�}�b�v���擾�B
		/// </summary>
		/// <returns></returns>
		const std::unique_ptr<char[]>& GetAlbedoMap() const
		{
			return m_albedoMap;
		}
		/// <summary>
		/// �A���x�h�}�b�v�̃T�C�Y���擾�B
		/// </summary>
		/// <returns></returns>
		unsigned int GetAlbedoMapSize() const
		{
			return m_albedoMapSize;
		}
		const char* GetAlbedoMapFilePath() const
		{
			return m_albedoMapFilePath;
		}
		/// <summary>
		/// ���t���N�V�����}�b�v���擾�B
		/// </summary>
		/// <returns></returns>
		const std::unique_ptr<char[]>& GetReflectionMap() const
		{
			return m_zeroValueMap;
		}
		/// <summary>
		/// ���t���N�V�����}�b�v�̃T�C�Y���擾�B
		/// </summary>
		/// <returns></returns>
		unsigned int GetReflectionMapSize() const
		{
			return m_zeroValueMapSize;
		}
		const char* GetReflectionMapFilePath() const
		{
			return m_zeroValueMapFilePath;
		}
		/// <summary>
		/// ���܃}�b�v���擾�B
		/// </summary>
		/// <returns></returns>
		const std::unique_ptr<char[]>& GetRefractionMap() const
		{
			return m_zeroValueMap;
		}
		/// <summary>
		/// ���܃}�b�v�̃T�C�Y���擾�B
		/// </summary>
		/// <returns></returns>
		unsigned int GetRefractionMapSize() const
		{
			return m_zeroValueMapSize;
		}
		const char* GetRefractionMapFilePath() const
		{
			return m_zeroValueMapFilePath;
		}
	private:
		std::unique_ptr<char[]> m_albedoMap;	//�A���x�h�}�b�v�B
		unsigned int m_albedoMapSize = 0;		//�A���x�h�}�b�v�̃T�C�Y�B
		std::unique_ptr<char[]>	m_normalMap;	//�@���}�b�v�B
		unsigned int m_normalMapSize = 0;		//�@���}�b�v�̃T�C�Y�B
		std::unique_ptr<char[]>	m_specMap;		//�@���}�b�v�B
		unsigned int m_specMapSize = 0;			//�@���}�b�v�̃T�C�Y�B
		std::unique_ptr<char[]> m_zeroValueMap;	//0�̒l���i�[���Ă���}�b�v�B
		unsigned int m_zeroValueMapSize = 0;	//0�̒l���i�[���Ă���}�b�v�̃T�C�Y�B
		const char* m_albedoMapFilePath;
		const char* m_normalMapFilePath;
		const char* m_specMapFilePath;
		const char* m_zeroValueMapFilePath;
	};
}