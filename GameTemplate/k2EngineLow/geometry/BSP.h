
#pragma once

#include "geometry/AABB.h"

namespace nsK2EngineLow {

	/// <summary>
	/// BSP
	/// </summary>
	
	class BSP {
	private:
		/// <summary>
		/// 平面。
		/// </summary>
		struct SPlane {
			Vector3 normal;		// 平面の法線
			float distance;		// 平面までの距離。
		};
	public:
		enum EnCompositeType {
			enCompositeType_Node,
			enCompositeType_Leaf,
			enCompositeType_LeafList
		};
		struct IComposite {
			int type = enCompositeType_Node;
		};

		using ICompositePtr = std::shared_ptr<IComposite>;
		/// <summary>
		/// ノード
		/// </summary>
		struct SNode : public IComposite {
			SPlane plane;		// 分割平面。
			ICompositePtr rightNode;	// 右のノード。
			ICompositePtr leftNode;	// 左のノード。
		};
		/// <summary>
		/// リーフ
		/// </summary>
		struct SLeaf : public IComposite {
			Vector3 position;
			void* extraData;
		};
		/// <summary>
		/// リーフのリスト
		/// </summary>
		struct SLeafList : public IComposite {
			std::vector< ICompositePtr> leafList;
		};
	private:
		
		ICompositePtr m_rootNode = nullptr;			// ルートノード。
		std::vector<ICompositePtr> m_leafNodeArray;	// リーフノードの配列。
	public:
		/// <summary>
		/// リーフを追加。
		/// </summary>
		/// <param name="aabb"></param>
		void AddLeaf(const Vector3& position, void* extraData) 
		{
			// リーフノードを作る。
			ICompositePtr leafNode = std::make_shared<SLeaf>();
			m_leafNodeArray.emplace_back(leafNode);
			SLeaf* leaf = static_cast<SLeaf*>(leafNode.get());
			leaf->position = position;
			leaf->extraData = extraData;
			leaf->type = enCompositeType_Leaf;
			
		}
		/// <summary>
		/// BVHを構築。
		/// </summary>
		/// <remark></remark>
		void Build();
		/// <summary>
		/// BSPツリーを探索する
		/// </summary>
		/// <remark>
		/// BSPツリーを探索して、枝に到達すると、引数で指定されたコールバック関数が呼ばれます。
		/// </remark>
		/// <param name="pos">座標</param>
		/// <param name="onEndWalk">探索が終了した時に呼ばれる処理</param>
		void WalkTree(const Vector3& pos, std::function<void(IComposite* leaf)> onEndWalk);
	private:
		/// <summary>
		/// 平面でリーフノードを分割していく
		/// </summary>
		void SplitLeafArray(
			std::vector<ICompositePtr>& leftLeafNodeArray,
			std::vector<ICompositePtr>& rightLeafNodeArray,
			const SPlane& plane, 
			const std::vector<ICompositePtr>& leafNodeArray
		);
		/// <summary>
		/// 共分散行列から分割平面を計算する。
		/// </summary>
		/// <param name="plane"></param>
		/// <param name="covarianceMatrix"></param>
		void CalcSplitPlaneFromCovarianceMatrix(
			SPlane& plane,
			float covarianceMatrix[3][3],
			const Vector3& centerPos
		);
		/// <summary>
		/// リーフノードの配列から共分散行列を計算する。
		/// </summary>
		/// <param name="covarianceMatrix">共分散行列の計算先</param>
		/// <param name="leafNodeArray">リーフノードの配列</param>
		/// <param name="centerPos">リーフノードの中心座標</param>
		void CalcCovarianceMatrixFromLeafNodeList(
			float covarianceMatrix[3][3], 
			const std::vector<ICompositePtr>& leafNodeArray, 
			const Vector3& centerPos
		);
		/// <summary>
		/// リーフのリストから中心座標を計算する。
		/// </summary>
		/// <param name="leafNodeArray"></param>
		/// <returns></returns>
		Vector3 CalcCenterPositionFromLeafList(
			const std::vector<ICompositePtr>& leafNodeArray
		);
		/// <summary>
		/// BVHを構築。
		/// </summary>
		/// <param name="leafNodeArray"></param>
		ICompositePtr BuildInternal(const std::vector<ICompositePtr>& leafNodeArray);
		
	
		void WalkTree(ICompositePtr node, const Vector3& pos, std::function<void(IComposite* leaf)> onEndWalk );
	};
}