#pragma once
class Actor : public IGameObject
{
public:
	void Init(GamePad& gamePad, const char* filePath, const Vector3& initPos);
	void Update() override;
	void Render(RenderContext& rc) override;
private:
	ModelRender m_modelRender;
	Vector3 m_position;
	GamePad* m_gamePad = nullptr;
};

