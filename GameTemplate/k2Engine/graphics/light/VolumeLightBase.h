#pragma once

namespace nsK2Engine {
	/// <summary>
	/// ���f���x�[�X�̃{�����[�����C�g�̃x�[�X�N���X�B
	/// </summary>
	class VolumeLightBase : Noncopyable {
	public:
		/// <summary>
		/// �f�X�g���N�^�B
		/// </summary>
		virtual ~VolumeLightBase();
		
		/// <summary>
		/// �X�V�����B
		/// </summary>
		virtual void Update() = 0;
		/// <summary>
		/// �������B
		/// </summary>
		/// <param name="lightData">���C�g�f�[�^</param>
		/// <param name="tkmFilePath">tkm�t�@�C���̃t�@�C���p�X</param>
		/// <param name="psFinalEntryFunc">�ŏI�p�X�̃s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g</param>
		void Init(
			const void* lightData,
			const char* tkmFilePath,
			const char* psFinalEntryFunc
		);
		/// <summary>
		/// �{�����[�����C�g�̔w�ʂ�`��
		/// </summary>
		/// <remark>
		/// ���̊֐��̓G���W��������Ă΂�܂��B
		/// �Q�[��������Ăяo�����Ȃ��悤�ɁB
		/// </remark>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		void DrawToVolumeLightMapBack(RenderContext& rc);
		/// <summary>
		/// �{�����[�����C�g�̑O�ʂ�`��B
		/// </summary>
		/// <remark>
		/// ���̊֐��̓G���W��������Ă΂�܂��B
		/// �Q�[��������Ăяo�����Ȃ��悤�ɁB
		/// </remark>
		/// <param name="rc">�����_�����O�R���e�L�X�g</param>
		void DrawToVolumeLightMapFront(RenderContext& rc);
		/// <summary>
		/// �ŏI�`��
		/// </summary>
		void DrawFinal(RenderContext& rc);
		
	private:
		/// <summary>
		/// ���C�g�f�[�^��ݒ�B
		/// </summary>
		/// <param name="lightData">���C�g�f�[�^</param>
		virtual void SetLightData(const void* lightData) = 0;
		virtual void* GetFinalCB()  = 0;
		virtual int GetFinalCBSize() const = 0;
		virtual void PreDrawFinal(RenderContext& rc) = 0;
	protected:
		
		Model m_modelFront;		// �{�����[�����C�g�̑O�ʕ`��p�̃��f���B
		Model m_modelBack;		// �{�����[�����C�g�̔w�ʕ`��p�̃��f���B
		Sprite m_final;			// �ŏI�`��B
		
	};
	template<class TLightData, class TLightDataRaw>
	class VolumeLight : public VolumeLightBase {
	private:
		/// <summary>
		/// ���C�g�f�[�^���Z�b�g�B
		/// </summary>
		/// <param name="lightData"></param>
		void SetLightData(const void* lightData) override
		{
			m_lightData = static_cast<const TLightData*>(lightData);
		}
		void* GetFinalCB()  override
		{
			return &m_finalCB;
		}
		int GetFinalCBSize() const override
		{
			return sizeof(m_finalCB);
		}
		void PreDrawFinal(RenderContext& rc) override
		{
			// �ŏI�`��̒萔�o�b�t�@���X�V����B
			m_finalCB.lightDataRaw = m_lightData->GetRawData();
			// �J�����֌W�̃f�[�^��Draw�̃p�X�ŃR�s�[���Ȃ��ƁAUpdate�̏��Ԃ�
			// 1�t���[���O�̃J�������ɂȂ��Ă��܂��B
			m_finalCB.mViewProjInv = g_camera3D->GetViewProjectionMatrixInv();
			m_finalCB.randomSeed = rand() % 100;
		}
	protected:
		/// <summary>
		/// �ŏI�`��̃p�X�̒萔�o�b�t�@�B
		/// </summary>
		struct FinalCB {
			TLightDataRaw lightDataRaw; // ���̃��C�g�f�[�^�B
			Matrix mViewProjInv;		// �r���[�v���W�F�N�V�����s��̋t�s��
			float randomSeed;			// �����̎�
		};
		const TLightData* m_lightData = nullptr;
		FinalCB m_finalCB;		// �ŏI�`��̒萔�o�b�t�@�B
	};
}