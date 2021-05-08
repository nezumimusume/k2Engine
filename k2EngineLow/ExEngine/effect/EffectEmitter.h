#pragma once
#include "Effect.h"

/// <summary>
/// エフェクトを発生、再生するクラス。
/// </summary>
class EffectEmitter : public IGameObject
{
public:
	/// <summary>
	/// コンストラクタ。
	/// </summary>
	EffectEmitter();
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~EffectEmitter();
	/// <summary>
	/// 更新処理。
	/// </summary>
	void Update();
	/// <summary>
	/// エフェクトを読み込む。
	/// </summary>
	/// <param name="number">EffectEngine::GetInstance()->ResistEffectで指定した番号。</param>
	void Init(const int number);
	/// <summary>
	/// 座標を設定。
	/// </summary>
	/// <param name="position">座標。/param>
	void SetPosition(const Vector3& position)
	{
		m_position = position;
	}
	/// <summary>
	/// 座標を取得。
	/// </summary>
	/// <returns>座標。</returns>
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/// <summary>
	/// スケール(拡大率を設定)。
	/// </summary>
	/// <param name="scale">スケール(拡大率)。</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
	/// スケール(拡大率)を取得。
	/// </summary>
	/// <returns>スケール(拡大率)。</returns>
	const Vector3& GetScale() const
	{
		return m_scale;
	}
	/// <summary>
	/// 回転を設定。
	/// </summary>
	/// <param name="rotation">回転。</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}
	/// <summary>
	/// 回転を取得。
	/// </summary>
	/// <returns>回転。</returns>
	const Quaternion& GetRotation() const
	{
		return m_rotation;
	}
	/// <summary>
	/// 移動速度を設定。
	/// </summary>
	/// <param name="moveSpeed">移動速度。</param>
	void SetMoveSpeed(const Vector3& moveSpeed)
	{
		m_moveSpeed = moveSpeed;
	}
	/// <summary>
	/// 移動速度を取得。
	/// </summary>
	/// <returns>移動速度。</returns>
	const Vector3& GetMoveSpeed() const
	{
		return m_moveSpeed;
	}
	/// <summary>
	/// 再生中かどうかを取得。
	/// </summary>
	/// <returns>再生中ならtrue。</returns>
	const bool GetIsPlay() const
	{
		return m_effect.IsPlay();
	}
private:
	Effect m_effect;					//エフェクト。
	Vector3 m_position;					//座標。
	Vector3 m_moveSpeed;				//移動速度。
	Quaternion m_rotation;				//回転。
	Vector3 m_scale = g_vec3One;		//スケール(拡大率)。
};

