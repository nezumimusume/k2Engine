/*!
 *@brief	ゲームオブジェクトの基底クラス。
 */

#pragma once

#include <list>
#include <string>
namespace nsK2EngineLow {
	class RenderContext;

	/*!
		*@brief	ゲームオブジェクト。
		*/
	class IGameObject : public Noncopyable {
	public:
		/*!
			*@brief	デストラクタ
			*/
		virtual ~IGameObject()
		{
		}
	public:


		/*!
		*@brief	Updateの直前で呼ばれる開始処理。
		*@details
		*
		* 本関数がtrueを返すとゲームオブジェクトの準備が完了したと判断されて</br>
		* Update関数が呼ばれ出します。trueを返して以降はStart関数は呼ばれなくなります。</br>
		* ゲームオブジェクトの初期化に複数フレームかかる場合などはfalseを返して、初期化ステップなどを使って</br>
		* 適切に初期化を行ってください。
		*/
		virtual bool Start() { return true; }
		/*!
			*@brief	更新
			*/
		virtual void Update() {}
		/*!
		 *@brief	描画
		*/
		virtual void Render(RenderContext& renderContext)
		{
			(void)renderContext;
		}

	public:
		/*!
		*@brief Start関数が完了した？
		*/
		bool IsStart() const
		{
			return m_isStart;
		}
		/*!
		*@brief アクティブかどうかを判定。
		*/
		bool IsActive() const
		{
			return m_isActive;
		}
		/// <summary>
		/// ゲームオブジェクトをアクティブにする。
		/// </summary>
		void Activate()
		{
			m_isActive = true;
		}
		/// <summary>
		/// ゲームオブジェクトを非アクティブにする。
		/// </summary>
		void Deactivate()
		{
			m_isActive = false;
		}

		/// <summary>
		/// 死亡させる。
		/// </summary>
		void Dead()
		{
			m_isDead = true;
		}
		/// <summary>
		/// 死亡している？
		/// </summary>
		/// <returns>trueが返ってきたら死亡している</returns>
		bool IsDead() const
		{
			return m_isDead;
		}
		/// <summary>
		/// ゲームオブジェクトの名前を設定。
		/// </summary>
		/// <param name="name">名前</param>
		void SetName(const char* name)
		{
			if (name != nullptr) {
				m_name = name;
			}
		}
		/// <summary>
		/// 引数で渡された名前が、このゲームオブジェクトの名前とマッチするか判定。
		/// </summary>
		/// <param name="name"></param>
		/// <returns></returns>
		bool IsMatchName(const char* name) const
		{
			if (strcmp(m_name.c_str(), name) == 0) {
				return true;
			}
			return false;
		}
	public:

		void RenderWrapper(RenderContext& renderContext)
		{
			if (m_isActive && m_isStart && !m_isDead) {
				Render(renderContext);
			}
		}

		void UpdateWrapper()
		{
			if (m_isActive && m_isStart && !m_isDead) {
				Update();
			}
		}
		void StartWrapper()
		{
			if (m_isActive && !m_isStart && !m_isDead) {
				if (Start()) {
					//初期化処理完了。
					m_isStart = true;
				}
			}
		}
		friend class CGameObjectManager;
	protected:
		std::string m_name = "default";					//ゲームオブジェクトの名前
		bool m_isStart = false;							//Startの開始フラグ。
		bool m_isDead = false;							//死亡フラグ。
		bool m_isRegistDeadList = false;				//死亡リストに積まれている。
		bool m_isNewFromGameObjectManager;	//GameObjectManagerでnewされた。
		bool m_isRegist = false;							//GameObjectManagerに登録されている？
		bool m_isActive = true;							//Activeフラグ。
	};
}