#include "k2EngineLowPreCompile.h"
#include "DebugWireframe.h"

namespace nsK2EngineLow {
	DebugWireframe::DebugWireframe()
	{
	}

	DebugWireframe::~DebugWireframe()
	{

	}

	void DebugWireframe::Init()
	{
		if (g_graphicsEngine == nullptr) {
			return;
		}
		//���[�g�V�O�l�`�����������B
		InitRootSignature();
		//�V�F�[�_�[���������B
		InitSharder();
		//�p�C�v���C���X�e�[�g���������B
		InitPipelineState();
		//�ő咷�_�����̃������͐�Ɋm�ۂ��Ă����B
		m_vertexList.reserve(MAX_VERTEX);
		//���_�o�b�t�@�̏������B
		InitVertexBuffer();
		//�C���f�b�N�X�o�b�t�@�̏������B
		InitIndexBuffer();
		//�萔�o�b�t�@���������B
		InitConstantBuffer();
		//�f�B�X�N���v�^�q�[�v���������B
		InitDescriptorHeap();
	}

	void DebugWireframe::InitRootSignature()
	{
		m_rootSignature.Init(
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP
		);
	}

	void DebugWireframe::InitSharder()
	{
		m_Vshader.LoadVS("Assets/shader/debugWireFrame.fx", "VSMain");
		m_Pshader.LoadPS("Assets/shader/debugWireFrame.fx", "PSMain");
	}

	void DebugWireframe::InitPipelineState()
	{
		// ���_���C�A�E�g���`����B
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		//�p�C�v���C���X�e�[�g���쐬�B
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_Vshader.GetCompiledBlob());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_Pshader.GetCompiledBlob());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		psoDesc.NumRenderTargets = 3;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		//�A���x�h�J���[�o�͗p�B
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;
		m_pipelineState.Init(psoDesc);
	}

	void DebugWireframe::InitVertexBuffer()
	{
		//���_�o�b�t�@���쐬�B
		m_vertexBuffer.Init(sizeof(Vertex) * MAX_VERTEX, sizeof(Vertex));
	}

	void DebugWireframe::InitIndexBuffer()
	{
		//�C���f�b�N�X�o�b�t�@�̏������B
		m_indexBuffer.Init(sizeof(std::uint16_t) * MAX_VERTEX, sizeof(std::uint16_t));
		static std::uint16_t indices[MAX_VERTEX];
		for (int i = 0; i < MAX_VERTEX; i++) {
			indices[i] = i;
		}
		m_indexBuffer.Copy(indices, 0, 0, 0);
	}

	void DebugWireframe::InitConstantBuffer()
	{
		m_constantBuffer.Init(sizeof(Matrix));
	}

	void DebugWireframe::InitDescriptorHeap()
	{
		//�f�B�X�N���v�^�q�[�v�Ƀf�B�X�N���v�^��o�^���Ă����B
		m_descriptorHeap.RegistConstantBuffer(0, m_constantBuffer);
		//�f�B�X�N���v�^�q�[�v�ւ̓o�^���m�肳����B
		m_descriptorHeap.Commit();
	}

	void DebugWireframe::VertexBufferUpdate(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		//���_������������B
		Vertex vers[2];
		vers[0].pos = Vector3(from.x(), from.y(), from.z());
		vers[0].color = Vector3(color.x(), color.y(), color.z());
		vers[1].pos = Vector3(to.x(), to.y(), to.z());
		vers[1].color = Vector3(color.x(), color.y(), color.z());

		m_vertexList.push_back(vers[0]);
		m_vertexList.push_back(vers[1]);

		if (m_vertexList.size() > MAX_VERTEX) {
			//�`��ł��郉�C����50���{�܂ŁB���₵�������MAX_VERTEX�𑝂₵�Ă��������B
			std::abort();
		}
	}

	void DebugWireframe::ConstantBufferUpdate()
	{
		//�萔�o�b�t�@�̍X�V(c++)�B
		//mVP�̍X�V(�V�F�[�_�[)�B
		Matrix VP;		//�r���[�s��ƃv���W�F�N�V�����s��
		//�r���[�ƃv���W�F�N�V�����̊|���Z
		Matrix v = g_camera3D->GetViewMatrix();
		Matrix p = g_camera3D->GetProjectionMatrix();
		VP.Multiply(v, p);
		//�萔�o�b�t�@�ɓn�������ϐ����i�[(m_constantBuffer�̓��e���㏑��)
		m_constantBuffer.CopyToVRAM(&VP);
	}

	void DebugWireframe::RenderContextUpdate(RenderContext& rc)
	{
		//���[�g�V�O�l�`����ݒ�B
		rc.SetRootSignature(m_rootSignature);
		//���_���ǂ�Ȋ����ɕ`�悷��̂��B
		//�p�C�v���C���X�e�[�g�̐ݒ�B
		rc.SetPipelineState(m_pipelineState);
		//����͒��_��̊Ԃɐ���`���ݒ�B
		rc.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		//���_�o�b�t�@��ݒ�B
		rc.SetVertexBuffer(m_vertexBuffer);
		//�C���f�b�N�X�o�b�t�@��ݒ�B
		rc.SetIndexBuffer(m_indexBuffer);
		//�f�B�X�N���v�^�q�[�v�ɐݒ�B
		rc.SetDescriptorHeap(m_descriptorHeap);
		//�h���[�R�[���B
		rc.DrawIndexed(static_cast<UINT>(m_vertexList.size()));
	}

	//1�t���[������drawLine�͐��̐������s��
	void DebugWireframe::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		//���_�o�b�t�@�̍X�V�B
		VertexBufferUpdate(from, to, color);
	}

	void DebugWireframe::End(RenderContext& rc)
	{
		if (m_vertexList.size() == 0)
		{
			return;
		}
		//���_���R�s�[�B
		m_vertexBuffer.Copy(&m_vertexList.front());

		ConstantBufferUpdate();

		RenderContextUpdate(rc);
	}
}