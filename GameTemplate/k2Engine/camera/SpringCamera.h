#pragma once
#include "CameraCollisionSolver.h"

namespace nsK2Engine {
	/// <summary>
	/// バネカメラ。
	/// バネに引っ張られるようなカメラの挙動を実現するクラスです。
	/// SetTarget関数とSetPosition関数を使用して、
	/// カメラの目標となる注視点と視点を設定してください。
	/// Update関数を実行することで、カメラが設定された目標座標に追従していきます。
	/// </summary>
	class SpringCamera : public Noncopyable
	{
	public:
		/// <summary>
		/// コンストラクタ。
		/// </summary>
		SpringCamera();
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~SpringCamera();

		/// <summary>
		/// 初期化。
		/// </summary>
		/// <param name="camera">カメラ。</param>
		/// <param name="maxMoveSpeed">カメラの最大速度。</param>
		/// <param name="isEnableCollisionSolver">コリジョン処理を有効にする?</param>
		/// <param name="sphereCollisionRadius">球体のコリジョンの半径。isEnableCollisionSoloverがtrueの時に有効になります。</param>
		void Init(
			Camera& camera,
			float maxMoveSpeed,
			bool isEnableCollisionSolver,
			float sphereCollisionRadius
		);
		/// <summary>
		/// 目標となる注視点を設定。
		/// </summary>
		/// <param name="target">注視点。</param>
		void SetTarget(const Vector3& target)
		{
			m_target = target;
		}
		/// <summary>
		/// 目標となる視点を設定。
		/// </summary>
		/// <param name="position">視点。</param>
		void SetPosition(const Vector3& position)
		{
			m_position = position;
		}
		/// <summary>
		/// 遠平面を設定。
		/// </summary>
		/// <param name="_far">遠平面。</param>
		void SetFar(float _far)
		{
			if (m_camera == nullptr) {
				return;
			}
			m_camera->SetFar(_far);
		}
		/// <summary>
		/// 近平面。
		/// </summary>
		/// <param name="_near">近平面。</param>
		void SetNear(float _near)
		{
			if (m_camera == nullptr) {
				return;
			}
			m_camera->SetNear(_near);
		}
		/// <summary>
		/// カメラの回転行列を取得。
		/// </summary>
		/// <returns>カメラの回転行列。</returns>
		const Matrix& GetCameraRotation() const
		{
			if (m_camera == nullptr) {
				return Matrix::Identity;
			}
			return m_camera->GetCameraRotation();
		}
		/// <summary>
		/// 目標となる注視点を取得。
		/// この関数が返してくる注視点は、実際の注視点ではないので注意して下さい。
		///	実際の注視点がほしい場合はGetRealTargetを使用してください。
		/// </summary>
		/// <returns>注視点。</returns>
		const Vector3& GetTarget() const
		{
			return m_target;
		}
		/// <summary>
		/// 目標となる視点を取得。
		/// この関数が返してくる視点は、実際の視点ではないので注意してください。
		///	実際の始点がほしい場合はGetRealPositionを使用して下さい。
		/// </summary>
		/// <returns>視点。</returns>
		const Vector3& GetPosition() const
		{
			return m_position;
		}
		/// <summary>
		/// 実際の注視点を取得。
		/// </summary>
		/// <returns>注視点。</returns>
		const Vector3& GetRealTarget() const
		{
			if (m_camera == nullptr) {
				return Vector3::Zero;
			}
			return m_camera->GetTarget();

		}
		/// <summary>
		/// 実際の視点を取得。
		/// </summary>
		/// <returns>視点。</returns>
		const Vector3& GetRealPosition() const
		{
			if (m_camera == nullptr) {
				return Vector3::Zero;
			}
			return m_camera->GetPosition();
		}
		/// <summary>
		/// 更新。
		/// </summary>
		void Update();
		/// <summary>
		/// カメラを取得。
		/// </summary>
		/// <returns>カメラ。</returns>
		const Camera* GetCamera() const
		{
			return m_camera;
		}
		/// <summary>
		/// バネの減衰率を設定。
		/// 値が大きいほどカメラが遅れて付いてきます。
		/// </summary>
		/// <param name="dampingRate">バネの減衰率。</param>
		void SetDampingRate(float dampingRate)
		{
			m_targetDampingRate = dampingRate;
		}
		/// <summary>
		/// カメラを取得。
		/// </summary>
		/// <returns></returns>
		Camera* GetCamera()
		{
			return m_camera;
		}
		/// <summary>
		/// 画角を設定。
		/// </summary>
		/// <param name="angle">画角。</param>
		void SetViewAngle(float angle)
		{
			if (m_camera == nullptr) {
				return;
			}
			m_camera->SetViewAngle(angle);
		}
		/// <summary>
		/// 画角を取得。
		/// </summary>
		/// <returns>画角。</returns>
		float GetViewAngle() const
		{
			if (m_camera == nullptr) {
				return 0.0f;
			}
			return m_camera->GetViewAngle();
		}
		/// <summary>
		/// リフレッシュ。
		/// この関数を呼び出すと、バネの移動がリフレッシュされて、Update関数を呼び出すとカメラの位置が
		///	即座に目標視点と目標注視点に移動します。シーンが切り替わってカメラを一気に切り替えたいときなどに
		/// 使用してください。
		/// </summary>
		void Refresh()
		{
			m_targetMoveSpeed = Vector3::Zero;
			m_positionMoveSpeed = Vector3::Zero;
			m_isRefresh = true;
		}
		/// <summary>
		/// バネカメラの更新処理。
		/// Update関数で呼ばれています。
		/// バネカメラの更新処理だけ個別で呼びたいなどの特殊なケースで使用してください。
		/// </summary>
		void UpdateSpringCamera();
		/// <summary>
		/// カメラの更新。
		/// Update関数から呼ばれています。
		/// カメラの更新処理だけ個別で呼びたいなどの特殊なケースで使用してください。
		/// </summary>
		void UpdateCamera()
		{
			if (m_camera) {
				m_camera->Update();
			}
		}
	private:
		Camera* m_camera = nullptr;						//カメラ。
		Vector3		m_target = Vector3::Zero;				//目標となる注視点。
		Vector3		m_position = Vector3::Zero;				//目標となる視点。
		Vector3		m_targetMoveSpeed = Vector3::Zero;		//注視点の移動速度。
		Vector3		m_positionMoveSpeed = Vector3::Zero;	//視点の移動速度。
		float		m_maxMoveSpeed = 0.0f;					//最高移動速度。
		float		m_targetDampingRate = 1.0f;				//減衰率。値が大きいほどカメラが遅れ付いてくる。
		float		m_dampingRate = 1.0f;					//減衰率。
		float		m_dampingRateVel = 0.0f;
		bool		m_isEnableCollisionSolver = false;		//コリジョン処理が有効？
		bool		m_isRefresh = true;						//リフレッシュが必要？
		CameraCollisionSolver m_cameraCollisionSolver;
	};
}