#pragma once

class Game;

class MuscleRender : public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// モデルの最大数を設定する。
	/// </summary>
	/// <param name="numMuscle"></param>
	void SetMaxMuscle(int maxMuscle)
	{
		m_maxMuscle = maxMuscle;
	}
	/// <summary>
	/// インスタンシングデータの更新。
	/// </summary>
	/// <param name="pos"></param>
	/// <param name="rot"></param>
	/// <param name="scale"></param>
	void UpdateInstancingData(
		int instanceNo,
		const Vector3& pos,
		const Quaternion& rot, const Vector3& scale
	) {
		m_modelRender.UpdateInstancingData(instanceNo, pos, rot, scale);
	}
	/// <summary>
	/// インスタンスを破棄。
	/// </summary>
	/// <param name="instanceNo">インスタンス番号</param>
	void RemoveInstance(int instanceNo)
	{
		m_modelRender.RemoveInstance(instanceNo);
	}
private:
	ModelRender		m_modelRender;	// モデルレンダラー。
	int				m_maxMuscle = 0;	// 星の数

	AnimationClip	m_animationClip;
	Game*			m_game = nullptr;
};

