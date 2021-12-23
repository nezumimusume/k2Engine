#pragma once

namespace nsK2Engine {
	/// <summary>
	/// �|�C���g���C�g
	/// </summary>
	class PointLight : Noncopyable {
	public:
		// �f�X�g���N�^
		~PointLight();
		/// <summary>
		/// �������B
		/// </summary>
		void Init();
        /// <summary>
        /// ���W��ݒ�B
        /// </summary>
        /// <param name="position"></param>
        void SetPosition(const Vector3& position)
        {
            m_pointLight->SetPosition(position);
        }
        void SetPosition(float x, float y, float z)
        {
            m_pointLight->SetPosition(x, y, z);
        }
        /// <summary>
        /// �J���[��ݒ�B
        /// </summary>
        /// <param name="color"></param>
        void SetColor(const Vector3& color)
        {
            m_pointLight->SetColor(color);
        }
        void SetColor(float r, float g, float b)
        {
            m_pointLight->SetColor( r, g, b );
        }
        /// <summary>
        /// �͈͂�ݒ�B
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            m_pointLight->SetRange(range);
        }
        /// <summary>
        /// �e�����̗ݏ搔��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetAffectPowParam(float powParam)
        {
            m_pointLight->SetAffectPowParam(powParam);
        }
        /// <summary>
        /// ���W���擾�B
        /// </summary>
        /// <returns></returns>
        const Vector3& GetPosition() const
        {
            return m_pointLight->GetPosition();
        }
        /// <summary>
        /// �J���[���擾�B
        /// </summary>
        /// <returns></returns>
        const Vector3& GetColor() const
        {
            return m_pointLight->GetColor();
        }
        /// <summary>
        /// �e���͈͂��擾�B
        /// </summary>
        /// <returns></returns>
        float GetRange() const
        {
            return m_pointLight->GetRange();
        }
        /// <summary>
        /// �X�V�B
        /// </summary>
        void Update()
        {
            m_pointLight->Update();
        }
        /// <summary>
        /// ���f�[�^���擾�B
        /// </summary>
        /// <returns></returns>
        const SPointLight& GetRawData() const
        {
            return *m_pointLight;
        }
	private:
		SPointLight* m_pointLight = nullptr;	// �|�C���g���C�g�B
	};
}