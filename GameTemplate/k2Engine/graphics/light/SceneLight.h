#pragma once

namespace nsK2Engine {

    class VolumeSpotLight;
    // �f�B���N�V�������C�g
    struct DirectionalLight
    {
        Vector3 direction;  // ���C�g�̕���
        int castShadow;     // �e���L���X�g����H
        Vector4 color;      // ���C�g�̃J���[
    };


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
    struct SPointLight
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
    /// <summary>
    /// �X�|�b�g���C�g�\����
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
   struct SSpotLight {
   private:
        Vector3 position;                   // ���W
        int isUse = false;                  // �g�p���t���O�B
        Vector3 positionInView;             // �J������Ԃł̍��W�B
        int no = 0;                         // ���C�g�̔ԍ��B
        Vector3 direction;                  // �ˏo�����B
        float range;                        // �e���͈́B
        Vector3 color = g_vec3Zero;         // ���C�g�̃J���[�B
        float angleAffectMul = 1.0f;        // �p�x�ɂ�錸�����ɏ�Z����p�����[�^�B
        Vector3 color2 = g_vec3Zero;        // ��ڂ̃J���[�B
        float pad1;
        Vector3 color3 = g_vec3Zero;        // �O�ڂ̃J���[�B
        float pad2;
        Vector3 directionInView;            // �J������Ԃł̎ˏo�����B
        float pad3;
        Vector3 rangePow = g_vec3One;       // �����ɂ����̉e�����ɗݏ悷��p�����[�^�[�B1.0�Ő��`�̕ω�������B
                                            // x����ڂ̃J���[�Ay����ڂ̃J���[�Az���O�ڂ̃J���[�B
        float pad4;
        Vector3 angle;                      // �ˏo�p�x(�P�ʁF���W�A���Bx����ڂ̃J���[�Ay����ڂ̃J���[�Az���O�ڂ̃J���[)�B
        float pad5;
        Vector3 anglePow = g_vec3One;       // �X�|�b�g���C�g�Ƃ̊p�x�ɂ����̉e�����ɗݏ悷��p�����[�^�B1.0�Ő��`�ɕω�����B
                                            // x����ڂ̃J���[�Ay����ڂ̃J���[�Az���O�ڂ̃J���[�B
        float pad6;
   public:
       /// <summary>
       /// �X�|�b�g���C�g�̔ԍ����擾�B
       /// </summary>
       /// <returns></returns>
       int GetNo() const
       {
           return no;
       }
       /// <summary>
       /// �X�|�b�g���C�g�̔ԍ���ݒ�B
       /// </summary>
       /// <param name="no"></param>
       void SetNo(int no)
       {
           this->no = no;
       }
        /// <summary>
        /// �ˏo������ݒ�B
        /// </summary>
        /// <param name="direction"></param>
        void SetDirection(const Vector3& direction)
        {
            this->direction = direction;
            this->direction.Normalize();
        }
        /// <summary>
        /// �ˏo������ݒ�B
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="z"></param>
        void SetDirection(float x, float y, float z)
        {
            direction.Set(x, y, z);
            direction.Normalize();
        }
        /// <summary>
        /// �ˏo�������擾�B
        /// </summary>
        /// <returns></returns>
        const Vector3& GetDirection() const
        {
            return direction;
;        }
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
        /// ��ڂ̌��̃J���[��ݒ�B
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
        /// ��ڂ̌��̃J���[��ݒ�B
        /// </summary>
        /// <param name="color"></param>
        void SetColor2(const Vector3 color)
        {
            color2 = color;
        }
        void SetColor2(float r, float g, float b)
        {
            SetColor2({ r, g, b });
        }
        /// <summary>
        /// �O�ڂ̌��̃J���[��ݒ�B
        /// </summary>
        /// <param name="color"></param>
        void SetColor3(const Vector3 color)
        {
            color3 = color;
        }
        void SetColor3(float r, float g, float b)
        {
            SetColor3({ r, g, b });
        }
        /// <summary>
        /// �X�|�b�g���C�g�̉e���͈͂�ݒ�B
        /// </summary>
        /// <param name="range"></param>
        void SetRange(float range)
        {
            this->range = range;
        }
        /// <summary>
        /// ��ڂ̌��̊p�x�ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam(float powParam)
        {
            this->anglePow.x = powParam;
        }
        /// <summary>
        /// ��ڂ̌��̊p�x�ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam2(float powParam)
        {
            anglePow.y = powParam;
        }
        /// <summary>
        /// �O�ڂ̌��̊p�x�ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetAngleAffectPowParam3(float powParam)
        {
            anglePow.z = powParam;
        }
        
        /// <summary>
        /// ��ڂ̌��̋����ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam(float powParam)
        {
            rangePow.x = powParam;
        }
        /// <summary>
        /// ��ڂ̌��̋����ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam2(float powParam)
        {
            rangePow.y = powParam;
        }
        /// <summary>
        /// �O�ڂ̌��̋����ɂ��e�����ɗݏ悷��l��ݒ�B
        /// </summary>
        /// <param name="powParam"></param>
        void SetRangeAffectPowParam3(float powParam)
        {
            rangePow.z = powParam;
        }
        
        /// <summary>
        /// ��ڂ̌��̎ˏo�p�x��ݒ�B
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle(float angle)
        {
            this->angle.x = angle;
        }
        /// <summary>
        /// ��ڂ̌��̎ˏo�p�x��ݒ�B
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle2(float angle)
        {
            this->angle.y = angle;
        }
        /// <summary>
        /// �O�ڂ̌��̎ˏo�p�x��ݒ�B
        /// </summary>
        /// <param name="angle"></param>
        void SetAngle3(float angle)
        {
            this->angle.z = angle;
        }
        /// <summary>
        /// ��ڂ̌��̎ˏo�p�x���擾�B
        /// </summary>
        /// <returns></returns>
        float GetAngle() const
        {
            return angle.x;
        }
        /// <summary>
        /// ��ڂ̌��̎ˏo�p�x���擾�B
        /// </summary>
        /// <returns></returns>
        float GetAngle2() const
        {
            return angle.y;
        }
        /// <summary>
        /// �O�ڂ̌��̎ˏo�p�x���擾�B
        /// </summary>
        /// <returns></returns>
        float GetAngle3() const
        {
            return angle.z;
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
        /// �e���͈͂��擾�B
        /// </summary>
        /// <returns></returns>
        float GetRange() const
        {
            return range;
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
        SPointLight pointLights[MAX_POINT_LIGHT];                    // �|�C���g���C�g�̔z��B
        SSpotLight spotLights[MAX_SPOT_LIGHT];                       // �X�|�b�g���C�g�̔z��B
        Matrix mViewProjInv;    // �r���[�v���W�F�N�V�����s��̋t�s��
        Vector3 eyePos;         // �J�����̈ʒu
        int numPointLight;      // �|�C���g���C�g�̐��B
        Vector3 ambinetLight;   // �����B
        int numSpotLight;       // �X�|�b�g���C�g�̐��B
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
        /// <remark>
        /// �{�֐��𗘗p���Ēǉ������|�C���g���C�g�́A
        /// �s�v�ɂȂ�����DeletePointLight()���g�p���āA�폜���Ă��������B
        /// </remark>
        /// <returns>�ǉ����ꂽ�|�C���g���C�g�̃A�h���X</returns>
        SPointLight* NewPointLight()
        {
            return NewDynamicLight<SPointLight>(m_unusePointLightQueue);
        }
        /// <summary>
        /// �V�[������|�C���g���C�g���폜
        /// </summary>
        /// <param name="pointLight">�폜����|�C���g���C�g</param>
        void DeletePointLight(SPointLight* pointLight)
        {
            DeleteDynamicLight<SPointLight>(pointLight, m_unusePointLightQueue);
        }
        /// <summary>
        /// �V�[���ɃX�|�b�g���C�g��ǉ�
        /// </summary>
        /// <remark>
        /// ���̊֐��̓G���W�����ŗ��p����܂��B
        /// �Q�[�����ł͗��p���Ȃ��ł��������B
        /// </remark>
        /// <returns>�ǉ����ꂽ�X�|�b�g���C�g�̃A�h���X</returns>
        SSpotLight* NewSpotLight()
        {
            return NewDynamicLight<SSpotLight>(m_unuseSpotLightQueue);
        }
        /// <summary>
        /// �V�[������X�|�b�g���C�g���폜
        /// </summary>
        /// <param name="spotLight">�폜����X�|�b�g���C�g</param>
        void DeleteSpotLight(SSpotLight* spotLight)
        {
            DeleteDynamicLight<SSpotLight>(spotLight, m_unuseSpotLightQueue);
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
        /// </param>
        void SetAmbientByIBLTexture(const wchar_t* textureFilePath, float luminance);
        /// <summary>
        /// IBL�����𖳌��ɂ���B
        /// </summary>
        void DisableIBLTextureForAmbient()
        {
            // todo ���Ή��B
        }
        /// <summary>
        /// ������ݒ�B
        /// </summary>
        /// <param name="ambient"></param>
        void SetAmbinet(Vector3 ambient)
        {
            m_light.ambinetLight = ambient;
        }
        /// <summary>
        /// �X�V
        /// </summary>
        void Update();
    private:
        /// <summary>
        /// �V�������I���C�g��ǉ��B
        /// </summary>
        /// <typeparam name="TDynamicLight"></typeparam>
        /// <typeparam name="TQue"></typeparam>
        /// <param name="que"></param>
        /// <returns></returns>
        template<class TDynamicLight>
        TDynamicLight* NewDynamicLight(std::deque< TDynamicLight*>& que)
        {
            if (que.empty()) {
                // ����ȏド�C�g��ǉ����邱�Ƃ͂ł��Ȃ��B
                return nullptr;
            }
            // ���g�p�̃��C�g���ŃL���[������o���B
            TDynamicLight* newPt = que.front();
            // �g�p���ɂ���B
            newPt->Use();
            // ���o�����v�f��擪���珜���B
            que.pop_front();
            return newPt;
        }
        /// <summary>
        /// �V�[�����瓮�I���C�g���폜
        /// </summary>
        /// <param name="pointLight">�폜����|�C���g���C�g</param>
        template<class TDynamicLight>
        void DeleteDynamicLight(TDynamicLight* light, std::deque< TDynamicLight*>& que)
        {
            if (light != nullptr) {
                // �t���O�𖢎g�p�ɕύX����B
                light->UnUse();
                // ���g�p���X�g�ɒǉ�����B
                que.push_back(light);
            }
        }
    private:
        Light m_light;  //�V�[�����C�g�B
        std::deque< SPointLight* > m_unusePointLightQueue;       // ���g�p�̃|�C���g���C�g�̃L���[�B
        std::deque< SSpotLight* > m_unuseSpotLightQueue;         // ���g�p�̃X�|�b�g���C�g�̃L���[�B�B
    };
}