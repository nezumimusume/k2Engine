/*!
 *@brief	���x���B
 */

#include "k2EnginePreCompile.h"
#include "LevelRender.h"
#include "MapChipRender.h"

#include "level3D/TklFile.h"


namespace nsK2Engine {

	LevelRender::~LevelRender()
	{

	}
	void LevelRender::CreateMapChipRender(const LevelObjectData& objData, const char* filePath)
	{
		std::string key = filePath;
		//�}�b�v�`�b�v�����_�[�ɂ܂��t�b�N����Ă��Ȃ�������B
		if (m_mapChipRenderPtrs.count(key) == 0)
		{
			//�t�b�N����Ȃ������̂ŁA�}�b�v�`�b�v���쐬����B
			auto mapChipRender = std::make_shared<MapChipRender>(objData, filePath);
			m_mapChipRenderPtrs[key] = mapChipRender;
		}
		else
		{
			auto& mapChipRender = m_mapChipRenderPtrs[key];
			//�}�b�v�`�b�v�f�[�^��ǉ�����B
			mapChipRender->AddMapChipData(objData);
		}
	}

	void LevelRender::Init(
		const char* filePath,
		std::function<bool(LevelObjectData& objData)> hookFunc
	)
	{
		//tkl�t�@�C�������[�h����B
		m_tklFile.Load(filePath);

		MatrixTklToLevel();

		//���[�g�{�[���͂���Ȃ����߁Ai��1����n�߂邱�Ƃŏ��O�ł���B
		for (int i = 1; i < m_bonelist.size(); i++) {
			auto bone = m_bonelist[i].get();
			if (bone->GetParentBoneNo() == 0) {

				//�}�b�v�`�b�v���쐬����B
				//�s�񂩂�|�W�V�������쐬����B
				LevelObjectData levelObjData;
				bone->CalcWorldTRS(levelObjData.position, levelObjData.rotation, levelObjData.scale);

				//Zup��Yup�̕ύX�B
				float fix = levelObjData.position.y;
				levelObjData.position.y = levelObjData.position.z;
				levelObjData.position.z = -fix;

				fix = levelObjData.rotation.y;
				levelObjData.rotation.y = levelObjData.rotation.z;
				levelObjData.rotation.z = -fix;

				std::swap(levelObjData.scale.y, levelObjData.scale.z);
				//Zup��Yup�@�I���B



				levelObjData.name = m_bonelist[i]->GetName();
				std::wstring name = levelObjData.name;
				//@�����邩�ǂ����A��������B
				int pos = static_cast<int>(name.find('@'));
				//@�����݂��Ă���΁A@��̐������擾����B
				if (pos != std::wstring::npos)
				{
					std::wstring number = name.substr(static_cast<size_t>(pos) + 1);
					//wchar�^��int�^�ɕϊ����Ă���B
					levelObjData.number = std::stoi(number.c_str());
				}
				//�p�X�̍쐬
				wchar_t filePath[256];
				swprintf_s(filePath, L"Assets/modelData/%s.tkm", levelObjData.name);
				//�}���`�o�C�g�ɕϊ� wchar�� char
				//�x�[�X�̕�����B
				size_t origsize = wcslen(filePath) + 1;
				//�ϊ�����������
				size_t convertedChars = 0;
				//�}���`�o�C�g�i�[�p
				char strCon[] = "";
				//�T�C�Y
				size_t strConSize = (strlen(strCon) + 1) * 2;
				//�ϊ���̃T�C�Y�B
				const size_t newsize = origsize * 2;
				//�A����̃T�C�Y
				char* cFilePath = new char[newsize + strConSize];
				//�}���`�o�C�g�ɕϊ�����B���肫��Ȃ����̂͐؂肷��
				wcstombs_s(&convertedChars, cFilePath, newsize, filePath, _TRUNCATE);
				//������̘A��
				_mbscat_s((unsigned char*)cFilePath, newsize + strConSize, (unsigned char*)strCon);




				//Hook���o�^�ς݂Ȃ�΃}�b�v�`�b�v�͍쐬�s�v
				//false �̂܂܂Ȃ�쐬����B
				bool isHooked = false;
				//hookFunc�ɂȂɂ������Ă��Ȃ���Sobject�ɂ͉����Ȃ��B
				if (hookFunc != nullptr) {
					isHooked = hookFunc(levelObjData);
				}

				//hook��false�Ȃ܂܂Ȃ�΁B
				if (isHooked == false) {
					//�}�b�v�`�b�v�����_�[���쐬�B
					CreateMapChipRender(levelObjData, cFilePath);
				}

			}
		}

		for (auto& mapChipRender : m_mapChipRenderPtrs)
		{
			//�}�b�v�`�b�v�����_�[���������B
			mapChipRender.second->Init();
		}
	}

	void LevelRender::MatrixTklToLevel()
	{
		m_tklFile.QuaryObject([&](TklFile::SObject& tklObj) {
			//�R�s�[�B
			//�o�C���h�|�[�Y�B
			Matrix bindPoseMatrix;
			memcpy(bindPoseMatrix.m[0], &tklObj.bindPose[0], sizeof(tklObj.bindPose[0]));
			memcpy(bindPoseMatrix.m[1], &tklObj.bindPose[1], sizeof(tklObj.bindPose[1]));
			memcpy(bindPoseMatrix.m[2], &tklObj.bindPose[2], sizeof(tklObj.bindPose[2]));
			memcpy(bindPoseMatrix.m[3], &tklObj.bindPose[3], sizeof(tklObj.bindPose[3]));
			bindPoseMatrix.m[0][3] = 0.0f;
			bindPoseMatrix.m[1][3] = 0.0f;
			bindPoseMatrix.m[2][3] = 0.0f;
			bindPoseMatrix.m[3][3] = 1.0f;

			/// <summary>
			/// �o�C���h�|�[�Y�̋t�s��
			/// ��3��Vector3�ϊ��̍ۂɕs�v�Ȃ̂ŏ������B
			/// </summary>
			Matrix invBindPoseMatirx;
			memcpy(invBindPoseMatirx.m[0], &tklObj.invBindPose[0], sizeof(tklObj.invBindPose[0]));
			memcpy(invBindPoseMatirx.m[1], &tklObj.invBindPose[1], sizeof(tklObj.invBindPose[1]));
			memcpy(invBindPoseMatirx.m[2], &tklObj.invBindPose[2], sizeof(tklObj.invBindPose[2]));
			memcpy(invBindPoseMatirx.m[3], &tklObj.invBindPose[3], sizeof(tklObj.invBindPose[3]));
			invBindPoseMatirx.m[0][3] = 0.0f;
			invBindPoseMatirx.m[1][3] = 0.0f;
			invBindPoseMatirx.m[2][3] = 0.0f;
			invBindPoseMatirx.m[3][3] = 1.0f;

			wchar_t boneName[256];

			/// <summary>
			/// tkl�̃I�u�W�F�N�g���̌^�����C�h�����ɕϊ��B
			/// char -> wchar_t
			/// �ϊ�����char�^�̕�����B
			/// �ϊ����ʂ�wchar_t�^������B
			/// �T�C�Y
			/// </summary>
			mbstowcs(boneName, tklObj.name.get(), 256);
			BonePtr bone = std::make_unique<Bone>(
				boneName,
				bindPoseMatrix,
				invBindPoseMatirx,
				tklObj.parentNo,
				tklObj.no
				);
			//�{�[����ςށB
			m_bonelist.push_back(std::move(bone));
			//�R�s�[�I���B
		});

	}

	void LevelRender::Update()
	{
		for (auto& mapChipRender : m_mapChipRenderPtrs)
		{
			//�}�b�v�`�b�v�����_�[���X�V����B
			mapChipRender.second->Update();
		}
	}

	void LevelRender::Draw(RenderContext& rc)
	{
		for (auto mapChipRenderPtr : m_mapChipRenderPtrs)
		{
			mapChipRenderPtr.second->Draw(rc);
		}
	}
}