#pragma once


namespace nsK2Engine {
#ifdef K2_DEBUG
	namespace nsDbg {
		class VectorRenderer;
	}
#endif // #ifdef K2_DEBUG
	/// <summary>
	/// K2Engine
	/// </summary>
	/// <remark>
	/// �͌��w�������̃Q�[���G���W���B
	/// k2EngineLow���x�[�X�Ɋg�����ꂽ�G���W���ł��B
	/// ��ɁA�����_�����O�G���W���̋@�\��񋟂��Ă��܂��B
	/// 
	/// ���̃N���X�̓V���O���g���p�^�[���Ő݌v����Ă��܂��B
	/// �V���O���g���p�^�[���͉��L�̓�̋@�\��񋟂���p�^�[���ł��B
	/// 1.�@�O���[�o���ȃA�N�Z�X�|�C���g
	///		->�O���[�o���ϐ��̋@�\
	/// 2�@�C���X�^���X�̐�����ɐ�������@�\�B
	/// �C���X�^���X�̐�����CreateInstance()�A
	/// �C���X�^���X�̔j����DeleteInstance()�A
	/// �C���X�^���X�̎擾��CreateInstance()���Ăяo���Ă��������B
	/// </reramk>
	class K2Engine {
	private:
		/// <summary>
		/// �R���X�g���N�^�B
		/// </summary>
		
		K2Engine(){}
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~K2Engine();
	public:
		/// <summary>
		/// �������f�[�^�B
		/// </summary>
		struct InitData {
			HWND hwnd;					// �E�B���h�E�n���h���B
			UINT frameBufferWidth;		// �t���[���o�b�t�@�̕��B
			UINT frameBufferHeight;		// �t���[���o�b�t�@�̍����B
			bool isSoftShadow;			// �\�t�g�V���h�E���s���H
		};
		/// <summary>
		/// �C���X�^���X�̍쐬�B
		/// </summary>
		static void CreateInstance(const InitData& initData)
		{
			m_instance = new K2Engine;
			m_instance->Init(initData);
		}
		/// <summary>
		/// �C���X�^���X�̔j���B
		/// </summary>
		static void DeleteInstance()
		{
			delete m_instance;
		}
		/// <summary>
		/// �C���X�^���X���擾�B
		/// </summary>
		/// <returns></returns>
		static K2Engine* GetInstance()
		{
			return m_instance;
		}
		/// <summary>
		/// K2Engine��������
		/// </summary>
		void Init(const InitData& initData);
		/// <summary>
		/// �G���W���̏��������s�B
		/// </summary>
		void Execute();
#ifdef K2_DEBUG
		/// <summary>
		/// �x�N�g���̃f�o�b�O�o��
		/// </summary>
		/// <param name="vector">�x�N�g��</param>
		/// <param name="origin">�N�_</param>
		/// <param name="name">���O</param>
		void DrawVector(const Vector3& vector, const Vector3& origin, const char* name = "no name");
		/// <summary>
		/// �x�N�g���̃f�o�b�O�`���L���ɂ���B
		/// </summary>
		void SetDrawVectorEnable();
		/// <summary>
		/// �x�N�g���̃f�o�b�O�`��𖳌��ɂ���B
		/// </summary>
		void SetDrawVectorDisable();
#else
		void DrawVector(const Vector3& vector, const Vector3& origin, const char* name = "no name")
		{

		}
		void SetDrawVectorEnable()
		{

		}
		void SetDrawVectorDisable()
		{

		}
#endif // #ifdef K2_DEBUG
	private:

		K2EngineLow m_k2EngineLow;							// 
		CollisionObjectManager m_collisionObjectManager;	// 
		RenderingEngine m_renderingEngine;					// �����_�����O�G���W���B
#ifdef K2_DEBUG
		nsDbg::VectorRenderer* m_vectorRenderer = nullptr;	// �f�o�b�O�x�N�g���`�揈��
#endif // K2_DEBUG
		static K2Engine* m_instance;
	};

	// �O���[�o���ȃA�N�Z�X�|�C���g�B
	extern K2Engine* g_k2Engine;
	extern RenderingEngine* g_renderingEngine;
	extern SceneLight* g_sceneLight;
	extern CollisionObjectManager* g_collisionObjectManager;

}