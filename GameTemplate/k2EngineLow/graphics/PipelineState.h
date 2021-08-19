#pragma once

namespace nsK2EngineLow {
	/// <summary>
	/// パイプラインステート。
	/// </summary>
	class PipelineState : public Noncopyable {
	public:
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~PipelineState();

		/// <summary>
		/// 初期化。
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
		/// 解放。
		/// </summary>
		void Release();
	private:
		ID3D12PipelineState* m_pipelineState = nullptr;	//パイプラインステート。
	};
}
