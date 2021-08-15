#include "k2EnginePreCompile.h"
#include "SpringCamera.h"

namespace nsK2Engine {
	namespace
	{
		float dampingK = 35.0f;
		float CalcSpringScalar(
			float positionNow,
			float positionTarget,
			float& moveSpeed)
		{
			float deltaTime = g_gameTime->GetFrameDeltaTime();

			float dampingRate = 0.2f;
			float distance;
			distance = positionTarget - positionNow;
			if (fabsf(distance) < FLT_EPSILON) {
				moveSpeed = 0.0f;
				return positionTarget;
			}
			float originalDir = distance;
			originalDir /= fabsf(distance);
			float springAccel;
			springAccel = distance;

			float t = dampingK / (2.0f * dampingRate);
			float springK = t * t;
			springAccel *= springK;
			//加速度を決定。
			float vt = moveSpeed;
			vt *= dampingK;
			springAccel -= vt;
			springAccel *= deltaTime;
			moveSpeed += springAccel;

			float newPos = positionNow;
			float addPos = moveSpeed;
			addPos *= deltaTime;
			newPos += addPos;
			vt = positionTarget - newPos;
			if (fabsf(vt) < FLT_EPSILON) {
				//目標座標まで移動完了した。
				newPos = positionTarget;
				moveSpeed = 0.0f;
			}
			else {
				vt /= fabsf(vt);
				if (vt * originalDir < 0.0f) {
					//目標座標を超えた。
					newPos = positionTarget;
					moveSpeed = 0.0f;
				}
			}
			return newPos;
		}

		/// <summary>
		/// バネ減衰を使用して、現在の位置、目標となる位置、速度、加速度から新しい位置を計算する。
		/// </summary>
		Vector3 CalcSpringVector(
			const Vector3& positionNow,
			const Vector3& positionTarget,
			Vector3& moveSpeed,
			float maxMoveSpeed,
			float dampingRate
		)
		{
			float deltaTime = min(1.0f / 30.0f, g_gameTime->GetFrameDeltaTime());
			//現在の位置と目標の位置との差分を求める。
			Vector3 distance;
			distance.Subtract(positionTarget, positionNow);
			Vector3 originalDir = distance;
			originalDir.Normalize();
			Vector3 springAccel;
			springAccel = distance;

			float t = dampingK / (2.0f * dampingRate);
			float springK = t * t;
			springAccel.Scale(springK);
			//加速度を決定。
			Vector3 vt = moveSpeed;
			vt.Scale(dampingK);
			springAccel.Subtract(vt);

			springAccel.Scale(deltaTime);
			moveSpeed.Add(springAccel);
			if (moveSpeed.LengthSq() > maxMoveSpeed * maxMoveSpeed) {
				//最高速度より速くなってしまった。
				moveSpeed.Normalize();
				moveSpeed.Scale(maxMoveSpeed);
			}
			Vector3 newPos = positionNow;
			if (moveSpeed.Length() < 1.0f) {
				newPos = positionTarget;
				moveSpeed = Vector3::Zero;
			}
			else {
				Vector3 addPos = moveSpeed;
				addPos.Scale(deltaTime);
				newPos.Add(addPos);
				vt.Subtract(positionTarget, newPos);
				vt.Normalize();
				if (vt.Dot(originalDir) < 0.0f) {
					//目標座標を超えた。
					newPos = positionTarget;
					moveSpeed = Vector3::Zero;
				}
			}
			return newPos;
		}
	}

	SpringCamera::SpringCamera()
	{

	}
	SpringCamera::~SpringCamera()
	{

	}
	void SpringCamera::Init(Camera& camera,
		float maxMoveSpeed,
		bool isEnableCollisionSolver,
		float sphereCollisionRadius)
	{
		m_camera = &camera;
		SetTarget(m_camera->GetTarget());
		SetPosition(m_camera->GetPosition());
		m_isEnableCollisionSolver = isEnableCollisionSolver;
		m_cameraCollisionSolver.Init(sphereCollisionRadius);
		m_targetMoveSpeed = Vector3::Zero;
		m_positionMoveSpeed = Vector3::Zero;
		m_maxMoveSpeed = maxMoveSpeed;
		m_isRefresh = true;
	}
	void SpringCamera::UpdateSpringCamera()
	{
		if (m_camera == nullptr) {
			return;
		}
		if (m_isRefresh) {
			//リフレッシュが必要なら、カメラの座標を一気に目標座標にする。
			//シーンの切り替わり時など、一気にカメラを変更する必要があるときに使用してください。
			m_camera->SetTarget(m_target);
			m_camera->SetPosition(m_position);
			m_isRefresh = false;
		}
		else {
			m_dampingRate = CalcSpringScalar(m_dampingRate, m_targetDampingRate, m_dampingRateVel);
			Vector3 target = CalcSpringVector(m_camera->GetTarget(), m_target, m_targetMoveSpeed, m_maxMoveSpeed, m_dampingRate);
			Vector3 position = CalcSpringVector(m_camera->GetPosition(), m_position, m_positionMoveSpeed, m_maxMoveSpeed, m_dampingRate);
			m_camera->SetTarget(target);
			m_camera->SetPosition(position);
		}
	}
	void SpringCamera::Update()
	{
		UpdateSpringCamera();
		if (m_isEnableCollisionSolver) {
			Vector3 result;
			m_cameraCollisionSolver.Execute(
				result,
				m_camera->GetPosition(),
				m_camera->GetTarget()
			);
			m_camera->SetPosition(result);
		}
		UpdateCamera();
	}
}