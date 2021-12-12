#pragma once

/// <summary>
/// パスのポイント。
/// </summary>
struct Point {
	Vector3		s_vector = Vector3::Zero;		//座標。
	int			s_number = 0;					//番号。
};

class Path
{
public:
	/// <summary>
	/// コンストラクタ。
	/// </summary>
	Path();
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~Path();
	/// <summary>
	/// パスデータを読み込む。
	/// </summary>
	/// <param name="filePath">ファイルパス。</param>
	void Load(const char* filePath);
	/// <summary>
	/// パスのポイントの数を取得。
	/// </summary>
	/// <returns>パスのポイントの数。</returns>
	const int GetNumberPoint() const
	{
		return static_cast<const int>(m_pointMap.size());
	}
	/// <summary>
	/// 最初のポイントを取得。
	/// </summary>
	/// <returns> 最初のポイント。</returns>
 	const Point* GetFistPoint()
	{
		return &m_pointMap[0];
	}
	/// <summary>
	/// 現在の座標から一番近いポイントを取得。
	/// </summary>
	/// <param name="position">座標。</param>
	/// <returns>ポイント。</returns>
	const Point* GetNearPoint(const Vector3& position);
	/// <summary>
	/// 次のポイントを取得。
	/// </summary>
	/// <param name="number">現在のポイントの番号。</param>
	/// <returns>ポイント。</returns>
	const Point* GetNextPoint(const int number);
private:
	std::map<const int ,Point>		m_pointMap;			//ポイントのマップ。
};

