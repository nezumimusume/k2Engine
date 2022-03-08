#pragma once


#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include "DirectXTK/Inc/DDSTextureLoader.h"
#include "DirectXTK/Inc/ResourceUploadBatch.h"
#include "RenderContext.h"
#include "rayTracing/RaytracingEngine.h"
#include "Camera.h"
#include "NullTextureMaps.h"
#include "font/FontEngine.h"
#include "FrameBuffer.h"
#include <pix.h>

namespace nsK2EngineLow {
	/// <summary>
	/// モデルの上方向。
	/// </summary>
	enum EnModelUpAxis {
		enModelUpAxisY,		//モデルの上方向がY軸。
		enModelUpAxisZ,		//モデルの上方向がZ軸。
	};
	/// <summary>
	/// アルファブレンディングモード
	/// </summary>
	enum AlphaBlendMode {
		AlphaBlendMode_None,	//アルファブレンディングなし(上書き)。
		AlphaBlendMode_Trans,	//半透明合成
		AlphaBlendMode_Add,		//加算合成
		AlphaBlendMode_Multiply	//乗算合成
	};

	/// <summary>
	/// DirectX12に依存するグラフィックスエンジン
	/// </summary>
	class GraphicsEngine : public Noncopyable {
	public:
		/// <summary>
		/// デストラクタ。
		/// </summary>
		~GraphicsEngine();
		/// <summary>
		/// 初期化
		/// </summary>
		/// <remarks>
		/// 本関数を呼び出すことでDirectX12の初期化が行われます。
		/// </remarks>
		/// <param name="hwnd">Windowハンドル</param>
		/// <param name="frameBufferwidth">フレームバッファの幅</param>
		/// <param name="frameBufferHeight">フレームバッファの高さ</param>
		/// <param name="raytracingInitData">レイトレの初期化情報</param>
		/// <returns>falseが返ってきたら作成に失敗。</returns>
		bool Init(
			HWND hwnd, 
			UINT frameBufferWidth, 
			UINT frameBufferHeight,
			const raytracing::InitData& raytracingInitData
		);
		/// <summary>
		/// レンダリング開始。
		/// </summary>
		/// <remarks>
		/// 1フレームのレンダリングの開始時に呼び出してください。
		/// </remarks>
		void BeginRender();
		/// <summary>
		/// レンダリング終了。
		/// </summary>
		/// <remarks>
		/// 1フレームのレンダリングの終了時に呼び出してください。
		/// </remarks>
		void EndRender();
		/// <summary>
		/// D3Dデバイスを取得。
		/// </summary>
		/// <returns></returns>
		ID3D12Device5* GetD3DDevice()
		{
			return m_d3dDevice;
		}
		/// <summary>
		/// バックバッファの番号を取得。
		/// </summary>
		/// <returns>バックバッファの番号。</returns>
		UINT GetBackBufferIndex() const
		{
			return m_frameIndex;
		}
		/// <summary>
		/// コマンドキューを取得。
		/// </summary>
		/// <returns></returns>
		ID3D12CommandQueue* GetCommandQueue() const
		{
			return m_commandQueue;
		}
		/// <summary>
		/// コマンドリストを取得。
		/// </summary>
		/// <returns></returns>
		ID3D12GraphicsCommandList4* GetCommandList() const
		{
			return m_commandList[m_frameIndex];
		}
		/// <summary>
		/// CBR_SRVのディスクリプタのサイズを取得。
		/// </summary>
		/// <returns></returns>
		UINT GetCbrSrvDescriptorSize() const
		{
			return m_cbrSrvDescriptorSize;
		}
		/// <summary>
		/// サンプラのディスクリプタヒープサイズを取得。
		/// </summary>
		/// <returns></returns>
		UINT GetSapmerDescriptorSize() const
		{
			return m_samplerDescriptorSize;
		}
		/// <summary>
		/// レンダリングコンテキストを取得。
		/// </summary>
		/// <returns></returns>
		RenderContext& GetRenderContext()
		{
			return m_renderContext;
		}
		/// <summary>
		/// フレームバッファの幅を取得。
		/// </summary>
		/// <returns>フレームバッファの幅。</returns>
		UINT GetFrameBufferWidth() const
		{
			return m_frameBufferWidth;
		}
		/// <summary>
		/// フレームバッファの高さを取得。
		/// </summary>
		/// <returns>フレームバッファの高さ。</returns>
		UINT GetFrameBufferHeight() const
		{
			return m_frameBufferHeight;
		}
		/// <summary>
		/// レンダリングターゲットをフレームバッファに変更する。
		/// </summary>
		/// <param name="rc"></param>
		void ChangeRenderTargetToFrameBuffer(RenderContext& rc);
		/// <summary>
		/// 現在のフレームバッファのレンダリングターゲットビューを取得。
		/// </summary>
		/// <returns></returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameBuffuerRTV() const
		{
			return m_frameBuffer.GetCurrentRenderTargetViewDescriptorHandle();
		}
		/// <summary>
		/// フレームバッファへの描画時に使用されているデプスステンシルビューを取得。
		/// </summary>
		/// <returns></returns>
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameBuffuerDSV() const
		{
			return m_frameBuffer.GetCurrentDepthStencilViewDescriptorHandle();
		}
		/// <summary>
		/// レイトレの結果のテクスチャを取得。
		/// </summary>
		/// <returns></returns>
		Texture& GetRaytracingOutputTexture()
		{
			return m_raytracingEngine.GetOutputTexture();
		}
		/// <summary>
		/// 3DModelをレイトレワールドに登録。
		/// </summary>
		/// <param name="model">追加するモデル</param>
		void RegistModelToRaytracingWorld(Model& model)
		{
			if (m_isPossibleRaytracing) {
				// ハードウェアレイトレーシングがサポートされている場合のみ
				m_raytracingEngine.RegistGeometry(model);
			}
		}
		/// <summary>
		/// レイトレワールドに再構築リクエストを送る。
		/// </summary>
		void RequestRebuildRaytracingWorld()
		{
			if (m_isPossibleRaytracing) {
				m_raytracingEngine.RequestRebuildRaytracingWorld();
			}
		}
		/// <summary>
		/// 3Dモデルをレイトレワールドから削除。
		/// </summary>
		/// <param name="model">削除するモデル</param>
		void RemoveModelFromRaytracingWorld(Model& model)
		{
			if (m_isPossibleRaytracing) {
				// ハードウェアレイトレーシングがサポートされている場合のみ。
				m_raytracingEngine.RemoveGeometry(model);
			}
		}
		/// <summary>
		/// レイトレーシングをディスパッチ。
		/// </summary>
		/// <param name="rc"></param>
		void DispatchRaytracing(RenderContext& rc)
		{
			if (m_isPossibleRaytracing) {
				// ハードウェアレイトレーシングがサポートされている場合のみ
				m_raytracingEngine.Dispatch(rc);
			}
		}
		
		/// <summary>
		/// レイトレ用のスカイキューブボックスを設定。
		/// </summary>
		/// <param name="skycubeBox"></param>
		void SetRaytracingSkyCubeBox(Texture& skycubeBox)
		{
			m_raytracingEngine.SetSkyCubeBox(skycubeBox);
		}
		/// <summary>
		/// フレームバッファにコピー。
		/// </summary>
		/// <param name="pDst"></param>
		void CopyToFrameBuffer(RenderContext& rc, ID3D12Resource* pSrc)
		{
			D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				m_frameBuffer.GetCurrentRenderTarget(),
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_COPY_DEST);
			rc.ResourceBarrier(barrier);
			rc.CopyResource(m_frameBuffer.GetCurrentRenderTarget(), pSrc);

			D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
				m_frameBuffer.GetCurrentRenderTarget(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				D3D12_RESOURCE_STATE_RENDER_TARGET);
			rc.ResourceBarrier(barrier2);
		}
		/// <summary>
		/// ヌルテクスチャマップを取得。
		/// </summary>
		/// <returns></returns>
		const NullTextureMaps& GetNullTextureMaps() const
		{
			return m_nullTextureMaps;
		}
		/// <summary>
		/// フォントエンジンを取得。
		/// </summary>
		/// <returns></returns>
		FontEngine& GetFontEngine()
		{
			return m_fontEngine;
		}
		/// <summary>
		/// フレームバッファに描画するときのビューポートを取得。
		/// </summary>
		/// <returns></returns>
		D3D12_VIEWPORT& GetFrameBufferViewport()
		{
			return m_frameBuffer.GetViewport();
		}
		/// <summary>
		/// ID3D12Objectの解放。
		/// </summary>
		/// <param name="res"></param>
		void ReleaseD3D12Object(IUnknown* res)
		{
			if(res == nullptr){
				return;
			}
			// D3Dオブジェクトは解放までに1フレームの時間をかける。
			// 描画コマンドは１フレーム遅れて実行されるように実装されているため、即座に開放すると描画中に
			// リソースが解放されてしまう。そのため、１フレーム遅延して開放する必要がある。
			m_reqDelayRelease3d12ObjectList.push_back({ res, 1 });
		}
		/// <summary>
		/// レイトレーシングを行うことが可能か判定。
		/// </summary>
		/// <returns>trueが返ってきたらレイトレを行える。</returns>
		bool IsPossibleRaytracing() const 
		{
			return m_isPossibleRaytracing;
		}
#ifdef K2_DEBUG
		void BeginGPUEvent(const char* eventName)
		{
			PIXBeginEvent(m_commandList[m_frameIndex], 0xffffffffffffffff, eventName);
		}
		void EndGPUEvent()
		{
			PIXEndEvent(m_commandList[m_frameIndex]);
		}
#else
		void BeginGPUEvent(const char*)
		{

		}
		void EndGPUEvent()
		{
		}
#endif
	private:
		/// <summary>
		/// D3Dデバイスの作成。
		/// </summary>
		/// <returns>trueが返ってきたら作成に成功。</returns>
		bool CreateD3DDevice(IDXGIFactory4* dxgiFactory);
		/// <summary>
		/// コマンドキューの作成。
		/// </summary>
		/// <returns>trueが返ってきたら作成に成功。</returns>
		bool CreateCommandQueue();
		/// <summary>
		/// DirectX グラフィックス インフラストラクチャーの作成。
		/// </summary>
		/// <remarks>
		/// DirectX グラフィックス インフラストラクチャーは
		/// カーネル モード ドライバーおよびシステム ハードウェアと通信するためのインターフェースです。 
		/// </remarks>
		/// <returns>作成されたDXGI</returns>
		IDXGIFactory4* CreateDXGIFactory();
		/// <summary>
		/// フレームバッファ用のディスクリプタヒープを作成。
		/// </summary>
		/// <returns>trueが返ってきたら作成に成功。</returns>
		bool CreateDescriptorHeapForFrameBuffer();
		/// <summary>
		/// フレームバッファ用のレンダリングターゲットビューを作成。
		/// </summary>
		/// <returns>trueが返ってきたら作成に成功。</returns>
		bool CreateRTVForFameBuffer();
		/// <summary>
		/// フレームバッファ用の深度ステンシルビューを作成。
		/// </summary>
		/// <param name="frameBufferWidth">フレームバッファの幅</param>
		/// <param name="frameBufferHeight">フレームバッファの高さ</param>
		/// <returns>trueが返ってきたら作成に成功。</returns>
		bool CreateDSVForFrameBuffer(UINT frameBufferWidth, UINT frameBufferHeight);
		/// <summary>
		/// コマンドリストの作成。
		/// </summary>
		/// <returns>trueが返ってきたら作成に成功。</returns>
		bool CreateCommandList();
		/// <summary>
		/// GPUとの同期オブジェクト作成
		/// </summary>
		/// <returns>trueが返ってきたら作成に成功。</returns>
		bool CreateSynchronizationWithGPUObject();
		/// <summary>
		/// 描画の完了待ち。
		/// </summary>
		void WaitDraw();
		/// <summary>
		/// ID3D12Objectの解放リクエストを処理する。
		/// </summary>
		void ExecuteRequestReleaseD3D12Object();
	private:
		//GPUベンダー定義。
		enum GPU_Vender {
			GPU_VenderNvidia,	//NVIDIA
			GPU_VenderAMD,		//Intel
			GPU_VenderIntel,	//AMD
			Num_GPUVender,
		};
		/// <summary>
		/// D3D12オブジェクトの遅延リリースリクエスト
		/// </summary>
		struct RequestDelayReleaseD3D12Object
		{
			IUnknown* d3dObject;	// リリースするD3Dオブジェクト
			int delayTime;				// 遅延フレーム。この値は毎フレームデクリメントされ、0になると解放されます。
		};
		ID3D12Device5* m_d3dDevice = nullptr;							//D3Dデバイス。
		ID3D12CommandQueue* m_commandQueue = nullptr;					// コマンドキュー。
		ID3D12CommandAllocator* m_commandAllocator[2] = { nullptr };	//コマンドアロケータ。
		ID3D12GraphicsCommandList4* m_commandList[2] = { nullptr };		//コマンドリスト。
		ID3D12PipelineState* m_pipelineState = nullptr;					//パイプラインステート。
		UINT m_cbrSrvDescriptorSize = 0;								//CBR_SRVのディスクリプタのサイズ。
		UINT m_samplerDescriptorSize = 0;								//サンプラのディスクリプタのサイズ。			
		RenderContext m_renderContext;									//レンダリングコンテキスト。
		FrameBuffer m_frameBuffer;										//フレームバッファ
		
		// GPUとの同期で使用する変数。
		UINT m_frameIndex = 0;
		HANDLE m_fenceEvent = nullptr;
		ID3D12Fence* m_fence = nullptr;
		UINT64 m_fenceValue = 0;
		UINT m_frameBufferWidth = 0;				// フレームバッファの幅。
		UINT m_frameBufferHeight = 0;				// フレームバッファの高さ。
		Camera m_camera2D;							// 2Dカメラ。
		Camera m_camera3D;							// 3Dカメラ。
		raytracing::Engine m_raytracingEngine;		// レイトレエンジン。
		NullTextureMaps m_nullTextureMaps;			// ヌルテクスチャマップ。
		FontEngine m_fontEngine;					// フォントエンジン。
		std::unique_ptr<DirectX::GraphicsMemory> m_directXTKGfxMemroy;					//DirectXTKのグラフィックメモリシステム。
		bool m_isExecuteCommandList = false;											//コマンドリストをGPUに流した？
		std::list< RequestDelayReleaseD3D12Object > m_reqDelayRelease3d12ObjectList;	// D3D12オブジェクトの遅延解放リクエストのリスト。
		bool m_isPossibleRaytracing = false;		// レイトレーシングを行うことが可能？
	};
	extern GraphicsEngine* g_graphicsEngine;	//グラフィックスエンジン
	extern Camera* g_camera2D;					//2Dカメラ。
	extern Camera* g_camera3D;					//3Dカメラ。

	/// <summary>
	/// D3D12オブジェクトを解放。
	/// </summary>
	/// <param name="obj">開放したいオブジェクト</param>
	template < class TD3D12Obj > 
	static inline void ReleaseD3D12Object(TD3D12Obj*& obj)
	{
		if (obj == nullptr) {
			return;
		}
		if (g_graphicsEngine) {
			g_graphicsEngine->ReleaseD3D12Object(obj);
		}
		else {
			obj->Release();
		}
		obj = nullptr;
	}
	static inline void BeginGPUEvent(const char* eventName)
	{
		g_graphicsEngine->BeginGPUEvent(eventName);
		
	}
	static inline void EndGPUEvent()
	{
		g_graphicsEngine->EndGPUEvent();
	}

}

