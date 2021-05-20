#pragma once

struct CaslData;
/// <summary>
/// マップチップ2D。
/// </summary>
class MapChip2D
{
public:
	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="caslData">Caslデータ。</param>
	void Init(CaslData* caslData);
	/// <summary>
	/// 更新処理。
	/// </summary>
	void Update()
	{
		m_sprite.Update(m_position, m_rotation, m_scale);
	}
	/// <summary>
	/// スプライトを描画する。
	/// </summary>
	/// <param name="renderContext">レンダーコンテキスト。</param>
	void Draw(RenderContext& renderContext)
	{
		m_sprite.Draw(renderContext);
	}
private:
	Vector3 m_position;					//座標。
	Quaternion m_rotation;				//回転。
	Vector3 m_scale = g_vec3One;	//大きさ。
	Sprite m_sprite;						//スプライト。
};

