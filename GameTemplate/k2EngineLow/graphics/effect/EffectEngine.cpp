#include "k2EngineLowPreCompile.h"

#include "EffectEngine.h"

namespace nsK2EngineLow {
	EffectEngine* EffectEngine::m_instance = nullptr;	//�B��̃C���X�^���X�B

	EffectEngine::EffectEngine()
	{
		K2_ASSERT(
			m_instance == nullptr,
			"EffectEngine�̃C���X�^���X�𕡐���邱�Ƃ͂ł��܂���B"
		);
		//auto format = DXGI_FORMAT_R8G8B8A8_UNORM;
		auto format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		auto commandQueue = g_graphicsEngine->GetCommandQueue();
		for (int i = 0; i < 2; i++) {
			// �����_���[���쐬�B
			m_renderer[i] = ::EffekseerRendererDX12::Create(
				d3dDevice,
				commandQueue,
				2,
				&format,
				1,
				DXGI_FORMAT_D32_FLOAT,
				false,
				8000
			);
			//�������v�[���̍쐬�B
			m_memoryPool[i] = EffekseerRenderer::CreateSingleFrameMemoryPool(m_renderer[i]->GetGraphicsDevice());
			// �R�}���h���X�g�̍쐬
			m_commandList[i] = EffekseerRenderer::CreateCommandList(m_renderer[i]->GetGraphicsDevice(), m_memoryPool[i]);
		}
		// �G�t�F�N�g�}�l�[�W���[�̍쐬�B
		m_manager = ::Effekseer::Manager::Create(8000);

		
		m_manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
	}
	Effekseer::EffectRef EffectEngine::LoadEffect(const int number)
	{
		Effekseer::EffectRef effect;
		auto it = m_effectMap.find(number);
		if (it != m_effectMap.end()) {
			//���[�h�ς݁B
			effect = it->second;
		}
		else {
			//���[�h�ł��Ȃ��B
			//ResistEffect�ŃG�t�F�N�g�ǂݍ���łˁI
			std::abort();
		}
		return effect;
	}
	EffectEngine::~EffectEngine()
	{

	}
	int EffectEngine::Play(Effekseer::EffectRef effect)
	{
		return m_manager->Play(effect, 0, 0, 0);
	}
	void EffectEngine::Stop(int effectHandle)
	{
		m_manager->StopEffect(effectHandle);
	}


	void EffectEngine::Update(float deltaTime)
	{
		int backBufferNo = g_graphicsEngine->GetBackBufferIndex();
		// Begin a command list
		// �R�}���h���X�g���J�n����B
		EffekseerRendererDX12::BeginCommandList(m_commandList[backBufferNo], g_graphicsEngine->GetCommandList());
		m_renderer[backBufferNo]->SetCommandList(m_commandList[backBufferNo]);

		m_manager->Update();

		//�����_���[�ɃJ�����s���ݒ�B
		m_renderer[backBufferNo]->SetCameraMatrix(*(const Effekseer::Matrix44*)&g_camera3D->GetViewMatrix());
		//�����_���[�Ƀv���W�F�N�V�����s���ݒ�B
		m_renderer[backBufferNo]->SetProjectionMatrix(*(const Effekseer::Matrix44*)&g_camera3D->GetProjectionMatrix());

		m_renderer[backBufferNo]->SetTime(deltaTime);
	}

	void EffectEngine::BeginFrame()
	{
		int backBufferNo = g_graphicsEngine->GetBackBufferIndex();
		m_memoryPool[backBufferNo]->NewFrame();
		// �`�惂�W���[���̐ݒ�B
		m_manager->SetSpriteRenderer(m_renderer[backBufferNo]->CreateSpriteRenderer());
		m_manager->SetRibbonRenderer(m_renderer[backBufferNo]->CreateRibbonRenderer());
		m_manager->SetRingRenderer(m_renderer[backBufferNo]->CreateRingRenderer());
		m_manager->SetTrackRenderer(m_renderer[backBufferNo]->CreateTrackRenderer());
		m_manager->SetModelRenderer(m_renderer[backBufferNo]->CreateModelRenderer());

		// ���[�_�[�̐ݒ�B
		m_manager->SetTextureLoader(m_renderer[backBufferNo]->CreateTextureLoader());
		m_manager->SetModelLoader(m_renderer[backBufferNo]->CreateModelLoader());
		m_manager->SetMaterialLoader(m_renderer[backBufferNo]->CreateMaterialLoader());
	}
	void EffectEngine::Draw()
	{
		int backBufferNo = g_graphicsEngine->GetBackBufferIndex();
		// Begin to rendering effects
		// �G�t�F�N�g�̕`��J�n�������s���B
		m_renderer[backBufferNo]->BeginRendering();

		// Render effects
		// �G�t�F�N�g�̕`����s���B
		m_manager->Draw();

		// Finish to rendering effects
		// �G�t�F�N�g�̕`��I���������s���B
		m_renderer[backBufferNo]->EndRendering();

		// Finish a command list
		// �R�}���h���X�g���I������B
		m_renderer[backBufferNo]->SetCommandList(nullptr);
		EffekseerRendererDX12::EndCommandList(m_commandList[backBufferNo]);
	}

	void EffectEngine::ResistEffect(const int number, const char16_t* filePath)
	{
		Effekseer::EffectRef effect;
		auto it = m_effectMap.find(number);
		if (it == m_effectMap.end()) {
			//�V�K�B
			effect = Effekseer::Effect::Create(m_manager, filePath);
			m_effectMap.insert({ number, effect });
		}
	}
}