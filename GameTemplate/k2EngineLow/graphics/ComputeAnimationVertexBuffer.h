#pragma once


namespace nsK2EngineLow {
	/// <summary>
	/// �A�j���[�V�����ςݒ��_�o�b�t�@���v�Z���鏈���B
	/// </summary>
	class ComputeAnimationVertexBuffer : Noncopyable{
	public:
		/// <summary>
		/// ������
		/// </summary>
		/// <param name="tkmFilePath">tkm�t�@�C���̃t�@�C���p�X</param>
		/// <param name="enModelUpAxis">���f���̏����</param>
		void Init(const char* tkmFilePath, EnModelUpAxis enModelUpAxis);
		/// <summary>
		/// �f�B�X�p�b�`
		/// </summary>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		void Dispatch(RenderContext& rc);
		/// <summary>
		/// �A�j���[�V�����v�Z�ςݒ��_�o�b�t�@���擾�B
		/// </summary>
		/// <param name="meshNo">���b�V���̔ԍ�</param>
		/// <returns>���_�o�b�t�@</returns>
		const VertexBuffer& GetAnimatedVertexBuffer(int meshNo ) const
		{
			return m_animatedVertexBufferArray[meshNo];
		}
		VertexBuffer& GetAnimatedVertexBuffer(int meshNo) 
		{
			return m_animatedVertexBufferArray[meshNo];
		}		
	private:
		/// <summary>
		/// �f�B�X�N���v�^�q�[�v���������B
		/// </summary>
		void CreateDescriptorHeaps();
	private:
		int m_numMesh = 0;														// ���b�V���̐��B
		DescriptorHeap m_descriptorHeap;										// �f�B�X�N���v�^�q�[�v�B
		Shader m_shader;														// �V�F�[�_�[�B
		TkmFile m_tkmFile;														// tkm�t�@�C���B
		std::unique_ptr< PipelineState[]> m_pipilineStateArray;					// �p�C�v���C���X�e�[�g�̔z��B
		std::unique_ptr<RootSignature[]> m_rootSignatureArray;					// ���[�g�V�O�l�`���̔z��B
		std::unique_ptr<VertexBuffer[]> m_vertexBufferArray;					// ���_�o�b�t�@�̔z��B�z��̃C���f�b�N�X��tkm�t�@�C���̃��b�V���ԍ��ɑΉ����Ă��܂��B
		std::unique_ptr<VertexBuffer[]> m_animatedVertexBufferArray;			// �A�j���[�V�����v�Z�ςݒ��_�o�b�t�@�̔z��B�z��̃C���f�b�N�X��tkm�t�@�C���̃��b�V���ԍ��ɑΉ����Ă��܂��B
		std::unique_ptr<StructuredBuffer[]> m_vertexBufferSBArray;				// ���_�o�b�t�@�̃X�g���N�`���[�h�o�b�t�@�̔z��B
		std::unique_ptr<RWStructuredBuffer[]> m_animatedVertexBufferRWSBArray;	// �A�j���[�V�����v�Z�ςݒ��_�o�b�t�@��RW�X�g���N�`���[�h�o�b�t�@�B�z��̃C���f�b�N�X��tkm�t�@�C���̃��b�V���ԍ��ɑΉ����Ă��܂��B
	};
}