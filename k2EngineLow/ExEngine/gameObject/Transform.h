/*!
 *@brief	Transform
 */

#pragma once

namespace tkEngine{
	/*!
	 *@brief	Transform
	 *@details
	 * UnityのTransformと同様の使い方ができる。
	 */
	class CTransform{
	public:
		CTransform()
		{
		}
		~CTransform()
		{
		}
		/*!
		 *@brief	ワールド座標を取得。
		 */
		const CVector3& GetPosition() const
		{
			return m_position;
		}
		/*!
		 *@brief	ワールド回転を取得。
		 */
		const CQuaternion& GetRotation() const
		{
			return m_rotation;
		}
		/*!
		 *@brief	ワールド拡大率を取得。
		 */
		const CVector3& GetScale() const
		{
			return m_scale;
		}
		/*!
		 *@brief	ローカル座標を取得。
		 */
		const CVector3& GetLocalPosition() const
		{
			return m_localPosition;
		}
		/*!
		 *@brief	ローカル回転を取得。
		 */
		const CQuaternion& GetLocalRotation() const
		{
			return m_localRotation;
		}
		/*!
		 *@brief	ローカル拡大を取得。
		 */
		const CVector3& GetLocalScale() const
		{
			return m_localScale;
		}
		/*!
		 *@brief	ローカル座標を設定。
		 */
		void SetLocalPosition(const CVector3& pos)
		{
			m_localPosition = pos;
		}
		/*!
		 *@brief	ローカル回転を設定。
		 */
		void SetLocalRotation(const CQuaternion& rot)
		{
			m_localRotation = rot;
		}
		/*!
		 *@brief	ローカル拡大率を設定。
		 */
		void SetLocalScale(const CVector3& scale)
		{
			m_localScale = scale;
		}
		/*!
		 *@brief	このインスタンスと子供すべてのワールド行列を更新する。
		 */
		void UpdateWorldMatrixAll();
	private:
		CTransform* m_parent = nullptr;							//!<親。
		std::list<CTransform*> m_children;						//!<子供。
		CVector3	m_position = CVector3::Zero;				//!<ワールド座標。
		CQuaternion m_rotation = CQuaternion::Identity;			//!<ワールド回転。
		CVector3	m_scale = CVector3::Zero;					//!<ワールド拡大率。
		CVector3	m_localPosition = CVector3::Zero;			//!<ローカル座標。
		CQuaternion	m_localRotation = CQuaternion::Identity;	//!<ローカル回転。
		CVector3	m_localScale = CVector3::One;				//!<ローカルスケール。
		CMatrix		m_worldMatrix;								//!<ワールド行列。
	};
}