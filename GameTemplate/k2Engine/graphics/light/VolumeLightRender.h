#pragma once


namespace nsK2Engine {
	/// <summary>
	/// �{�����[�����C�g�����_���[�B
	/// </summary>
	class VolumeLightRender : Noncopyable {
	public:
		/// <summary>
		/// �������B
		/// </summary>
		void Init();

		/// <summary>
		/// �{�����[�����C�g��`�悷��B
		/// </summary>
		void Render(RenderContext& rc, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);
		
		/// <summary>
		/// �{�����[���X�|�b�g���C�g�������_���[�ɒǉ�
		/// </summary>
		/// <param name="lig">���C�g</param>
		void AddVolumeSpotLight(VolumeSpotLight& lig)
		{
			m_volumeSpotLightArray.emplace_back(&lig);
		}
		/// <summary>
		/// �{�����[���X�|�b�g���C�g�������_���[����폜
		/// </summary>
		/// <param name="lig"></param>
		void RemoveVolumeSpotLight(VolumeSpotLight& lig)
		{
			auto it = std::find(m_volumeSpotLightArray.begin(), m_volumeSpotLightArray.end(), &lig);
			if (it != m_volumeSpotLightArray.end()) {
				m_volumeSpotLightArray.erase(it);
			}
		}
		Texture& GetVolumeLightMapFrontTexture()
		{
			return m_volumeLightMapFront.GetRenderTargetTexture();
		}
		Texture& GetVolumeLightMapBackTexture()
		{
			return m_volumeLightMapBack.GetRenderTargetTexture();
		}
	private:
		RenderTarget m_volumeLightMapFront;                     // ��O�̃{�����[�����C�g�}�b�v�B
		RenderTarget m_volumeLightMapBack;                      // �����̃{�����[�����C�g�}�b�v�B
		std::list< VolumeSpotLight* > m_volumeSpotLightArray;   // �{�����[���X�|�b�g���C�g�̔z��B
	};
}