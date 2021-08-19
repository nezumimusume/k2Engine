#pragma once

namespace nsK2Engine {
    // �f�B���N�V�������C�g
    struct DirectionalLight
    {
        Vector3 direction;  // ���C�g�̕���
        int castShadow;     // �e���L���X�g����H
        Vector4 color;      // ���C�g�̃J���[
    };

    // �|�C���g���C�g�B

    /// <summary>
    /// �|�C���g���C�g�\����
    /// </summary>
    /// <remark>
    /// ���̍\���̂�POD�^�Ƃ��Ĉ����Ă��܂��B
    /// �{�\���̂ɉ��z�֐��Ȃǂ͐�΂ɒǉ����Ȃ��悤�ɂ��Ă��������B
    /// memcpy�Amemset�Ȃǂ̊֐��𗘗p���Ă���\��������܂��B
    /// ���z�֐��Ȃǂ�ǉ�����ƁA���z�֐��e�[�u�����󂳂�܂��B
    /// 
    /// �܂��A���̍\���̂̃I�u�W�F�N�g�̃f�[�^�̓V�F�[�_�[���ɒ萔�o�b�t�@�Ƃ��ē]������Ă��܂��B
    /// �����o�ϐ���ǉ������ꍇ�́AlightCulling.fx�ADeferredLighting.fx���ύX����K�v������܂��B
    /// </remark>
    struct PointLight
    {
    private:
        Vector3 position;       // ���W
        int isUse = false;      // �g�p���t���O�B
        Vector3 positionInView; // �J������Ԃł̍��W
        float pad1;
        Vector3 color;          // ���C�g�̃J���[
        float pad2;
        Vector3 attn;           // �����p�����[�^�Bx�ɉe���͈́Ay�ɂ͉e�����ɗݏ悷��p�����[�^�B
        float pad3;
    public:

        /// <summary>
        /// ���W��ݒ�B
        /// </summary>
        /// <param name="position"></param>
        void SetPosition(const Vector3& position)
        {
            this->position = position;
        }
        void SetPosition(float x, float y, float z)
        {
            SetPosition({ x, y, z });
        }
        /// <summary>
        /// �J���[��ݒ�B
        /// </summary>
        /// <param name="color"></param>
        void SetColor(const Vector3& color)
        {
            this->color = color;
        }
        void SetColor(float r, float g, float b)
        {
            SetColor({ r, g, b });
        }
        /// <summary>
        /// �͈͂�ݒ�B
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            attn.x = range;
        }
        /// <summary>
        /// �e�����̗ݏ搔��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetAffectPowParam(float powParam)
        {
            attn.y = powParam;
        }
        /// <summary>
        /// ���W���擾�B
        /// </summary>
        /// <returns></returns>
        const Vector3& GetPosition() const
        {
            return position;
        }
        /// <summary>
        /// �J���[���擾�B
        /// </summary>
        /// <returns></returns>
        const Vector3& GetColor() const
        {
            return color;
        }
        /// <summary>
        /// �e���͈͂��擾�B
        /// </summary>
        /// <returns></returns>
        float GetRange() const
        {
            return attn.x;
        }
        /// <summary>
        /// �|�C���g���C�g���g�p���ɂ���B
        /// </summary>
        /// /// <remark>
        /// ���̊֐���k2Engine�����ŗ��p����Ă��܂��B
        /// �Q�[��������͎g�p���Ȃ��悤�ɒ��ӂ��Ă��������B
        /// </remark>
        void Use()
        {
            isUse = true;
        }
        /// <summary>
        /// �|�C���g���C�g�𖢎g�p�ɂ���B
        /// </summary>
        /// <remark>
        /// ���̊֐���k2Engine�����ŗ��p����Ă��܂��B
        /// �Q�[��������͎g�p���Ȃ��悤�ɒ��ӂ��Ă��������B
        /// </remark>
        void UnUse()
        {
            isUse = false;
        }
        /// <summary>
        /// �X�V�B
        /// </summary>
        /// <remark>
        /// ���̊֐���k2Engine�����ŗ��p����Ă��܂��B
        /// �Q�[��������͎g�p���Ȃ��悤�ɒ��ӂ��Ă��������B
        /// </remark>
        void Update();

    };
    // ���C�g�\����
    struct Light
    {
        DirectionalLight directionalLight[MAX_DIRECTIONAL_LIGHT];   // �f�B���N�V���i�����C�g�̔z��B
        PointLight pointLights[MAX_POINT_LIGHT];                    // �|�C���g���C�g�̔z��B
        Matrix mViewProjInv;    // �r���[�v���W�F�N�V�����s��̋t�s��
        Vector3 eyePos;         // �J�����̈ʒu
        int numPointLight;      // �|�C���g���C�g�̐��B
        Vector3 ambinetLight;   // �����B
    };

    /// <summary>
    /// �V�[�����C�g�N���X�B
    /// </summary>
    class SceneLight : public Noncopyable {
    public:
        /// <summary>
        /// �������B
        /// </summary>
        void Init();
        /// <summary>
        /// �V�[�����C�g���擾�B
        /// </summary>
        /// <returns></returns>
        Light& GetSceneLight()
        {
            return m_light;
        }
        /// <summary>
        /// �f�B���N�V�������C�g�̃p�����[�^��ݒ�
        /// </summary>
        /// <param name="lightNo"></param>
        /// <param name="direction"></param>
        /// <param name="color"></param>
        void SetDirectionLight(int lightNo, Vector3 direction, Vector3 color)
        {
            m_light.directionalLight[lightNo].direction = direction;
            m_light.directionalLight[lightNo].color = color;
        }
        /// <summary>
        /// �e���L���X�g����H
        /// </summary>
        /// <param name="ligNo"></param>
        /// <returns></returns>
        bool IsCastShadow(int ligNo)
        {
            return m_light.directionalLight[ligNo].castShadow;
        }
        /// <summary>
        /// �V�[���Ƀ|�C���g���C�g��ǉ�
        /// </summary>

        /// <summary>
        /// �V�[���Ƀ|�C���g���C�g��ǉ�
        /// </summary>
        /// <remark>
        /// �{�֐��𗘗p���Ēǉ������|�C���g���C�g�́A
        /// �s�v�ɂȂ�����DeletePointLight()���g�p���āA�폜���Ă��������B
        /// </remark>
        /// <returns>�ǉ����ꂽ�|�C���g���C�g�̃A�h���X</returns>
        PointLight* NewPointLight()
        {
            if (m_unusePointLightQueue.empty()) {
                // ����ȏ�|�C���g���C�g��ǉ����邱�Ƃ͂ł��Ȃ��B
                return nullptr;
            }
            // ���g�p�̃|�C���g���C�g���ŃL���[������o���B
            auto* newPt = m_unusePointLightQueue.front();
            // �g�p���ɂ���B
            newPt->Use();
            // ���o�����v�f��擪���珜���B
            m_unusePointLightQueue.pop_front();
            return newPt;
        }
        /// <summary>
        /// �V�[������|�C���g���C�g���폜
        /// </summary>
        /// <param name="pointLight">�폜����|�C���g���C�g</param>
        void DeletePointLight(PointLight* pointLight)
        {
            if (pointLight != nullptr) {
                // �t���O�𖢎g�p�ɕύX����B
                pointLight->UnUse();
                // ���g�p���X�g�ɒǉ�����B
                m_unusePointLightQueue.push_back(pointLight);
            }
        }
        /// <summary>
        /// �����̌v�Z�̂��߂�IBL�e�N�X�`����ݒ�B
        /// </summary>
        /// <remark>
        /// ���̊֐��𗘗p���āAIBL�e�N�X�`�����Z�b�g����ƁA
        /// ������IBL�e�N�X�`������T���v�����O���āA����𗘗p����
        /// ���C�e�B���O���s���܂��B
        /// IBL�e�N�X�`���𗘗p���������̌v�Z���I�t�ɂ������ꍇ�́ADisableIBLForAmbinet()���Ăяo���āA
        /// IBL�𖳌��ɂ��Ă��������B
        /// </remark>
        /// <param name="textureFilePath">
        /// IBL�e�N�X�`���̃t�@�C���p�X�B
        /// �L���[�u�}�b�v�ł���K�v������܂��B
        /// </param>
        /// <param name="luminance">
        /// IBL�e�N�X�`���̖��邳�B
        /// <param>
        void SetIBLTextureForAmbient(const wchar_t* textureFilePath, float luminance);
        /// <summary>
        /// IBL�����𖳌��ɂ���B
        /// </summary>
        void DisableIBLTextureForAmbient()
        {
            // todo ���Ή��B
        }
        /// <summary>
        /// �X�V
        /// </summary>
        void Update();

    private:
        Light m_light;  //�V�[�����C�g�B
        std::deque< PointLight* > m_unusePointLightQueue; //���g�p�̃|�C���g���C�g�̃L���[�B
    };
}