
#include "BSP.h"


/// <summary>
   /// ヤコビ法を用いてn×n行列の固有値と固有ベクトルを求める。
   /// 下記のページのプログラムを利用。
   /// </summary>
   /// http://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?%B8%C7%CD%AD%C3%CD/%B8%C7%CD%AD%A5%D9%A5%AF%A5%C8%A5%E
   /// <param name="a">実対称行列．計算後，対角要素に固有値が入る</param>
   /// <param name="v">固有ベクトル。aと同じサイズ</param>
   /// <param name="eps">収束誤差</param>
   /// <param name="iter_max">反復数の最大数</param>
   /// <returns></returns>
#if 1
#define   TOL     1.0e-10   
#define   N       3
#define   MAX     100
template<int n>
int EigenJacobiMethod(float a[], float x[])
{
	int    i, j, k, m, count, status;
	float amax, amax0, theta, co, si, co2, si2, cosi, pi = 4.0f * atan(1.0f);
	float aii, aij, ajj, aik, ajk;

	//   初期値設定
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (i == j)  x[n * i + j] = 1.0f; else  x[n * i + j] = 0.0f;
		}
	}

	//   反復計算
	count = 0;  status = 9;
	while (count <= MAX) {
		//  非対角要素の最大値を探索
		amax = 0.0;
		for (k = 0; k < n - 1; k++) {
			for (m = k + 1; m < n; m++) {
				amax0 = fabs(a[n * k + m]);
				if (amax0 > amax) { i = k;  j = m, amax = amax0; }
			}
		}
		//  収束判定
		if (amax <= TOL) { status = 0;   break; }
		else {
			aii = a[n * i + i];   aij = a[n * i + j];   ajj = a[n * j + j];
			//   回転角度計算
			if (fabs(aii - ajj) < TOL) {
				theta = 0.25f * pi * aij / fabs(aij);
			}
			else {
				theta = 0.5f * atan(2.0f * aij / (aii - ajj));
			}
			co = cos(theta); si = sin(theta); co2 = co * co; si2 = si * si; cosi = co * si;

			//   相似変換行列
			a[n * i + i] = co2 * aii + 2.0f * cosi * aij + si2 * ajj;
			a[n * j + j] = si2 * aii - 2.0f * cosi * aij + co2 * ajj;
			a[n * i + j] = 0.0f;    a[n * j + i] = 0.0f;
			for (k = 0; k < n; k++) {
				if (k != i && k != j) {
					aik = a[n * k + i];            ajk = a[n * k + j];
					a[n * k + i] = co * aik + si * ajk;  a[n * i + k] = a[n * k + i];
					a[n * k + j] = -si * aik + co * ajk;  a[n * j + k] = a[n * k + j];
				}
			}

			//   固有ベクトル
			for (k = 0; k < n; k++) {
				aik = x[n * k + i];   ajk = x[n * k + j];
				x[n * k + i] = co * aik + si * ajk;
				x[n * k + j] = -si * aik + co * ajk;
			}
			count++;
		}
	}
	return status;
}
#else
template<int n>
int EigenJacobiMethod(float* a, float* v, float eps = 1e-8, int iter_max = 100)
{
	float bim[n];
	float bjm[n];
	float bii, bij, bjj, bji;

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			v[i * n + j] = (i == j) ? 1.0 : 0.0;
		}
	}

	int cnt = 0;
	for (;;) {
		int i = 0;
		int j = 0;

		float x = 0.0;
		for (int ia = 0; ia < n; ++ia) {
			for (int ja = 0; ja < n; ++ja) {
				int idx = ia * n + ja;
				if (ia != ja && fabs(a[idx]) > x) {
					i = ia;
					j = ja;
					x = fabs(a[idx]);
				}
			}
		}

		float aii = a[i * n + i];
		float ajj = a[j * n + j];
		float aij = a[i * n + j];

		float alpha, beta;
		alpha = (aii - ajj) / 2.0;
		beta = sqrt(alpha * alpha + aij * aij);

		float st, ct;
		ct = sqrt((1.0 + fabs(alpha) / beta) / 2.0);    // sinθ
		st = (((aii - ajj) >= 0.0) ? 1.0 : -1.0) * aij / (2.0 * beta * ct);    // cosθ

		// A = PAPの計算
		for (int m = 0; m < n; ++m) {
			if (m == i || m == j) continue;

			float aim = a[i * n + m];
			float ajm = a[j * n + m];

			bim[m] = aim * ct + ajm * st;
			bjm[m] = -aim * st + ajm * ct;
		}

		bii = aii * ct * ct + 2.0 * aij * ct * st + ajj * st * st;
		bij = 0.0;

		bjj = aii * st * st - 2.0 * aij * ct * st + ajj * ct * ct;
		bji = 0.0;

		for (int m = 0; m < n; ++m) {
			a[i * n + m] = a[m * n + i] = bim[m];
			a[j * n + m] = a[m * n + j] = bjm[m];
		}
		a[i * n + i] = bii;
		a[i * n + j] = bij;
		a[j * n + j] = bjj;
		a[j * n + i] = bji;

		// V = PVの計算
		for (int m = 0; m < n; ++m) {
			float vmi = v[m * n + i];
			float vmj = v[m * n + j];

			bim[m] = vmi * ct + vmj * st;
			bjm[m] = -vmi * st + vmj * ct;
		}
		for (int m = 0; m < n; ++m) {
			v[m * n + i] = bim[m];
			v[m * n + j] = bjm[m];
		}

		float e = 0.0;
		for (int ja = 0; ja < n; ++ja) {
			for (int ia = 0; ia < n; ++ia) {
				if (ia != ja) {
					e += fabs(a[ja * n + ia]);
				}
			}
		}
		if (e < eps) break;

		cnt++;
		if (cnt > iter_max) break;
	}

	return cnt;
}
#endif
Vector3 BSP::CalcCenterPositionFromLeafList(const std::vector<SEntityPtr>& leafArray)
{
	// まずは、AABBの中心座標を求める。
	Vector3 centerPos;
	for (const auto& leafPtr : leafArray) {
		auto leaf = static_cast<SLeaf*>(leafPtr.get());
		centerPos += leaf->position;
	}
	centerPos /= static_cast<float>(leafArray.size());
	return centerPos;
}
void BSP::CalcCovarianceMatrixFromLeafNodeList(
	float covarianceMatrix[3][3],
	const std::vector<SEntityPtr>& leafNodeArray,
	const Vector3& centerPos
)
{
	// 0で初期化する。
	memset(covarianceMatrix, 0, sizeof(float[3][3]));
	// 共分散行列を計算する。
	// 共分散とはXとYとZの要素がどれくらい関連づいて分散しているかを表すもの。
	// 共分散行列は、それを行列としてまとめたもの。
	for (const auto& leafPtr : leafNodeArray) {
		auto leaf = static_cast<SLeaf*>(leafPtr.get());
		const auto& aabbCenterPos = leaf->position;
		covarianceMatrix[0][0] += (aabbCenterPos.x - centerPos.x) * (aabbCenterPos.x - centerPos.x);
		covarianceMatrix[0][1] += (aabbCenterPos.x - centerPos.x) * (aabbCenterPos.y - centerPos.y);
		covarianceMatrix[1][0] = covarianceMatrix[0][1];

		covarianceMatrix[1][1] += (aabbCenterPos.y - centerPos.y) * (aabbCenterPos.y - centerPos.y);
		covarianceMatrix[0][2] += (aabbCenterPos.x - centerPos.x) * (aabbCenterPos.z - centerPos.z);
		covarianceMatrix[2][0] = covarianceMatrix[0][2];

		covarianceMatrix[2][2] += (aabbCenterPos.z - centerPos.z) * (aabbCenterPos.z - centerPos.z);
		covarianceMatrix[1][2] += (aabbCenterPos.y - centerPos.y) * (aabbCenterPos.z - centerPos.z);
		covarianceMatrix[2][1] = covarianceMatrix[1][2];
	}

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			covarianceMatrix[i][j] /= static_cast<float>(m_leafArray.size());
		}
	}
}
void BSP::CalcSplitPlaneFromCovarianceMatrix(
	SPlane& plane,
	float covarianceMatrix[3][3],
	const Vector3& centerPos,
	const std::vector<SEntityPtr>& leafArray
)
{

	// 共分散行列が計算できたので、ヤコビ法を用いて固有値と固有ベクトルを求める。
	Vector3 eigenVector[3];
	EigenJacobiMethod<3>(
		reinterpret_cast<float*>(covarianceMatrix),
		reinterpret_cast<float*>(&eigenVector)
		);

	// 1番目大きな固有値の固有ベクトルを分割平面の法線とする。
	float eigenScalar_0 = fabsf(covarianceMatrix[0][0]);
	float eigenScalar_1 = fabsf(covarianceMatrix[1][1]);
	float eigenScalar_2 = fabsf(covarianceMatrix[2][2]);

	if (eigenScalar_0 > eigenScalar_1 && eigenScalar_0 > eigenScalar_2) {
		plane.normal.x = eigenVector[0].x;
		plane.normal.y = eigenVector[1].x;
		plane.normal.z = eigenVector[2].x;
	}
	else if (eigenScalar_1 > eigenScalar_0 && eigenScalar_1 > eigenScalar_2) {
		plane.normal.x = eigenVector[0].y;
		plane.normal.y = eigenVector[1].y;
		plane.normal.z = eigenVector[2].y;
	}
	else if (eigenScalar_2 > eigenScalar_0 && eigenScalar_2 > eigenScalar_1) {
		plane.normal.x = eigenVector[0].z;
		plane.normal.y = eigenVector[1].z;
		plane.normal.z = eigenVector[2].z;
	}
	if (plane.normal.Length() < 0.1f) {
		// ヤコビ法で法線が計算できなかった。

		SLeaf* leafFront = static_cast<SLeaf*>(leafArray.front().get());
		SLeaf* leafBack = static_cast<SLeaf*>(leafArray.back().get());

		plane.normal = leafBack->position - leafFront->position;

		plane.normal.Normalize();
	}
	// 分割平面までの距離は中心座標までの距離とする。
	plane.distance = Dot(plane.normal, centerPos);
	if (plane.distance < 0.0f) {
		plane.normal *= -1.0f;
		plane.distance = fabsf(plane.distance);
	}
}
void BSP::SplitLeafArray(
	std::vector<SEntityPtr>& leftLeafArray,
	std::vector<SEntityPtr>& rightLeafArray,
	const SPlane& plane,
	const std::vector<SEntityPtr>& leafArray
)
{
	for (const auto& leafPtr : leafArray) {
		auto leaf = static_cast<SLeaf*>(leafPtr.get());
		float t = Dot(leaf->position, plane.normal);

		if (t < plane.distance) {
			// 左側に割り振る。
			leftLeafArray.emplace_back(leafPtr);
		}
		else {
			// 右側に割り振る。
			rightLeafArray.emplace_back(leafPtr);
		}

	}
}
BSP::SEntityPtr BSP::CreateBSPTreeEntity_LeafList(const std::vector<SEntityPtr>& leafArray)
{
	auto newNodePtr = std::make_shared<SLeafList>();
	auto leafList = static_cast<SLeafList*>(newNodePtr.get());
	leafList->type = enEntityType_LeafList;
	leafList->leafList = leafArray;
	return newNodePtr;
}
BSP::SEntityPtr BSP::CreateBSPTreeEntity(const std::vector<SEntityPtr>& leafArray)
{
	if (leafArray.size() == 1) {
		// リーフエンティティを返す。
		return leafArray.front();
	}

	// 主成分分析を行って、分割平面を求める。
	// まずは、リーフノードの中心座標を計算する。
	Vector3 centerPos = CalcCenterPositionFromLeafList(leafArray);

	// 続いて共分散行列を計算する
	float covarianceMatrix[3][3];
	CalcCovarianceMatrixFromLeafNodeList(covarianceMatrix, leafArray, centerPos);

	// 各共分散の要素を引っ張ってくる。
	Vector3* v_0 = (Vector3*)covarianceMatrix[0];
	Vector3* v_1 = (Vector3*)covarianceMatrix[1];
	Vector3* v_2 = (Vector3*)covarianceMatrix[2];

	if (v_0->Length() < 0.1f && v_1->Length() < 0.1f && v_2->Length() < 0.1f) {
		// 分散していないということは、ほとんどのリーフが非常に近い場所にあるということなので、
		// これ以上の分割は行わない。
		// BSPの末端ノードとして、リーフの配列ノードを作成する。
		return CreateBSPTreeEntity_LeafList(leafArray);
	}

	// 新しいノードを作る。
	auto newNodePtr = std::make_shared<SNode>();
	newNodePtr->type = enEntityType_Node;
	newNodePtr->centerPos = centerPos;
	newNodePtr->leafArray = leafArray;
	auto newNode = static_cast<SNode*>(newNodePtr.get());

	// 分散しているので、共分散行列を利用して
	// 分割平面を計算する。
	CalcSplitPlaneFromCovarianceMatrix(newNode->plane, covarianceMatrix, centerPos, leafArray);

	// 分割平面が求まったので、リーフを平面で振り分けしていく。
	std::vector<SEntityPtr> leftLeafArray;
	std::vector<SEntityPtr> rightLeafArray;
	SplitLeafArray(leftLeafArray, rightLeafArray, newNode->plane, leafArray);


	if (leftLeafArray.empty() || rightLeafArray.empty()) {
		// 片方の枝が空になった。
		// 分散しているので、こには来ないはずなんだけど、万が一来てしまうと、再起呼び出しが終わらずに
		// スタックオーバーフローしてしまうので、保険として。
		// 分散していないのでリーフのリストノードを作成する。
		return CreateBSPTreeEntity_LeafList(leafArray);
	}

	// 左の枝を構築
	newNode->leftEntity = CreateBSPTreeEntity(leftLeafArray);

	// 右の枝を構築。
	newNode->rightEntity = CreateBSPTreeEntity(rightLeafArray);


	return newNodePtr;
}
void BSP::WalkTree(SEntityPtr entityPtr, const Vector3& pos, std::function<void(SLeaf* leaf)> onEndWalk) const
{
	if (entityPtr->type == enEntityType_Node) {
		// これはノードなのでさらに潜る。
		// 左に潜る？右に潜る？
		SNode* node = static_cast<SNode*>(entityPtr.get());
		for (SEntityPtr& entity : node->leafArray) {

		}
		float t = Dot(pos, node->plane.normal);
		if (t < node->plane.distance) {
			// 左に潜る。
			WalkTree(node->leftEntity, pos, onEndWalk);
		}
		else {
			// 右に潜る。
			WalkTree(node->rightEntity, pos, onEndWalk);
		}
	}
	else if (entityPtr->type == enEntityType_Leaf) {
		// リーフに到達した。
		onEndWalk(static_cast<SLeaf*>(entityPtr.get()));
	}
	else if (entityPtr->type == enEntityType_LeafList) {
		// リーフのリストに到達した。
		auto leafList = static_cast<SLeafList*>(entityPtr.get());
		for (auto leaf : leafList->leafList) {
			onEndWalk(static_cast<SLeaf*>(leaf.get()));
		}
	}
}
void BSP::WalkTree(const Vector3& pos, std::function<void(SLeaf* leaf)> onEndWalk) const
{
	WalkTree(m_rootNode, pos, onEndWalk);
}
void BSP::Build()
{
	// ルートノードを作成。
	m_rootNode = CreateBSPTreeEntity(m_leafArray);
}