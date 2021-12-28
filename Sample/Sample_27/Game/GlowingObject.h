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
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
private:
	ModelRender					m_modelRender;
	SpotLight					m_spotLight;				//スポットライト。
	VolumeSpotLight				m_volumeSpotLight;			//ボリュームライト。
	Vector3						m_position;
	Quaternion					m_rotation;
	float						m_timer = 1.7f;
};

