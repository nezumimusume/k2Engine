#pragma once

// �f�B���N�V�������C�g
struct DirectionalLight
{
    Vector3 direction;  // ���C�g�̕���
    int castShadow;     // �e���L���X�g����H
    Vector4 color;      // ���C�g�̃J���[
};

// ���C�g�\����
struct Light
{
    std::array<DirectionalLight,
        NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT>
        directionalLight; // �f�B���N�V�������C�g
    Vector3 eyePos;         // �J�����̈ʒu
    float pad;
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