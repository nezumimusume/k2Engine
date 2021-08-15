#pragma once
#include "LinearMath/btIDebugDraw.h"
#include "graphics/Shader.h"

namespace nsK2EngineLow {
	class DebugWireframe :
		public btIDebugDraw, public Noncopyable
	{
	public:
		DebugWireframe();
		~DebugWireframe();
		/// <summary>
		/// 初期化。	
		/// </summary>
		void Init();
		/// <summary>
		/// drawLine()を呼ぶ前に行う処理。
		/// </summary>
		void Begin()
		{
			m_vertexList.clear();
		}
		/// <summary>
		/// drawLine()を呼んだ後に行う処理。
		/// </summary>
		void End(RenderContext& rc);
		/// 必須。
		/// <summary>
		/// 線ごとに一回ずつ呼ばれる。
		/// </summary>
		/// <param name="from">1つ目の頂点の座標。</param>
		/// <param name="to">2つ目の頂点の座標。</param>
		/// <param name="color">色。</param>
		void    drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		void    setDebugMode(int debugMode) override {};
		int     getDebugMode() const override
		{
			return true;
		};

		//何もしなくても問題なし {}　必要であれば定義
		void    drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {};
		void    reportErrorWarning(const char* warningString) override {};
		void    draw3dText(const btVector3& location, const char* textString) override {};
	private:
		/// <summary>
		/// ルートシグネチャの初期化。
		/// </summary>
		void InitRootSignature();
		/// <summary>
		/// シェーダーの初期化。
		/// </summary>
		void InitSharder();
		/// <summary>
		/// パイプラインステートの初期化。
		/// </summary>
		void InitPipelineState();
		/// <summary>
		/// 頂点バッファの初期化。
		/// </summary>
		void InitVertexBuffer();
		/// <summary>
		/// インデックスバッファの初期化。
		/// </summary>
		void InitIndexBuffer();
		/// <summary>
		/// 定数バッファの初期化。
		/// </summary>
		void InitConstantBuffer();
		/// <summary>
		/// ディスクリプタヒープの初期化。
		/// </summary>
		void InitDescriptorHeap();
		/// <summary>
		/// 頂点バッファの更新。
		/// </summary>
		void VertexBufferUpdate(const btVector3& from, const btVector3& to, const btVector3& color);
		/// <summary>
		/// 定数バッファの更新。
		/// </summary>
		void ConstantBufferUpdate();
		/// <summary>
		/// レンダーコンテキストの更新。
		/// </summary>
		void RenderContextUpdate(RenderContext& rc);
	private:
		struct Vertex
		{
			Vector3 pos;
			Vector3 color;
		};
		std::vector<Vertex>	m_vertexList;				//描画する頂点のリスト。
		ConstantBuffer		m_constantBuffer;			//定数バッファ。
		VertexBuffer		m_vertexBuffer;				//頂点バッファ。
		IndexBuffer			m_indexBuffer;				//インデックスバッファ。
		RootSignature		m_rootSignature;			//ルートシグネチャ。
		Shader				m_Vshader;					//頂点シェーダー。
		Shader				m_Pshader;					//ピクセルシェーダー。
		PipelineState		m_pipelineState;			//パイプラインステート。
		DescriptorHeap		m_descriptorHeap;			//ディスクリプタヒープ。	
		static const int	MAX_VERTEX = 10000000;		//頂点の最大数。
	};
}

