#pragma once

namespace nsK2Engine {
	/// <summary>
	/// カメラのコリジョンソルバー。
	///
	/// 一般的な三人称視点のゲームカメラの衝突解決を提供します。
	/// カメラを背景にめり込ませたくない場合などに使用してください。
	///
	///典型的な使用例。
	///int main()
	///{
	///	・
	///		・
	///		省略
	///		ゲームの初期化などを行う。。
	///		・
	///		・
	///		CameraCollisionSolver cameraCollisionSolver;
	///		//カメラのコリジョンソルバーを初期化する。
	///		cameraCollisionSolver.Init(50.0f);
	///		while (true) {	//ゲームループ
	///		・
	///			・
	///			省略
	///			このフレームのカメラの視点や注意点を計算したりする。
	///			・
	///			・
	///			Vector3 newCameraPosition;	//新しいカメラの視点の格納先。
	///			cameraCollisionSolver.Execute(
	///			newCameraPosition,		//第一引数は衝突解決後の視点の座標の格納先。
	///			cameraPosition,			//第二引数は衝突解決前の視点の座標。
	///			cameraTarget,			//第三引数は注視点の座標。
	///			//衝突解決した座標をカメラの座標として設定する。
	///			g_camera3D->SetPosition(newCameraPosition);
	///		}
	///
	///		return 0;
	///}
	/// </summary>
	class CameraCollisionSolver : public Noncopyable
	{
	public:
		/// <summary>
		/// コンストラクタ。
		/// </summary>
		CameraCollisionSolver();
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~CameraCollisionSolver();
		/// <summary>
		/// 初期化。
		/// </summary>
		/// <param name="radius">カメラのバウンディングスフィアの半径。</param>
		void Init(float radius);
		/// <summary>
		/// コリジョン解決の実行。
		/// </summary>
		/// <param name="result">コリジョン解決を行う視点の座標が格納される。</param>
		/// <param name="position">コリジョン解決を行う前の視点の座標。</param>
		/// <param name="target">カメラの注視点。</param>
		/// <returns>コリジョン解決を行った場合はtrueが返ってくる。</returns>
		bool Execute(Vector3& result, const Vector3& position, const Vector3& target);
	private:
		SphereCollider		m_collider;			//コライダー。
		float				m_radius = 0.0f;	//半径。
		bool				m_isInited = false;	//初期化済みのフラグ。trueになってたら初期化済み。
	};
}
