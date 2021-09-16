#pragma once
class Actor : public IGameObject
{
public:
	void Init(
		GamePad& gamePad, 
		const char* filePath, 
		const Vector3& initPos, 
		const Quaternion& initRot,
		Actor* pOtherActor
	);
	void Update() override;
	void Render(RenderContext& rc) override;
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	bool ApplyDamage();

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
	enum EnAttackCollisionState {
		enAttackCollisionState_BeforeInvoke,	// 攻撃コリジョン発生前	
		enAttackCollisionState_Invoke,			// 攻撃コリジョン発生中
		enAttackCollisionState_Disappearance,	// 攻撃コリジョン消滅
	};
	EnState m_state = enState_Idle;
	AnimationClip m_animClip[enAnimClip_Num];
	ModelRender m_modelRender;
	Vector3 m_position;
	Quaternion m_rotation;
	GamePad* m_gamePad = nullptr;
	Actor* m_otherActor = nullptr;			// 対戦相手。
	EnAttackCollisionState m_attackCollisionState = enAttackCollisionState_BeforeInvoke;
	float m_attackTimer = 0.0f;
};

