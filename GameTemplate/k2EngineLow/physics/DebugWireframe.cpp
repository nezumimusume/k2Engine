#include "k2EngineLowPreCompile.h"
#include "DebugWireframe.h"

namespace nsK2EngineLow {
	DebugWireframe::DebugWireframe()
	{
	}

	DebugWireframe::~DebugWireframe()
	{

	}

	void DebugWireframe::Init()
	{
		if (g_graphicsEngine == nullptr) {
			return;
		}
		//ルートシグネチャを初期化。
		InitRootSignature();
		//シェーダーを初期化。
		InitSharder();
		//パイプラインステートを初期化。
		InitPipelineState();
		//最大長点数分のメモリは先に確保しておく。
		m_vertexList.reserve(MAX_VERTEX);
		//頂点バッファの初期化。
		InitVertexBuffer();
		//インデックスバッファの初期化。
		InitIndexBuffer();
		//定数バッファを初期化。
		InitConstantBuffer();
		//ディスクリプタヒープを初期化。
		InitDescriptorHeap();
	}

	void DebugWireframe::InitRootSignature()
	{
		m_rootSignature.Init(
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP
		);
	}

	void DebugWireframe::InitSharder()
	{
		m_Vshader.LoadVS("Assets/shader/debugWireFrame.fx", "VSMain");
		m_Pshader.LoadPS("Assets/shader/debugWireFrame.fx", "PSMain");
	}

	void DebugWireframe::InitPipelineState()
	{
		// 頂点レイアウトを定義する。
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		//パイプラインステートを作成。
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0 };
		psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_Vshader.GetCompiledBlob());
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_Pshader.GetCompiledBlob());
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		//psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
		psoDesc.NumRenderTargets = 3;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;		//アルベドカラー出力用。
		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		psoDesc.SampleDesc.Count = 1;
		m_pipelineState.Init(psoDesc);
	}

	void DebugWireframe::InitVertexBuffer()
	{
		//頂点バッファを作成。
		m_vertexBuffer.Init(sizeof(Vertex) * MAX_VERTEX, sizeof(Vertex));
	}

	void DebugWireframe::InitIndexBuffer()
	{
		//インデックスバッファの初期化。
		m_indexBuffer.Init(sizeof(std::uint16_t) * MAX_VERTEX, sizeof(std::uint16_t));
		static std::uint16_t indices[MAX_VERTEX];
		for (int i = 0; i < MAX_VERTEX; i++) {
			indices[i] = i;
		}
		m_indexBuffer.Copy(indices, 0, 0, 0);
	}

	void DebugWireframe::InitConstantBuffer()
	{
		m_constantBuffer.Init(sizeof(Matrix));
	}

	void DebugWireframe::InitDescriptorHeap()
	{
		//ディスクリプタヒープにディスクリプタを登録していく。
		m_descriptorHeap.RegistConstantBuffer(0, m_constantBuffer);
		//ディスクリプタヒープへの登録を確定させる。
		m_descriptorHeap.Commit();
	}

	void DebugWireframe::VertexBufferUpdate(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		//頂点を書き換える。
		Vertex vers[2];
		vers[0].pos = Vector3(from.x(), from.y(), from.z());
		vers[0].color = Vector3(color.x(), color.y(), color.z());
		vers[1].pos = Vector3(to.x(), to.y(), to.z());
		vers[1].color = Vector3(color.x(), color.y(), color.z());

		m_vertexList.push_back(vers[0]);
		m_vertexList.push_back(vers[1]);

		if (m_vertexList.size() > MAX_VERTEX) {
			//描画できるラインは50万本まで。増やしたければMAX_VERTEXを増やしてください。
			std::abort();
		}
	}

	void DebugWireframe::ConstantBufferUpdate()
	{
		//定数バッファの更新(c++)。
		//mVPの更新(シェーダー)。
		Matrix VP;		//ビュー行列とプロジェクション行列
		//ビューとプロジェクションの掛け算
		Matrix v = g_camera3D->GetViewMatrix();
		Matrix p = g_camera3D->GetProjectionMatrix();
		VP.Multiply(v, p);
		//定数バッファに渡したい変数を格納(m_constantBufferの内容を上書き)
		m_constantBuffer.CopyToVRAM(&VP);
	}

	void DebugWireframe::RenderContextUpdate(RenderContext& rc)
	{
		//ルートシグネチャを設定。
		rc.SetRootSignature(m_rootSignature);
		//頂点をどんな感じに描画するのか。
		//パイプラインステートの設定。
		rc.SetPipelineState(m_pipelineState);
		//今回は頂点二つの間に線を描く設定。
		rc.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		//頂点バッファを設定。
		rc.SetVertexBuffer(m_vertexBuffer);
		//インデックスバッファを設定。
		rc.SetIndexBuffer(m_indexBuffer);
		//ディスクリプタヒープに設定。
		rc.SetDescriptorHeap(m_descriptorHeap);
		//ドローコール。
		rc.DrawIndexed(static_cast<UINT>(m_vertexList.size()));
	}

	//1フレーム内にdrawLineは線の数だけ行う
	void DebugWireframe::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		//頂点バッファの更新。
		VertexBufferUpdate(from, to, color);
	}

	void DebugWireframe::End(RenderContext& rc)
	{
		if (m_vertexList.size() == 0)
		{
			return;
		}
		//頂点をコピー。
		m_vertexBuffer.Copy(&m_vertexList.front());

		ConstantBufferUpdate();

		RenderContextUpdate(rc);
	}
}