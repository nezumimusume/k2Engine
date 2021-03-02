/*!
 * @brief	キャラクタコントローラー。
 */

#pragma once

#include "CapsuleCollider.h"
#include "RigidBody.h"


/*!
	* @brief	キャラクタコントローラー。
	*@details
	* キャラクターと背景の衝突解決を行うことができます。\n
	* 下記のサンプルプログラムを参考にしてください。\n
	* Sample/Sample07
	*/
class CharacterController{
public:
	CharacterController() {

	}
	~CharacterController()
	{
		RemoveRigidBoby();
	}
	/*!
		* @brief	初期化。
		*@param[in]	radius		カプセルコライダーの半径。
		*@param[in]	height		カプセルコライダーの高さ。
		*@param[in]	position	初期位置。
		*/
	void Init(float radius, float height, const Vector3& position);
	/*!
		* @brief	実行。
		*@param[in, out]	moveSpeed		移動速度。
		*@param[in]	deltaTime		経過時間。単位は秒。デフォルトでは、１フレームの経過時間が渡されています。
		*@return 移動後のキャラクターの座標。
		*/
	const Vector3& Execute(Vector3& moveSpeed, float deltaTime);
	/*!
		* @brief	座標を取得。
		*/
	const Vector3& GetPosition() const
	{
		return m_position;
	}
	/*!
		* @brief	座標を設定。
		*/
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	/*!
		* @brief	ジャンプ中か判定
		*/
	bool IsJump() const
	{
		return m_isJump;
	}
	/*!
	* @brief	地面上にいるか判定。
	*/
	bool IsOnGround() const
	{
		return m_isOnGround;
	}
	/*!
	* @brief	コライダーを取得。
	*/
	CCapsuleCollider* GetCollider()
	{
		return &m_collider;
	}
	/*!
	* @brief	剛体を取得。
	*/
	RigidBody* GetRigidBody()
	{
		return &m_rigidBody;
	}
	/*!
	* @brief	剛体を物理エンジンから削除。。
	*/
	void RemoveRigidBoby();
private:
	bool				m_isInited = false;				//!<初期化済み？
	Vector3 			m_position;						//!<座標。
	bool 				m_isJump = false;				//!<ジャンプ中？
	bool				m_isOnGround = true;			//!<地面の上にいる？
	CCapsuleCollider	m_collider;						//!<コライダー。
	float				m_radius = 0.0f;				//!<カプセルコライダーの半径。
	float				m_height = 0.0f;				//!<カプセルコライダーの高さ。
	RigidBody			m_rigidBody;					//剛体。
};
