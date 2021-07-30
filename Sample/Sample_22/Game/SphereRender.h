#pragma once
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
		const Vector3& pos,
		const Quaternion& rot, const Vector3& scale
	) {
		m_modelRender.UpdateInstancingData(pos, rot, scale);
	}
private:
	ModelRender	m_modelRender;	// モデルレンダラー。
	int			m_maxSphere = 0;	// 星の数
};

