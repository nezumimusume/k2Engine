
#include "BSP.h"


/// <summary>
   /// ���R�r�@��p����n�~n�s��̌ŗL�l�ƌŗL�x�N�g�������߂�B
   /// ���L�̃y�[�W�̃v���O�����𗘗p�B
   /// </summary>
   /// http://www.slis.tsukuba.ac.jp/~fujisawa.makoto.fu/cgi-bin/wiki/index.php?%B8%C7%CD%AD%C3%CD/%B8%C7%CD%AD%A5%D9%A5%AF%A5%C8%A5%E
   /// <param name="a">���Ώ̍s��D�v�Z��C�Ίp�v�f�ɌŗL�l������</param>
   /// <param name="v">�ŗL�x�N�g���Ba�Ɠ����T�C�Y</param>
   /// <param name="eps">�����덷</param>
   /// <param name="iter_max">�������̍ő吔</param>
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

	//   �����l�ݒ�
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {
			if (i == j)  x[n * i + j] = 1.0f; else  x[n * i + j] = 0.0f;
		}
	}

	//   �����v�Z
	count = 0;  status = 9;
	while (count <= MAX) {
		//  ��Ίp�v�f�̍ő�l��T��
		amax = 0.0;
		for (k = 0; k < n - 1; k++) {
			for (m = k + 1; m < n; m++) {
				amax0 = fabs(a[n * k + m]);
				if (amax0 > amax) { i = k;  j = m, amax = amax0; }
			}
		}
		//  ��������
		if (amax <= TOL) { status = 0;   break; }
		else {
			aii = a[n * i + i];   aij = a[n * i + j];   ajj = a[n * j + j];
			//   ��]�p�x�v�Z
			if (fabs(aii - ajj) < TOL) {
				theta = 0.25f * pi * aij / fabs(aij);
			}
			else {
				theta = 0.5f * atan(2.0f * aij / (aii - ajj));
			}
			co = cos(theta); si = sin(theta); co2 = co * co; si2 = si * si; cosi = co * si;

			//   �����ϊ��s��
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

			//   �ŗL�x�N�g��
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
		ct = sqrt((1.0 + fabs(alpha) / beta) / 2.0);    // sin��
		st = (((aii - ajj) >= 0.0) ? 1.0 : -1.0) * aij / (2.0 * beta * ct);    // cos��

		// A = PAP�̌v�Z
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

		// V = PV�̌v�Z
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
	// �܂��́AAABB�̒��S���W�����߂�B
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
	// 0�ŏ���������B
	memset(covarianceMatrix, 0, sizeof(float[3][3]));
	// �����U�s����v�Z����B
	// �����U�Ƃ�X��Y��Z�̗v�f���ǂꂭ�炢�֘A�Â��ĕ��U���Ă��邩��\�����́B
	// �����U�s��́A������s��Ƃ��Ă܂Ƃ߂����́B
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

	// �����U�s�񂪌v�Z�ł����̂ŁA���R�r�@��p���ČŗL�l�ƌŗL�x�N�g�������߂�B
	Vector3 eigenVector[3];
	EigenJacobiMethod<3>(
		reinterpret_cast<float*>(covarianceMatrix),
		reinterpret_cast<float*>(&eigenVector)
		);

	// 1�Ԗڑ傫�ȌŗL�l�̌ŗL�x�N�g���𕪊����ʂ̖@���Ƃ���B
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
		// ���R�r�@�Ŗ@�����v�Z�ł��Ȃ������B

		SLeaf* leafFront = static_cast<SLeaf*>(leafArray.front().get());
		SLeaf* leafBack = static_cast<SLeaf*>(leafArray.back().get());

		plane.normal = leafBack->position - leafFront->position;

		plane.normal.Normalize();
	}
	// �������ʂ܂ł̋����͒��S���W�܂ł̋����Ƃ���B
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
			// �����Ɋ���U��B
			leftLeafArray.emplace_back(leafPtr);
		}
		else {
			// �E���Ɋ���U��B
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
		// ���[�t�G���e�B�e�B��Ԃ��B
		return leafArray.front();
	}

	// �听�����͂��s���āA�������ʂ����߂�B
	// �܂��́A���[�t�m�[�h�̒��S���W���v�Z����B
	Vector3 centerPos = CalcCenterPositionFromLeafList(leafArray);

	// �����ċ����U�s����v�Z����
	float covarianceMatrix[3][3];
	CalcCovarianceMatrixFromLeafNodeList(covarianceMatrix, leafArray, centerPos);

	// �e�����U�̗v�f�����������Ă���B
	Vector3* v_0 = (Vector3*)covarianceMatrix[0];
	Vector3* v_1 = (Vector3*)covarianceMatrix[1];
	Vector3* v_2 = (Vector3*)covarianceMatrix[2];

	if (v_0->Length() < 0.1f && v_1->Length() < 0.1f && v_2->Length() < 0.1f) {
		// ���U���Ă��Ȃ��Ƃ������Ƃ́A�قƂ�ǂ̃��[�t�����ɋ߂��ꏊ�ɂ���Ƃ������ƂȂ̂ŁA
		// ����ȏ�̕����͍s��Ȃ��B
		// BSP�̖��[�m�[�h�Ƃ��āA���[�t�̔z��m�[�h���쐬����B
		return CreateBSPTreeEntity_LeafList(leafArray);
	}

	// �V�����m�[�h�����B
	auto newNodePtr = std::make_shared<SNode>();
	newNodePtr->type = enEntityType_Node;
	newNodePtr->centerPos = centerPos;
	newNodePtr->leafArray = leafArray;
	auto newNode = static_cast<SNode*>(newNodePtr.get());

	// ���U���Ă���̂ŁA�����U�s��𗘗p����
	// �������ʂ��v�Z����B
	CalcSplitPlaneFromCovarianceMatrix(newNode->plane, covarianceMatrix, centerPos, leafArray);

	// �������ʂ����܂����̂ŁA���[�t�𕽖ʂŐU�蕪�����Ă����B
	std::vector<SEntityPtr> leftLeafArray;
	std::vector<SEntityPtr> rightLeafArray;
	SplitLeafArray(leftLeafArray, rightLeafArray, newNode->plane, leafArray);


	if (leftLeafArray.empty() || rightLeafArray.empty()) {
		// �Е��̎}����ɂȂ����B
		// ���U���Ă���̂ŁA���ɂ͗��Ȃ��͂��Ȃ񂾂��ǁA�����ꗈ�Ă��܂��ƁA�ċN�Ăяo�����I��炸��
		// �X�^�b�N�I�[�o�[�t���[���Ă��܂��̂ŁA�ی��Ƃ��āB
		// ���U���Ă��Ȃ��̂Ń��[�t�̃��X�g�m�[�h���쐬����B
		return CreateBSPTreeEntity_LeafList(leafArray);
	}

	// ���̎}���\�z
	newNode->leftEntity = CreateBSPTreeEntity(leftLeafArray);

	// �E�̎}���\�z�B
	newNode->rightEntity = CreateBSPTreeEntity(rightLeafArray);


	return newNodePtr;
}
void BSP::WalkTree(SEntityPtr entityPtr, const Vector3& pos, std::function<void(SLeaf* leaf)> onEndWalk) const
{
	if (entityPtr->type == enEntityType_Node) {
		// ����̓m�[�h�Ȃ̂ł���ɐ���B
		// ���ɐ���H�E�ɐ���H
		SNode* node = static_cast<SNode*>(entityPtr.get());
		for (SEntityPtr& entity : node->leafArray) {

		}
		float t = Dot(pos, node->plane.normal);
		if (t < node->plane.distance) {
			// ���ɐ���B
			WalkTree(node->leftEntity, pos, onEndWalk);
		}
		else {
			// �E�ɐ���B
			WalkTree(node->rightEntity, pos, onEndWalk);
		}
	}
	else if (entityPtr->type == enEntityType_Leaf) {
		// ���[�t�ɓ��B�����B
		onEndWalk(static_cast<SLeaf*>(entityPtr.get()));
	}
	else if (entityPtr->type == enEntityType_LeafList) {
		// ���[�t�̃��X�g�ɓ��B�����B
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
	// ���[�g�m�[�h���쐬�B
	m_rootNode = CreateBSPTreeEntity(m_leafArray);
}