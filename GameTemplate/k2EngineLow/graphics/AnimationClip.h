#pragma once

#include "tkFile/TkaFile.h"

namespace nsK2EngineLow {
	/// <summary>
	/// キーフレーム
	/// </summary>
	struct KeyFrame {
		uint32_t boneIndex;	//!<ボーンインデックス。
		float time;					//!<時間。
		Matrix transform;			//!<トランスフォーム。
	};

	/// <summary>
	/// アニメーションイベント。
	/// </summary>
	class AnimationEvent : public Noncopyable {
	public:


		/// <summary>
		/// イベント発生時間を設定。
		/// </summary>
		/// <returns></returns>
		float GetInvokeTime() const
		{
			return m_invokeTime;
		}

		/// <summary>
		/// イベント名を取得。
		/// </summary>
		/// <returns></returns>
		const wchar_t* GetEventName() const
		{
			return m_eventName.c_str();
		}

		/// <summary>
		/// イベント発生時間を設定。
		/// </summary>
		/// <param name="time"></param>
		void SetInvokeTime(float time)
		{
			m_invokeTime = time;
		}
		/*!
		*@brief	イベント名を設定。
		*/

		/// <summary>
		/// イベント名を設定
		/// </summary>
		/// <param name="name"></param>
		void SetEventName(const wchar_t* name)
		{
			m_eventName = name;
		}

		/// <summary>
		/// イベントが発生済みか判定
		/// </summary>
		/// <returns></returns>
		bool IsInvoked() const
		{
			return m_isInvoked;
		}

		/// <summary>
		/// イベント発生済みのフラグを設定する。
		/// </summary>
		/// <param name="flag"></param>
		void SetInvokedFlag(bool flag)
		{
			m_isInvoked = flag;
		}
	private:
		bool m_isInvoked = false;	//!<イベント発生済み？
		float m_invokeTime = 0.0f;	//!<イベント発生時間。
		std::wstring m_eventName;	//!<イベント名。
	};

	/// <summary>
	/// アニメーションクリップクラス。
	/// </summary>
	class AnimationClip {
	public:

		using keyFramePtrList = std::vector<KeyFrame*>;
		/// <summary>
		/// コンストラクタ
		/// </summary>
		AnimationClip()
		{
		}
		
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~AnimationClip();
		/// <summary>
		/// アニメーションクリップを同期ロード。
		/// </summary>
		/// <param name="filePath"></param>
		void Load(const char* filePath);

		/// <summary>
		/// キーフレームとアニメーションイベントを構築する。
		/// </summary>
		/// <remarks>
		/// ロードが終わったときに呼び出してください。
		/// </remarks>
		void BuildKeyFramesAndAnimationEvents();
		/*!
		*@brief	ループする？
		*/
		bool IsLoop() const
		{
			return m_isLoop;
		}
		/*!
		*@brief	ループフラグを設定する。
		*/
		void SetLoopFlag(bool flag)
		{
			m_isLoop = flag;
		}
		/*!
		*@brief
		*/
		const std::vector<keyFramePtrList>& GetKeyFramePtrListArray() const
		{
			return m_keyFramePtrListArray;
		}
		const keyFramePtrList& GetTopBoneKeyFrameList() const
		{
			return *m_topBoneKeyFramList;
		}
		
		/// <summary>
		/// クリップ名を取得。
		/// </summary>
		/// <returns></returns>
		const wchar_t* GetName() const
		{
			return m_clipName.c_str();
		}
		
		/// <summary>
		/// アニメーションイベントを取得。
		/// </summary>
		/// <returns></returns>
		std::unique_ptr<AnimationEvent[]>& GetAnimationEvent()
		{
			return m_animationEvent;
		}

		/// <summary>
		/// アニメーションイベントの数を取得
		/// </summary>
		/// <returns></returns>
		int GetNumAnimationEvent() const
		{
			return m_tkaFile->GetNumAnimationEvent();
		}
	private:
		using KeyframePtr = std::unique_ptr<KeyFrame>;
		std::wstring m_clipName;										//!<アニメーションクリップの名前。
		bool m_isLoop = false;											//!<ループフラグ。
		std::vector<KeyframePtr>			m_keyframes;				//キーフレーム。
		std::vector<keyFramePtrList>		m_keyFramePtrListArray;		//ボーンごとのキーフレームのリストを管理するための配列。
		std::unique_ptr<AnimationEvent[]>	m_animationEvent;			//アニメーションイベント。
		int									m_numAnimationEvent = 0;	//アニメーションイベントの数。
		keyFramePtrList* m_topBoneKeyFramList = nullptr;
		TkaFile*							m_tkaFile = nullptr;		//tkaファイル
	};
	using AnimationClipPtr = std::unique_ptr<AnimationClip>;
}