#pragma once

#include "MyRenderer.h"

class RenderingEngine;
/// <summary>
/// スキンモデルレンダー。
/// </summary>
class ModelRender
{
public:
	ModelRender() {}
	/// <summary>
	/// 初期化。
	/// ディファードレンダリング。
	/// </summary>
	/// <param name="filePath">ファイルパス。</param>
	/// <param name="animationClips">アニメーションクリップ。</param>
	/// <param name="numAnimationClips">アニメーションクリップの数。</param>
	/// <param name="">モデルの上方向。</param>
	/// <param name="">trueなら影が落ちる。</param>
	void Init(const char* filePath,
		AnimationClip* animationClips = nullptr,
		int numAnimationClips = 0,
		EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
		bool isShadowReciever = true);
	/// <summary>
/// 初期化。
/// フォワードレンダリング。
/// </summary>
/// <param name="filePath">ファイルパス。</param>
/// <param name="animationClips">アニメーションクリップ。</param>
/// <param name="numAnimationClips">アニメーションクリップの数。</param>
/// <param name="">モデルの上方向。</param>
/// <param name="">trueなら影が落ちる。</param>
	void InitForwardRendering(const char* filePath,
		AnimationClip* animationClips = nullptr,
		int numAnimationClips = 0,
		EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
		bool isShadowReciever = true);
	/// <summary>
	/// 更新処理。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理。
	/// </summary>
	void Draw();

	/// <summary>
	/// アニメーション再生。
	/// </summary>
	/// <param name="animNo">アニメーションクリップの番号。</param>
	/// <param name="interpolateTime">補完時間(単位：秒。)</param>
	void PlayAnimation(int animNo, float interpolateTime = 0.0f)
	{
		m_animation.Play(animNo, interpolateTime);
	}

	/// <summary>
	/// アニメーションの再生中？
	/// </summary>
	bool IsPlayingAnimation() const
	{
		return m_animation.IsPlaying();
	}

	/// <summary>
	/// モデルを取得。
	/// </summary>
	/// <returns>モデル</returns>
	Model& GetModel()
	{
		if (m_renderToGBufferModel.IsInited()) {
			return m_renderToGBufferModel;
		}
		if (m_forwardRenderModel.IsInited()) {
			return m_forwardRenderModel;
		}
	}

	/// <summary>
	/// 座標、回転、拡大を全て更新。
	/// </summary>
	/// <param name="pos">座標。</param>
	/// <param name="rotation">回転。</param>
	/// <param name="scale">拡大。</param>
	void SetTRS(const Vector3& pos, const Quaternion& rotation, const Vector3& scale)
	{
		SetPosition(pos);
		SetRotation(rotation);
		SetScale(scale);
	}

	/// <summary>
	/// 座標を設定。
	/// </summary>
	/// <param name="pos">座標。</param>
	void SetPosition(const Vector3& pos)
	{
		m_position = pos;
	}

	/// <summary>
	/// 回転を設定。
	/// </summary>
	/// <param name="rotation">回転。</param>
	void SetRotation(const Quaternion& rotation)
	{
		m_rotation = rotation;
	}

	/// <summary>
	/// 大きさ。
	/// </summary>
	/// <param name="scale">大きさ。</param>
	void SetScale(const Vector3& scale)
	{
		m_scale = scale;
	}
	/// <summary>
/// シャドウキャスターのフラグを設定する
/// </summary>
	void SetShadowCasterFlag(bool flag)
	{
		m_isShadowCaster = flag;
	}
private:
	/// <summary>
	/// モデルの初期化。
	/// </summary>
	/// <param name="filePath">ファイルパス。</param>
	/// <param name="enModelUpAxis">モデルの上向き。</param>
	void InitModel(const char* filePath, EnModelUpAxis enModelUpAxis);
	/// <summary>
	/// スケルトンの初期化。
	/// </summary>
	/// <param name="filePath">ファイルパス。</param>
	void InitSkeleton(const char* filePath);
	/// <summary>
	/// アニメーションの初期化。
	/// </summary>
	/// <param name="animationClips">アニメーションクリップ。</param>
	/// <param name="numAnimationClips">アニメーションクリップの数。</param>
	/// <param name="enModelUpAxis">モデルの上向き。</param>
	void InitAnimation(AnimationClip* animationClips,
		int numAnimationClips, 
		EnModelUpAxis enModelUpAxis);
private:
	/// <summary>
	 /// 共通の初期化処理
	 /// </summary>
	 /// <param name="renderingEngine">レンダリングエンジン</param>
	 /// <param name="tkmFilePath">tkmファイルパス</param>
	void InitCommon(RenderingEngine& renderingEngine, const char* tkmFilePath);
private:
	AnimationClip*				m_animationClips = nullptr;			//アニメーションクリップ。
	int							m_numAnimationClips = 0;			//アニメーションクリップの数。
	Vector3 					m_position = Vector3::Zero;			//座標。
	Quaternion	 				m_rotation = Quaternion::Identity;	//回転。
	Vector3						m_scale = Vector3::One;				//拡大率。
	EnModelUpAxis				m_enFbxUpAxis = enModelUpAxisZ;		//FBXの上方向。
	Animation					m_animation;						//アニメーション。
	Model m_zprepassModel;					//ZPrepassで描画されるモデル
	Model m_forwardRenderModel;				//フォワードレンダリングの描画パスで描画されるモデル
	Model m_renderToGBufferModel;			//RenderToGBufferで描画されるモデル
	Model m_shadowModels[NUM_DEFERRED_LIGHTING_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP];	//シャドウマップに描画するモデル
	bool						m_isUpdateAnimation = true;			//アニメーションを更新する？
	Skeleton					m_skeleton;							//骨。
	bool						m_isShadowCaster = true;			//シャドウキャスターフラグ
};

