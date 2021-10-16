#include "k2EngineLowPreCompile.h"
#include "geometry/BSP.h"

namespace nsK2EngineLow {
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
            int i, j;

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
    Vector3 BSP::CalcCenterPositionFromLeafList(const std::vector<ICompositePtr>& leafNodeArray)
    {
        // まずは、AABBの中心座標を求める。
        Vector3 centerPos;
        for (const auto& leafPtr : leafNodeArray) {
            auto leaf = static_cast<SLeaf*>(leafPtr.get());
            centerPos += leaf->position;
        }
        centerPos /= static_cast<float>(leafNodeArray.size());
        return centerPos;
    }
    void BSP::CalcCovarianceMatrixFromLeafNodeList(
        float covarianceMatrix[3][3],
        const std::vector<ICompositePtr>& leafNodeArray,
        const Vector3& centerPos
    )
    {
        // 0で初期化する。
        memset(covarianceMatrix, 0, sizeof(float[3][3]));
        
        for (const auto& leafPtr : leafNodeArray) {
            auto leaf = static_cast<SLeaf*>(leafPtr.get());
            const auto& aabbCenterPos = leaf->position;
            covarianceMatrix[0][0] += (centerPos.x - aabbCenterPos.x) * (centerPos.x - aabbCenterPos.x);
            covarianceMatrix[0][1] += (centerPos.x - aabbCenterPos.x) * (centerPos.y - aabbCenterPos.y);
            covarianceMatrix[1][0] = covarianceMatrix[0][1];

            covarianceMatrix[1][1] += (centerPos.y - aabbCenterPos.y) * (centerPos.y - aabbCenterPos.y);
            covarianceMatrix[0][2] += (centerPos.x - aabbCenterPos.x) * (centerPos.z - aabbCenterPos.z);
            covarianceMatrix[2][0] = covarianceMatrix[0][2];

            covarianceMatrix[2][2] += (centerPos.z - aabbCenterPos.z) * (centerPos.z - aabbCenterPos.z);
            covarianceMatrix[1][2] += (centerPos.y - aabbCenterPos.y) * (centerPos.z - aabbCenterPos.z);
            covarianceMatrix[2][1] = covarianceMatrix[0][2];
        }

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                covarianceMatrix[i][j] /= static_cast<float>(m_leafNodeArray.size());
            }
        }
    }
    void BSP::CalcSplitPlaneFromCovarianceMatrix(
        SPlane& plane,
        float covarianceMatrix[3][3],
        const Vector3& centerPos
    )
    {
        // 共分散行列が計算できたので、ヤコビ法を用いて固有値と固有ベクトルを求める。
        Vector3 eigenVector[3];
        EigenJacobiMethod<3>(
            reinterpret_cast<float*>(covarianceMatrix),
            reinterpret_cast<float*>(&eigenVector)
            );
        // 1番目大きな固有値の固有ベクトルを分割平面の法線とする。

        float eigenScalar_0 = covarianceMatrix[0][0];
        float eigenScalar_1 = covarianceMatrix[1][1];
        float eigenScalar_2 = covarianceMatrix[2][2];

        if (eigenScalar_0 > eigenScalar_1 && eigenScalar_0 > eigenScalar_2) {
            plane.normal = eigenVector[0];
        }
        else if (eigenScalar_1 > eigenScalar_0 && eigenScalar_1 > eigenScalar_2) {
            plane.normal = eigenVector[1];
        }
        else if (eigenScalar_2 > eigenScalar_0 && eigenScalar_2 > eigenScalar_1) {
            plane.normal = eigenVector[2];
        }

        // 分割平面までの距離は中心座標までの距離とする。
        plane.distance = Dot(plane.normal, centerPos);
    }
    void BSP::SplitLeafArray(
        std::vector<ICompositePtr>& leftLeafNodeArray,
        std::vector<ICompositePtr>& rightLeafNodeArray,
        const SPlane& plane,
        const std::vector<ICompositePtr>& leafNodeArray
    )
    {
        for (const auto& leafPtr : leafNodeArray) {
            auto leaf = static_cast<SLeaf*>(leafPtr.get());
            float t = Dot(leaf->position, plane.normal);

            if (t < plane.distance) {
                // 左側に割り振る。
                leftLeafNodeArray.emplace_back(leafPtr);
            }
            else {
                // 右側に割り振る。
                rightLeafNodeArray.emplace_back(leafPtr);
            }

        }
    }
    BSP::ICompositePtr BSP::BuildInternal(const std::vector<ICompositePtr>& leafNodeArray)
    {
        if (leafNodeArray.size() == 1) {
            // リーフノード
            return leafNodeArray.front();
        }

        // 主成分分析を行って、分割平面を求める。
        // まずは
        Vector3 centerPos = CalcCenterPositionFromLeafList(leafNodeArray);

        // 続いて共分散行列を計算する
        float covarianceMatrix[3][3];
        CalcCovarianceMatrixFromLeafNodeList(covarianceMatrix, leafNodeArray, centerPos);
        
        // 各共分散の要素を引っ張ってくる。
        Vector3* v_0 = (Vector3*)covarianceMatrix[0];
        Vector3* v_1 = (Vector3*)covarianceMatrix[1];
        Vector3* v_2 = (Vector3*)covarianceMatrix[2];
        
        if (v_0->Length() < 0.1f && v_1->Length() < 0.1f && v_2->Length() < 0.1f) {
            // 分散していないということは、ほとんどのリーフが非常に近い場所にあるということなので、
            // これ以上の分割は行わない。
            // BSPの末端ノードとして、リーフの配列ノードを作成する。
            auto newNodePtr = std::make_shared<SLeafList>();
            auto leafList = static_cast<SLeafList*>(newNodePtr.get());
            leafList->type = enCompositeType_LeafList;
            leafList->leafList = leafNodeArray;
            return newNodePtr;
        }
        
        // 新しいノードを作る。
        ICompositePtr newNodePtr;
        newNodePtr = std::make_shared<SNode>();
        newNodePtr->type = enCompositeType_Node;
        auto newNode = static_cast<SNode*>(newNodePtr.get());

        // 分散しているので、共分散行列を利用して
        // 分割平面を計算する。
        CalcSplitPlaneFromCovarianceMatrix(newNode->plane, covarianceMatrix, centerPos);
       
        // 分割平面が求まったので、リーフノードを平面で振り分けしていく。
        std::vector<ICompositePtr> leftLeafNodeArray;
        std::vector<ICompositePtr> rightLeafNodeArray;
        SplitLeafArray(leftLeafNodeArray, rightLeafNodeArray, newNode->plane, leafNodeArray);
        
        
        if (leftLeafNodeArray.empty() || rightLeafNodeArray.empty()) {
            // 片方の枝が空になった。
            // 分散しているので、こには来ないはずなんだけど、万が一来てしまうと、再起呼び出しが終わらずに
            // スタックオーバーフローしてしまうので、保険として。
            // 分散していないのでリーフのリストノードを作成する。
            newNodePtr = std::make_shared<SLeafList>();
            auto leafList = static_cast<SLeafList*>(newNodePtr.get());
            leafList->type = enCompositeType_LeafList;
            leafList->leafList = leafNodeArray;
            return newNodePtr;
        }
        
        // 左の枝を構築
        newNode->leftNode = BuildInternal(leftLeafNodeArray);
        
        // 右の枝を構築。
        newNode->rightNode = BuildInternal(rightLeafNodeArray);
        
        
        return newNodePtr;
    }
	/// <summary>
	/// BVHを構築。
	/// </summary>
	/// <remark></remark>
	void BSP::Build()
	{
		// ルートノードを作成。
        m_rootNode = BuildInternal(m_leafNodeArray); std::make_shared<SNode>();
	}
}