#pragma once

#include "HID/GamePad.h"
#include "util/TResourceBank.h"
#include "tkFile/TkmFile.h"
#include "graphics/Shader.h"

namespace nsK2EngineLow {
	class GraphicsEngine;
	class GameTime;
	class Texture;

	class K2EngineLow {
	public:
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
	
		
	private:
		GraphicsEngine* m_graphicsEngine = nullptr;		//�O���t�B�b�N�G���W���B
		TResourceBank<TkmFile> m_tkmFileBank;			//tkm�t�@�C���o���N�B
		TResourceBank<Shader> m_shaderBank;				//�V�F�[�_�[�o���N
		TResourceBank<Texture>	m_textureBank;			//�e�N�X�`���o���N�B
		GamePad m_pad[GamePad::CONNECT_PAD_MAX];		//�Q�[���p�b�h�B
		GameTime m_gameTime;
	};

	extern K2EngineLow* g_engine;	// �჌�x��K2�G���W���B
	extern GameTime* g_gameTime;
}

