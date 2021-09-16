#pragma once
class Actor : public IGameObject
{
public:
	void Init(
		GamePad& gamePad, 
		const char* filePath, 
		const Vector3& initPos, 
		const Quaternion& initRot
	);
	void Update() override;
	void Render(RenderContext& rc) override;
private:
	enum EnAnimClip{
		enAnimClip_Attack,
		enAnimClip_Down,
		enAnimClip_Idle,
		enAnimClip_ReceiveDamage,
		enAnimClip_Walk,
		enAnimClip_Num
	};
	enum EnState {
		enState_Idle,
		enState_Walk,
		enState_Attack,
		enState_Down,
		enState_RecieveDamage,
		enState_Num,
	};
	EnState m_state = enState_Idle;
	AnimationClip m_animClip[enAnimClip_Num];
	ModelRender m_modelRender;
	Vector3 m_position;
	Quaternion m_rotation;
	GamePad* m_gamePad = nullptr;
};

