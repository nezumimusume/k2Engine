#pragma once

#include "graphics/Bloom.h"
#include "graphics/Dof.h"
#include "graphics/Fxaa.h"
#include "graphics/ToneMap.h"
#include "graphics/Ssao.h"
#include "graphics/Ssr.h"

/// <summary>
/// ポストエフェクト。
/// </summary>
class PostEffect : public Noncopyable
{
public:
    /// <summary>
    /// 初期化。
    /// </summary>
    /// <param name="mainRenderTarget">レンダターゲット。</param>
    /// <param name="zprepassRenderTarget">zプリパスレンダ―ターゲット。</param>
    /// <param name="normalRenderTarget">法線が書き込まれているレンダ―ターゲット。</param>
    void Init(
        RenderTarget& mainRenderTarget, 
        RenderTarget& zprepassRenderTarget,
        RenderTarget& normalRenderTarget,
        RenderTarget& metallicSmoothRenderTarget,
        RenderTarget& albedoRenderTarget
    );
    /// <summary>
    /// 描画。
    /// </summary>
    /// <param name="rc">レンダ―コンテキスト。</param>
    /// <param name="mainRenderTarget">メインレンダ―ターゲット。</param>
    void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
    /// <summary>
    /// シーンが切り替わったことを通知する。
    /// </summary>
    /// <remark>
    /// シーンの連続性を利用したグラフィック処理のための関数です。
    /// 明暗順応、TAA等の処理は、シーンが切り替わったときにこの関数を呼び出さないと
    /// 数フレームの間、表現が多少おかしくなります。
    /// </remark>
    /// <param name="changeSceneTime">シーン切り替えにかかる時間。</param>
    void NotifyChangeScene(float changeSceneTime)
    {
        m_tonemap.NotifyChangeScene(changeSceneTime);
    }
private:
    Bloom m_bloom;	    //ブルーム
    Dof m_dof;		    //被写界深度
    Fxaa m_fXaa;        //FXAA。
    ToneMap m_tonemap;  //トーンマップ。
    Ssao m_ssao;        //SSAO(スクリーンスペースアンビエントオクルージョン)。
    Ssr m_ssr;          //SSR(スクリーンスペースリフレクション)。
};

