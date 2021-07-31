#pragma once

// �f�B���N�V�������C�g
struct DirectionalLight
{
    Vector3 direction;  // ���C�g�̕���
    int castShadow;     // �e���L���X�g����H
    Vector4 color;      // ���C�g�̃J���[
};

// �|�C���g���C�g�B
struct PointLight
{
    Vector3 position;       // ���W
    float pad0;
    Vector3 positionInView; // �J������Ԃł̍��W
    float pad1;
    Vector3 color;          // ���C�g�̃J���[
    float range;            // ���C�g�̉e����^����͈�
};
// ���C�g�\����
struct Light
{
    DirectionalLight directionalLight[MAX_DIRECTIONAL_LIGHT];   // �f�B���N�V���i�����C�g�̔z��B
    PointLight pointLights[MAX_POINT_LIGHT];                    // �|�C���g���C�g�̔z��B
    Matrix mViewProjInv;    // �r���[�v���W�F�N�V�����s��̋t�s��
    Vector3 eyePos;         // �J�����̈ʒu
    int numPointLight;      // �|�C���g���C�g�̐��B
    Vector3 ambinetLight;   // ����
};

/// <summary>
/// �V�[�����C�g�N���X�B
/// </summary>
class SceneLight : public Noncopyable{
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
private:
    Light m_light;  //�V�[�����C�g�B
};