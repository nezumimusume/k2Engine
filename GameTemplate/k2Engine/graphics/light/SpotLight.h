#pragma once

namespace nsK2Engine {
	struct SSpotLight;
	/// <summary>
	/// �X�|�b�g���C�g�N���X�B
	/// </summary>
    /// <remark>
    /// ���̃X�|�b�g���C�g�ɂ͂R�̌���ݒ肷�邱�Ƃ��ł��܂��B
    /// �Q�ԖڂƂR�Ԗڂ̌��̓{�����[�����C�g�p�̃p�����[�^�ł��B
    /// �{�����[�����C�g����������ۂɂ́A�u�������������点�����v��
    /// �u�������������点�����v�Ƃ���������������ꍇ������܂��B
    /// ���̂悤�Ȓ������������ꍇ�ɗ��p���Ă��������B
    /// �{�����[�����C�g�ł̎g�p���@��Sample_27���Q�Ƃ��Ă��������B
    /// </remark>
	class SpotLight : Noncopyable{
	public:
		// �f�X�g���N�^�B
		~SpotLight();
		/// <summary>
		/// �������B
		/// </summary>
		void Init();
        /// <summary>
        /// �X�|�b�g���C�g�̔ԍ����擾�B
        /// </summary>
        /// <returns></returns>
        int GetNo() const
        {
            return m_spotLight->GetNo();
        }
        /// <summary>
        /// �X�|�b�g���C�g�̔ԍ���ݒ�B
        /// </summary>
        /// <param name="no"></param>
        void SetNo(int no)
        {
            m_spotLight->SetNo(no);
        }
        /// <summary>
        /// �ˏo������ݒ�B
        /// </summary>
        /// <param name="direction"></param>
        void SetDirection(const Vector3& direction)
        {
            m_spotLight->SetDirection(direction);
        }
        /// <summary>
        /// �ˏo������ݒ�B
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="z"></param>
        void SetDirection(float x, float y, float z)
        {
            m_spotLight->SetDirection(x, y, z);
        }
        /// <summary>
        /// �ˏo�������擾�B
        /// </summary>
        /// <returns></returns>
        const Vector3& GetDirection() const
        {
            return m_spotLight->GetDirection();
            
        }
        /// <summary>
        /// ���W��ݒ�B
        /// </summary>
        /// <param name="position"></param>
        void SetPosition(const Vector3& position)
        {
            m_spotLight->SetPosition(position);
        }
        void SetPosition(float x, float y, float z)
        {
            m_spotLight->SetPosition( x, y, z );
        }
        /// <summary>
        /// ��ڂ̌��̃J���[��ݒ�B
        /// </summary>
        /// <param name="color"></param>
        void SetColor(const Vector3& color)
        {
            m_spotLight->SetColor(color);
        }
        void SetColor(float r, float g, float b)
        {
            m_spotLight->SetColor( r, g, b );
        }
        /// <summary>
        /// ��ڂ̌��̃J���[��ݒ�B
        /// </summary>
        /// <reamrk>
        /// �{�����[�����C�g���g�p����Ƃ��ɂ̂ݎg�p����܂��B
        /// </remark>
        /// <param name="color"></param>
        void SetColor2(const Vector3 color)
        {
            m_spotLight->SetColor2(color);
        }
        void SetColor2(float r, float g, float b)
        {
            m_spotLight->SetColor2(r, g, b);
        }
        /// <summary>
        /// �O�ڂ̌��̃J���[��ݒ�B
        /// </summary>
        /// <reamrk>
        /// �{�����[�����C�g���g�p����Ƃ��ɂ̂ݎg�p����܂��B
        /// </remark>
        /// <param name="color"></param>
        void SetColor3(const Vector3 color)
        {
            m_spotLight->SetColor3(color);
        }
        void SetColor3(float r, float g, float b)
        {
            m_spotLight->SetColor3(r, g, b);
        }
        /// <summary>
        /// �X�|�b�g���C�g�͈̔͂�ݒ�B
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            m_spotLight->SetRange(range);
        }

        /// <summary>
        /// ��ڂ̌��̋����ɂ��e�����̗ݏ搔��ݒ�B
        /// </summary>
        /// <remark>
        /// SetColor()�Őݒ肵�����ɉe����^���܂��B
        /// </remark>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam(float powParam)
        {
            m_spotLight->SetRangeAffectPowParam(powParam);
        }
        /// <summary>
        /// ��ڂ̌��̋����ɂ��e�����̗ݏ搔��ݒ�B
        /// </summary>
        /// <remark>
        /// SetColor2()�Őݒ肵�����ɉe����^���܂��B
        /// �{�����[�����C�g���g�p����Ƃ��ɂ̂ݎg�p����܂��B
        /// </remark>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam2(float powParam)
        {
            m_spotLight->SetRangeAffectPowParam2(powParam);
        }
        /// <summary>
        /// �O�ڂ̌��̋����ɂ��e�����̗ݏ搔��ݒ�B
        /// </summary>
        /// <remark>
        /// SetColor3()�Őݒ肵�����ɉe����^���܂��B
        /// �{�����[�����C�g���g�p����Ƃ��ɂ̂ݎg�p����܂��B
        /// </remark>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam3(float powParam)
        {
            m_spotLight->SetRangeAffectPowParam3(powParam);
        }
        /// <summary>
        /// ��ڂ̌��̃X�|�b�g���C�g�ƃs�N�Z���̊p�x�ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam(float powParam)
        {
            m_spotLight->SetAngleAffectPowParam(powParam);
        }
        /// <summary>
        /// ��ڂ̌��̃X�|�b�g���C�g�ƃs�N�Z���̊p�x�ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <remark>
        /// SetColor2()�Őݒ肵�����ɉe����^���܂��B
        /// �{�����[�����C�g���g�p����Ƃ��ɂ̂ݎg�p����܂��B
        /// </remark>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam2(float powParam)
        {
            m_spotLight->SetAngleAffectPowParam2(powParam);
        }
        /// <summary>
        /// �O�ڂ̌��̃X�|�b�g���C�g�ƃs�N�Z���̊p�x�ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <remark>
        /// SetColor3()�Őݒ肵�����ɉe����^���܂��B
        /// �{�����[�����C�g���g�p����Ƃ��ɂ̂ݎg�p����܂��B
        /// </remark>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam3(float powParam)
        {
            m_spotLight->SetAngleAffectPowParam3(powParam);
        }

        /// <summary>
        /// �ˏo�p�x
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle(float angle)
        {
            m_spotLight->SetAngle(angle);
        }
        /// <summary>
        /// ��ڂ̌��̎ˏo�p�x
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle2(float angle)
        {
            m_spotLight->SetAngle2(angle);
        }
        /// <summary>
        /// �O�ڂ̌��̎ˏo�p�x
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle3(float angle)
        {
            m_spotLight->SetAngle3(angle);
        }
        /// <summary>
        /// �ˏo�p�x���擾�B
        /// </summary>
        /// <returns></returns>
        float GetAngle() const
        {
            return m_spotLight->GetAngle();
        }
        float GetAngle2() const
        {
            return m_spotLight->GetAngle2();
        }
        float GetAngle3() const
        {
            return m_spotLight->GetAngle3();
        }
        /// <summary>
        /// ���W���擾�B
        /// </summary>
        /// <returns></returns>
        const Vector3& GetPosition() const
        {
            return m_spotLight->GetPosition();
        }
        /// <summary>
        /// �e���͈͂��擾�B
        /// </summary>
        /// <returns></returns>
        float GetRange() const
        {
            return m_spotLight->GetRange();
        }
        /// <summary>
        /// �X�V
        /// </summary>
        void Update()
        {
            m_spotLight->Update();
        }
        /// <summary>
        /// ���f�[�^���擾�B
        /// </summary>
        /// <returns></returns>
        const SSpotLight& GetRawData() const
        {
            return *m_spotLight;
        }
	private:
		SSpotLight* m_spotLight = nullptr;
	};
}