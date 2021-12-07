#pragma once

#include <map>
#include <string>

namespace nsK2EngineLow {
	/// <summary>
	/// エフェクトエンジンクラス。
	/// </summary>
	class EffectEngine : public Noncopyable {
		static EffectEngine* m_instance;	//唯一のインスタンス。
		EffekseerRenderer::RendererRef m_renderer[2];	//レンダラー。
		Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> m_memoryPool[2];	//メモリプール。
		Effekseer::RefPtr<EffekseerRenderer::CommandList> m_commandList[2];			//コマンドリスト。
		Effekseer::ManagerRef m_manager;
		std::map< int, Effekseer::EffectRef > m_effectMap;
	public:
		/// <summary>
		/// インスタンスの作成。
		/// </summary>
		static void CreateInstance()
		{
			if (m_instance == nullptr) {
				m_instance = new EffectEngine;
			}
		}
		/// <summary>
		/// インスタンスの取得。
		/// </summary>
		/// <returns></returns>
		static EffectEngine* GetInstance()
		{
			return m_instance;
		}
		/// <summary>
		/// インスタンスの削除。
		/// </summary>
		static void DeleteInstance()
		{
			delete m_instance;
			m_instance = nullptr;

		}
		/// <summary>
		/// エフェクトが再生中か判定。
		/// </summary>
		/// <param name="handle"></param>
		/// <returns></returns>
		bool IsPlay(int handle) const
		{
			return m_manager->GetShown(handle);
		}
		/// <summary>
		/// エフェクトのワールド行列を更新。
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="pos"></param>
		/// <param name="rot"></param>
		/// <param name="scale"></param>
		void UpdateEffectWorldMatrix(int handle, Vector3 pos, Quaternion rot, Vector3 scale)
		{
			Matrix mTrans, mRot, mScale, mBase;
			mTrans.MakeTranslation(pos);
			mRot.MakeRotationFromQuaternion(rot);
			mScale.MakeScaling(scale);
			mBase = mScale * mRot;
			mBase = mBase * mTrans;

			Effekseer::Matrix43 baseMat;
			baseMat.Value[0][0] = mBase.m[0][0];
			baseMat.Value[0][1] = mBase.m[0][1];
			baseMat.Value[0][2] = mBase.m[0][2];

			baseMat.Value[1][0] = mBase.m[1][0];
			baseMat.Value[1][1] = mBase.m[1][1];
			baseMat.Value[1][2] = mBase.m[1][2];

			baseMat.Value[2][0] = mBase.m[2][0];
			baseMat.Value[2][1] = mBase.m[2][1];
			baseMat.Value[2][2] = mBase.m[2][2];

			baseMat.Value[3][0] = mBase.m[3][0];
			baseMat.Value[3][1] = mBase.m[3][1];
			baseMat.Value[3][2] = mBase.m[3][2];

			m_manager->SetBaseMatrix(handle, baseMat);
		}
		/// <summary>
		/// エフェクトを再生。
		/// </summary>
		/// <param name="filePath">再生したいエフェクト</param>
		/// <returns>再生したエフェクトのハンドル。</returns>
		int Play(Effekseer::EffectRef effect);
		/// <summary>
		/// エフェクトを停止。
		/// </summary>
		/// <param name="effectHandle"></param>
		void Stop(int effectHandle);
		/// <summary>
		/// エフェクトをロード。
		/// </summary>
		Effekseer::EffectRef LoadEffect(const int number);
		/// <summary>
		/// 更新
		/// </summary>
		/// <param name="deltaTime">1フレームの経過時間。</param>
		void Update(float deltaTime);
		/// <summary>
		/// 描画。
		/// </summary>
		void Draw();
		/// <summary>
		/// エフェクトを読み込んで登録する。
		/// </summary>
		/// <param name="number">登録番号。</param>
		/// <param name="filePath">ファイルパス。</param>
		void ResistEffect(const int number, const char16_t* filePath);
		/// <summary>
		/// フレームの開始時に呼び出す必要がある処理。
		/// </summary>
		void BeginFrame();
	private:
		EffectEngine();
		~EffectEngine();
	};
}