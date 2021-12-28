#pragma once

namespace nsK2Engine {
	struct SSpotLight;
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
        /// �J���[��ݒ�B
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
        /// �͈͂�ݒ�B
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            m_spotLight->SetRange(range);
        }

        /// <summary>
        /// �����ɂ��e�����̗ݏ搔��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam(float powParam)
        {
            m_spotLight->SetRangeAffectPowParam(powParam);
        }
        
        
        /// <summary>
        /// ���ɂ��e�����̗ݏ搔���擾�B
        /// </summary>
        /// <returns></returns>
        float GetRangeAffectPowParam() const
        {
            return m_spotLight->GetRangeAffectPowParam();
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
        /// �ˏo�p�x���擾�B
        /// </summary>
        /// <returns></returns>
        float GetAngle() const
        {
            return m_spotLight->GetAngle();
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
        /// �J���[���擾�B
        /// </summary>
        /// <returns></returns>
        const Vector3& GetColor() const
        {
            return m_spotLight->GetColor();
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