#pragma once
#include "LinearMath/btIDebugDraw.h"
#include "graphics/Shader.h"

namespace nsK2EngineLow {
	class DebugWireframe :
		public btIDebugDraw, public Noncopyable
	{
	public:
		DebugWireframe();
		~DebugWireframe();
		/// <summary>
		/// �������B	
		/// </summary>
		void Init();
		/// <summary>
		/// drawLine()���ĂԑO�ɍs�������B
		/// </summary>
		void Begin()
		{
			m_vertexList.clear();
		}
		/// <summary>
		/// drawLine()���Ă񂾌�ɍs�������B
		/// </summary>
		void End(RenderContext& rc);
		/// �K�{�B
		/// <summary>
		/// �����ƂɈ�񂸂Ă΂��B
		/// </summary>
		/// <param name="from">1�ڂ̒��_�̍��W�B</param>
		/// <param name="to">2�ڂ̒��_�̍��W�B</param>
		/// <param name="color">�F�B</param>
		void    drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		void    setDebugMode(int debugMode) override {};
		int     getDebugMode() const override
		{
			return true;
		};

		//�������Ȃ��Ă����Ȃ� {}�@�K�v�ł���Β�`
		void    drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {};
		void    reportErrorWarning(const char* warningString) override {};
		void    draw3dText(const btVector3& location, const char* textString) override {};
	private:
		/// <summary>
		/// ���[�g�V�O�l�`���̏������B
		/// </summary>
		void InitRootSignature();
		/// <summary>
		/// �V�F�[�_�[�̏������B
		/// </summary>
		void InitSharder();
		/// <summary>
		/// �p�C�v���C���X�e�[�g�̏������B
		/// </summary>
		void InitPipelineState();
		/// <summary>
		/// ���_�o�b�t�@�̏������B
		/// </summary>
		void InitVertexBuffer();
		/// <summary>
		/// �C���f�b�N�X�o�b�t�@�̏������B
		/// </summary>
		void InitIndexBuffer();
		/// <summary>
		/// �萔�o�b�t�@�̏������B
		/// </summary>
		void InitConstantBuffer();
		/// <summary>
		/// �f�B�X�N���v�^�q�[�v�̏������B
		/// </summary>
		void InitDescriptorHeap();
		/// <summary>
		/// ���_�o�b�t�@�̍X�V�B
		/// </summary>
		void VertexBufferUpdate(const btVector3& from, const btVector3& to, const btVector3& color);
		/// <summary>
		/// �萔�o�b�t�@�̍X�V�B
		/// </summary>
		void ConstantBufferUpdate();
		/// <summary>
		/// �����_�[�R���e�L�X�g�̍X�V�B
		/// </summary>
		void RenderContextUpdate(RenderContext& rc);
	private:
		struct Vertex
		{
			Vector3 pos;
			Vector3 color;
		};
		std::vector<Vertex>	m_vertexList;				//�`�悷�钸�_�̃��X�g�B
		ConstantBuffer		m_constantBuffer;			//�萔�o�b�t�@�B
		VertexBuffer		m_vertexBuffer;				//���_�o�b�t�@�B
		IndexBuffer			m_indexBuffer;				//�C���f�b�N�X�o�b�t�@�B
		RootSignature		m_rootSignature;			//���[�g�V�O�l�`���B
		Shader				m_Vshader;					//���_�V�F�[�_�[�B
		Shader				m_Pshader;					//�s�N�Z���V�F�[�_�[�B
		PipelineState		m_pipelineState;			//�p�C�v���C���X�e�[�g�B
		DescriptorHeap		m_descriptorHeap;			//�f�B�X�N���v�^�q�[�v�B	
		static const int	MAX_VERTEX = 10000000;		//���_�̍ő吔�B
	};
}

