#pragma once

#include "Bloom.h"
#include "Dof.h"
#include "Fxaa.h"
#include "ToneMap.h"
#include "Ssao.h"

/// <summary>
/// ポストエフェクト。
/// </summary>
class PostEffect
{
public:
    /// <summary>
    /// 初期化。
    /// </summary>
    /// <param name="mainRenderTarget">レンダターゲット。</param>
    /// <param name="zprepassRenderTarget">zプリパスレンダ―ターゲット。</param>
    void Init(RenderTarget& mainRenderTarget, RenderTarget& zprepassRenderTarget);
    /// <summary>
    /// 描画。
    /// </summary>
    /// <param name="rc">レンダ―コンテキスト。</param>
    /// <param name="mainRenderTarget">メインレンダ―ターゲット。</param>
    void Render(RenderContext& rc, RenderTarget& mainRenderTarget);
private:
    Bloom m_bloom;	    //ブルーム
    Dof m_dof;		    //被写界深度
    Fxaa m_fXaa;        //FXAA。
    ToneMap m_tonemap;  //トーンマップ。
    Ssao m_ssao;        //SSAO(スクリーンスペースアンビエントオクルージョン)。
};

