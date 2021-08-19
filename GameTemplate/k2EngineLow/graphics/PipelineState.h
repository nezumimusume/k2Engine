#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// �p�C�v���C���X�e�[�g�B
	/// </summary>
	class PipelineState : public Noncopyable {
	public:
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		~PipelineState();

		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="desc"></param>
		void Init(D3D12_GRAPHICS_PIPELINE_STATE_DESC desc);
		void Init(D3D12_COMPUTE_PIPELINE_STATE_DESC desc);
		ID3D12PipelineState* Get()
		{
			return m_pipelineState;
		}
	private:
		/// <summary>
		/// ����B
		/// </summary>
		void Release();
	private:
		ID3D12PipelineState* m_pipelineState = nullptr;	//�p�C�v���C���X�e�[�g�B
	};
}
