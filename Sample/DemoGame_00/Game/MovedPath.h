#pragma once
#include "Path.h"

/// <summary>
/// オブジェクトをパス移動させる。
/// </summary>
class MovedPath
{
public:
	/// <summary>
	/// コンストラクタ。
	/// </summary>
	MovedPath();
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~MovedPath();
	const Vector3& Move();
	/// <summary>
	/// 初期化処理。
	/// </summary>
	/// <param name="position">座標。</param>
	/// <param name="moveSpeed">移動速度。</param>
	void Init(const Vector3& position, const float moveSpeed);
	/// <summary>
	/// パスデータを読み込む。
	/// </summary>
	/// <param name="filePath">ファイルパス。</param>
	void ReadPath(const char* filePath);
	/// <summary>
	/// 移動方向のベクトルを取得。
	/// </summary>
	/// <returns>移動方向ベクトル。</returns>
	const Vector3& GetMoveVector() const
	{
		return m_moveVector;
	}
	/// <summary>
	/// 停止中？
	/// </summary>
	/// <returns>停止中ならtrue。</returns>
	const bool GetIsStop() const
	{
		return m_isStop;
	}
private:
	Vector3			m_position = Vector3::Zero;					//座標
	Vector3			m_moveVector = Vector3::Zero;				//移動ベクトル
	float			m_moveSpeed = 0.0f;							//移動速度
	bool			m_isCulcMoveVector = false;					//移動ベクトルを計算したかどうか
	Path			m_path;
	const Point*	m_point = nullptr;							//今向かってるポイント
	bool			m_isStart = true;							//スタート
	bool			m_isStop = false;							//ストップ？
	bool			m_isAddDegree = true;						//方向転換
	float			m_time = 0.0f;								//方向転換の時間
	float			m_timer = 0.0f;
};

