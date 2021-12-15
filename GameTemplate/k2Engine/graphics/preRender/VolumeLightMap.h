#pragma once


namespace nsK2Engine {
	/// <summary>
	/// �{�����[�����C�g�}�b�v�B
	/// </summary>
	class VolumeLightMap : Noncopyable {
	public:
		/// <summary>
		/// �������B
		/// </summary>
		void Init();

		/// <summary>
		/// �{�����[�����C�g�}�b�v��`�悷��B
		/// </summary>
		/// <param name="rc"></param>
		void Render(RenderContext& rc);
		/// <summary>
		/// �{�����[�����C�g���f���̔w�ʂ�Z�l���������񂾃e�N�X�`�����擾�B
		/// </summary>
		/// <returns></returns>
		Texture& GetVolumeLightMapBackTexture()
		{
			return m_volumeLightMapBack.GetRenderTargetTexture();
		}
		/// <summary>
		/// �{�����[�����C�g���f���̑O�ʂ�Z�l���������񂾃e�N�X�`�����擾�B
		/// </summary>
		/// <returns></returns>
		Texture& GetVolumeLightMapFrontTexture()
		{
			return m_volumeLightMapFront.GetRenderTargetTexture();
		}
		/// <summary>
		/// �{�����[���X�|�b�g���C�g���V�[���ɒǉ�
		/// </summary>
		/// <param name="lig">���C�g</param>
		void AddVolumeSpotLight(VolumeSpotLight& lig)
		{
			m_volumeSpotLightArray.emplace_back(&lig);
		}
		/// <summary>
		/// �{�����[���X�|�b�g���C�g���V�[������폜
		/// </summary>
		/// <param name="lig"></param>
		void RemoveVolumeSpotLight(VolumeSpotLight& lig)
		{
			auto it = std::find(m_volumeSpotLightArray.begin(), m_volumeSpotLightArray.end(), &lig);
			if (it != m_volumeSpotLightArray.end()) {
				m_volumeSpotLightArray.erase(it);
			}
		}
	private:
		RenderTarget m_volumeLightMapFront;                     // ��O�̃{�����[�����C�g�}�b�v�B
		RenderTarget m_volumeLightMapBack;                      // �����̃{�����[�����C�g�}�b�v�B
		std::list< VolumeSpotLight* > m_volumeSpotLightArray;   // �{�����[���X�|�b�g���C�g�̔z��B
	};
}