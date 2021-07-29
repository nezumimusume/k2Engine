#pragma once
class StarRender : public IGameObject
{
public:
	bool Start();
	void Update();
	void Render(RenderContext& rc);
	/// <summary>
	/// ☆の最大数を設定する。
	/// </summary>
	/// <param name="numStar"></param>
	void SetMaxStar(int maxStar)
	{
		m_maxStar = maxStar;
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
	int			m_maxStar = 0;	// 星の数
};

