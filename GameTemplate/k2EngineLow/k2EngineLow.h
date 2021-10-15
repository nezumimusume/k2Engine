#pragma once

#include "HID/GamePad.h"
#include "util/TResourceBank.h"
#include "tkFile/TkmFile.h"
#include "graphics/Shader.h"
#include "time/FPSLimitter.h"

namespace nsK2EngineLow {
	class GraphicsEngine;
	class GameTime;
	class Texture;

	class K2EngineLow {
	public:
		/// <summary>
		/// �t���[�����[�g���[�h
		/// </summary>
		enum EnFrameRateMode {
			enFrameRateMode_Fix,		// �Œ�t���[�����[�g���[�h�B
			enFrameRateMode_Variable,	// �σt���[�����[�g���[�h�B
		};
		/// <summary>
		/// �t���[�����[�g�Ɋւ�����
		/// </summary>
		struct FrameRateInfo {
			EnFrameRateMode frameRateMode;	// �t���[�����[�g���[�h
			int maxFPS;						// �ő�FPS
		};
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~K2EngineLow();
		/// <summary>
		/// �X�V���������s�B
		/// </summary>
		void ExecuteUpdate();
		/// <summary>
		/// �`�揈�������s�B
		/// </summary>
		void ExecuteRender();
		/// <summary>
		/// �����蔻��`�揈�������s�B
		/// </summary>
		void DebubDrawWorld();
		/// <summary>
		/// �t���[���̊J�n���ɌĂ΂�鏈���B
		/// </summary>
		void BeginFrame();
		/// <summary>
		/// �t���[���̏I�����ɌĂ΂�鏈���B
		/// </summary>
		void EndFrame();

		/// <summary>
		/// �Q�[���G���W���̏������B
		/// </summary>
		void Init(HWND hwnd, UINT frameBufferWidth, UINT frameBufferHeight);
		/// <summary>
		/// tkm�t�@�C�����o���N����擾�B
		/// </summary>
		/// <param name="filePath">�t�@�C���p�X</param>
		/// <returns>tkm�t�@�C���Bnullptr���Ԃ��Ă�����o���N�ɓo�^����Ă��Ȃ��B</returns>
		TkmFile* GetTkmFileFromBank(const char* filePath)
		{
			return m_tkmFileBank.Get(filePath);
		}
		/// <summary>
		/// tkm�t�@�C�����o���N�ɓo�^�B
		/// </summary>
		/// <param name="filePath">tkm�t�@�C���p�X</param>
		/// <param name="tkmFile"></param>
		void RegistTkmFileToBank(const char* filePath, TkmFile* tkmFile)
		{
			m_tkmFileBank.Regist(filePath, tkmFile);
		}
		/// <summary>
		/// �V�F�[�_�[�t�@�C���o���N����V�F�[�_�[���擾�B
		/// </summary>
		/// <param name="filePath"></param>
		/// <returns></returns>
		Shader* GetShaderFromBank(const char* filePath, const char* entryPointFuncName)
		{
			std::string programName = filePath;
			programName += entryPointFuncName;
			return m_shaderBank.Get(programName.c_str());
		}
		/// <summary>
		/// �e�N�X�`�����o���N�ɓo�^�B
		/// </summary>
		/// <param name="filePath">�t�@�C���p�X�B</param>
		/// <param name="texture">�e�N�X�`���B</param>
		void RegistTextureToBank(const char* filePath, Texture* texture)
		{
			m_textureBank.Regist(filePath, texture);
		}
		/// <summary>
		/// �o���N����e�N�X�`�����擾�B
		/// </summary>
		/// <param name="filePath">�t�@�C���p�X�B</param>
		/// <returns>�e�N�X�`���B</returns>
		Texture* GetTextureFromBank(const char* filePath)
		{
			return m_textureBank.Get(filePath);
		}
		/// <summary>
		/// 
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="shader"></param>
		void RegistShaderToBank(const char* filePath, const char* entryPointFuncName, Shader* shader)
		{
			std::string programName = filePath;
			programName += entryPointFuncName;
			m_shaderBank.Regist(programName.c_str(), shader);
		}
		/// <summary>
		/// ���݂̃t���[�����[�g�Ɋւ�������擾
		/// </summary>
		/// <returns></returns>
		const FrameRateInfo& GetFrameRateInfo() const
		{
			return m_frameRateInfo;
		}
		/// <summary>
		/// �t���[�����[�g���[�h��ݒ肷��B
		/// </summary>
		/// <param name="frameRateMode">EnFrameRateMode���Q��</param>
		/// <param name="maxFPS">�ő�FPS</param>
		void SetFrameRateMode(EnFrameRateMode frameRateMode, int maxFPS)
		{
			if (frameRateMode == enFrameRateMode_Fix) {
				// �Œ�t���[�����[�g
				// 1�t���[���̌o�ߎ��Ԃ̒l���Œ�ɂ���B
				m_gameTime.EnableFixedFrameDeltaTime(1.0f / maxFPS);
			}
			else {
				// �σt���[�����[�g
				m_gameTime.DisableFixedFrameDeltaTime();
			}
			m_fpsLimitter.SetMaxFPS(maxFPS);
			// �t���[�����[�g�Ɋւ�������L���B
			m_frameRateInfo.frameRateMode = frameRateMode;
			m_frameRateInfo.maxFPS = maxFPS;
		}
		
	private:
		GraphicsEngine* m_graphicsEngine = nullptr;		// �O���t�B�b�N�G���W���B
		TResourceBank<TkmFile> m_tkmFileBank;			// tkm�t�@�C���o���N�B
		TResourceBank<Shader> m_shaderBank;				// �V�F�[�_�[�o���N
		TResourceBank<Texture>	m_textureBank;			// �e�N�X�`���o���N�B
		GamePad m_pad[GamePad::CONNECT_PAD_MAX];		// �Q�[���p�b�h�B
		GameTime m_gameTime;
		FPSLimitter m_fpsLimitter;						// FPS�ɐ����������鏈���B
		FrameRateInfo m_frameRateInfo = { enFrameRateMode_Variable , 60};
	};

	extern K2EngineLow* g_engine;	// �჌�x��K2�G���W���B
	extern GameTime* g_gameTime;
}

