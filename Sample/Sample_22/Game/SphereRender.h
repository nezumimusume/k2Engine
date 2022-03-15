#pragma once

class Game;

class SphereRender : public IGameObject
{
public:
	
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// 球の最大数を設定する。
	/// </summary>
	/// <param name="numSphere"></param>
	void SetMaxSphere(int maxSphere)
	{
		m_maxSphere = maxSphere;
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
		const Quaternion& rot, 
		const Vector3& scale
	) {
		m_modelRender.UpdateInstancingData(instanceNo, pos, rot, scale);
	}
	/// <summary>
	/// インスタンスを破棄。
	/// </summary>
	/// <param name="instanceNo">削除するインスタンスの番号</param>
	void RemoveInstance(int instanceNo)
	{
		m_modelRender.RemoveInstance(instanceNo);
	}
private:
	ModelRender	m_modelRender;	// モデルレンダラー。
	int			m_maxSphere = 0;	// 星の数
	Game*		m_game = nullptr;
};

