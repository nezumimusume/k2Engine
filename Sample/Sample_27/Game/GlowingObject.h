#pragma once
class GlowingObject : public IGameObject
{
public:
	GlowingObject();
	~GlowingObject();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
private:
	ModelRender					m_modelRender;
	SpotLight					m_spotLight;				//�X�|�b�g���C�g�B
	VolumeSpotLight				m_volumeSpotLight;			//�{�����[�����C�g�B
	Vector3						m_position;
	Quaternion					m_rotation;
	float						m_timer = 1.7f;
};

