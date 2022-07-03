#pragma once


namespace nsK2EngineLow {
	/// <summary>
	/// �A�j���[�V�����ςݒ��_�o�b�t�@���v�Z���鏈���B
	/// </summary>
	class ComputeAnimationVertexBuffer : Noncopyable {
	public:
		/// <summary>
		/// ������
		/// </summary>
		/// <param name="tkmFilePath">tkm�t�@�C���̃t�@�C���p�X</param>
		/// <param name="numBone">�{�[���̐�</param>
		/// <param name="boneMatrixTopAddress">�{�[���s��̔z��</param>
		/// <param name="enModelUpAxis">���f���̏����</param>
		/// <param name="numInstance">�C���X�^���X�̐�</param>
		/// <param name="worldMatrixArraySB">
		/// ���[���h�s��̔z��̃X�g���N�`���[�h�o�b�t�@�B
		/// �C���X�^���V���O�f�B�X�p�b�`���s���ۂ͎w�肵�Ă��������B
		/// </param>
		void Init(
			const char* tkmFilePath,
			int numBone,
			void* boneMatrixArray,
			EnModelUpAxis enModelUpAxis,
			int numInstance,
			StructuredBuffer* worldMatrixArraySB
		);
		
		/// <summary>
		/// �f�B�X�p�b�`
		/// </summary>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		/// <param name="worldMatrix">���[���h�s��</param>
		/// <param name="instanceNo">�C���X�^���X�ԍ�</param>
		void Dispatch(
			RenderContext& rc,
			const Matrix& worldMatrix,
			int instanceNo
		);
		
		/// <summary>
		/// �A�j���[�V�����v�Z�ςݒ��_�o�b�t�@���擾�B
		/// </summary>
		/// <param name="meshNo">���b�V���̔ԍ�</param>
		/// <returns>���_�o�b�t�@</returns>
		const VertexBuffer& GetAnimatedVertexBuffer(int meshNo) const
		{
			return m_meshArray[meshNo].m_animatedVertexBuffer;
		}
		VertexBuffer& GetAnimatedVertexBuffer(int meshNo)
		{
			return m_meshArray[meshNo].m_animatedVertexBuffer;
		}
		/// <summary>
		/// ���O�v�Z�ς݃C���f�b�N�X�o�b�t�@���擾�B
		/// </summary>
		/// <param name="meshNo">���b�V���ԍ��B</param>
		/// <param name="matNo">�}�e���A���ԍ�</param>
		/// <returns>���O�v�Z�ς݃C���f�b�N�X�o�b�t�@</returns>
		const IndexBuffer& GetAnimatedIndexBuffer(int meshNo, int matNo) const
		{
			return m_meshArray[meshNo].m_animatedIndexBufferArray[matNo];
		}
		IndexBuffer& GetAnimatedIndexBuffer(int meshNo, int matNo)
		{
			return m_meshArray[meshNo].m_animatedIndexBufferArray[matNo];
		}
	private:
		/// <summary>
		/// �f�B�X�N���v�^�q�[�v���������B
		/// </summary>
		void CreateDescriptorHeaps(StructuredBuffer* worldMatrixArraySB);
	private:
		/// <summary>
		/// b0���W�X�^�Ƀo�C���h�����f�[�^�B
		/// </summary>
		struct CB_0 {
			Matrix worldMatrix;		// ���[���h�s��B
			int numVertex;			// ���_���B
			int outputOffset = 0;	// �o�̓I�t�Z�b�g�B
		};
		/// <summary>
		/// ���b�V���P�ʂ̏����ЂƂ܂Ƃ߂��邽�߂̍\���́B
		/// </summary>
		struct SMeshLocal {
			ConstantBuffer m_cb0;											// b0���W�X�^�Ƀo�C���h�����萔�o�b�t�@�B
			RootSignature m_rootSignature;									// ���[�g�V�O�l�`���B
			PipelineState m_pipelineState;									// �p�C�v���C���X�e�[�g�B
			VertexBuffer m_vertexBuffer;									// ���_�o�b�t�@�B
			VertexBuffer m_animatedVertexBuffer;							// �A�j���[�V�����v�Z�ς݂̒��_�o�b�t�@�B
			StructuredBuffer m_vertexBufferSB;								// ���_�o�b�t�@�̃X�g���N�`���[�h�o�b�t�@�B
			RWStructuredBuffer m_animatedVertexBufferRWSB;					// �A�j���[�V�����v�Z�ς݂̒��_�o�b�t�@��RW�X�g���N�`���[�h�o�b�t�@�B
			std::unique_ptr< IndexBuffer[] > m_animatedIndexBufferArray;	// �C���f�b�N�X�o�b�t�@�̔z��B
			std::unique_ptr< int[] > m_originalIndexBufferSize;				// �I���W�i���̃C���f�b�N�X�o�b�t�@�̃T�C�Y�B
			int m_numMaterial = 0;											// �}�e���A���̐��B
		};
		bool m_isFirstDispatch = true;
		int m_numMesh = 0;									// ���b�V���̐��B
		int m_numInstance = 0;								// �C���X�^���X�̐��B
		void* m_boneMatrixArray = nullptr;					// �{�[���s��̔z��̐擪�A�h���X�B
		DescriptorHeap m_descriptorHeap;					// �f�B�X�N���v�^�q�[�v�B
		Shader* m_shader;									// �V�F�[�_�[�B
		TkmFile m_tkmFile;									// tkm�t�@�C���B
		StructuredBuffer m_boneMatricesStructureBuffer;		// �{�[���s��̍\�����o�b�t�@�B
		std::unique_ptr< SMeshLocal[] > m_meshArray;		// ���b�V���B
	};
}