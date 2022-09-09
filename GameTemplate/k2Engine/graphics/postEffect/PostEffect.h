#pragma once

#include "graphics/postEffect/Bloom.h"
#include "graphics/postEffect/Dof.h"
#include "graphics/postEffect/Fxaa.h"
#include "graphics/postEffect/ToneMap.h"
#include "graphics/postEffect/Ssao.h"
#include "graphics/postEffect/Ssr.h"
#include "graphics/postEffect/CalcSceneLuminance.h"

namespace nsK2Engine {
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
            m_calsSceneLuminance.NotifyChangeScene(changeSceneTime);
        }
        /// <summary>
        /// トーンマップをオフに。
        /// </summary>
        void DisableTonemap()
        {
            m_tonemap.Disable();
        }
        /// <summary>
        /// トーンマップをオンに。
        /// </summary>
        void EnableTonemap()
        {
            m_tonemap.Enable();
        }
        /// <summary>
        /// トーンマップが有効か判定する。
        /// </summary>
        /// <returns></returns>
        bool IsEnableTonemap() const
        {
            return m_tonemap.IsEnable();
        }
        /// <summary>
        /// シーンの明るさの基準値を設定
        /// </summary>
        void SetTonemapMiddlegray(float middlegray)
        {
            m_tonemap.SetMiddlegray(middlegray);
        }
        /// <summary>
        /// シーンの明るさの基準値を取得。
        /// </summary>
        /// <returns></returns>
        float GetTonemapMiddlegray() const
        {
            return m_tonemap.GetMiddlegray();
        }
        /// <summary>
        /// ブルームが発生する閾値を設定。
        /// </summary>
        /// <param name="value"></param>
        void SetBloomThreshold(float value)
        {
            m_bloom.SetThreshold(value);
        }
        /// <summary>
        /// ブルームが発生する閾値を取得。
        /// </summary>
        /// <returns></returns>
        float GetBloomThreshold() const
        {
            return m_bloom.GetThreshold();
        }
        /// <summary>
        /// シーンの平均輝度が記憶されているテクスチャを取得。
        /// </summary>
        /// <returns></returns>
        Texture& GetLuminanceAvgTextureInScene()
        {
            return m_calsSceneLuminance.GetLuminanceAvgTextureInScene();
        }
    private:
        CalcSceneLuminance m_calsSceneLuminance;    // シーンの平均輝度計算処理。
        Bloom m_bloom;	    //ブルーム
        Dof m_dof;		    //被写界深度
        Fxaa m_fXaa;        //FXAA。
        ToneMap m_tonemap;  //トーンマップ。
        Ssao m_ssao;        //SSAO(スクリーンスペースアンビエントオクルージョン)。
        Ssr m_ssr;          //SSR(スクリーンスペースリフレクション)。
    };
}
