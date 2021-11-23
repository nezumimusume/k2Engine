#include "k2EngineLowPreCompile.h"
#include "tkFile/TkmFile.h"

namespace nsK2EngineLow {
	
	namespace {
		struct SSmoothVertex {
			Vector3 newNormal = g_vec3Zero;
			TkmFile::SVertex* vertex = nullptr;
		};
	}
	//�@���X���[�W���O�B
	class NormalSmoothing {
	private:
		
		struct SFace {
			Vector3 normal;
			std::vector<int> vertexNos;
		};
	public:
		template <class IndexBuffer>
		void Execute(TkmFile::SMesh& mesh, const IndexBuffer& indexBuffer, BSP& bsp)
		{

			//�X�e�b�v�P�ʖ@�����v�Z���Ă����B
			auto numPolygon = indexBuffer.indices.size() / 3;
			std::vector< SFace> faces;
			faces.reserve(numPolygon);

			for (auto polyNo = 0; polyNo < numPolygon; polyNo++) {
				auto no = polyNo * 3;
				auto vertNo_0 = indexBuffer.indices[no];
				auto vertNo_1 = indexBuffer.indices[no + 1];
				auto vertNo_2 = indexBuffer.indices[no + 2];

				auto& vert_0 = mesh.vertexBuffer[vertNo_0];
				auto& vert_1 = mesh.vertexBuffer[vertNo_1];
				auto& vert_2 = mesh.vertexBuffer[vertNo_2];

				//�@�����v�Z����B
				Vector3 v0tov1 = vert_1.pos - vert_0.pos;
				Vector3 v0tov2 = vert_2.pos - vert_0.pos;
				Vector3 normal = Cross(v0tov1, v0tov2);
				normal.Normalize();
				SFace face;
				face.normal = normal;
				face.vertexNos.push_back(vertNo_0);
				face.vertexNos.push_back(vertNo_1);
				face.vertexNos.push_back(vertNo_2);
				faces.push_back(face);
			}

			//�X�e�b�v�Q�@�@���̕��ω�
			for (auto& face : faces) {
				for (auto vertNo : face.vertexNos) {
					auto& vert = mesh.vertexBuffer[vertNo];
					vert.normal += face.normal;
				}
			}
			for (auto& vert : mesh.vertexBuffer) {
				vert.normal.Normalize();
			}
		}
	};
	/// <summary>
	/// TKM�t�@�C���t�H�[�}�b�g�B
	/// </summary>
	/// <remarks>
	/// �����n�ɂ���Ă�1�o�C�g��8bit�łȂ����Ƃ�����A
	/// int�^��short�^���K�������A4�o�C�g�A2�o�C�g�ł���Ƃ͌���Ȃ��B
	/// ���̂��߁Astd::uint16_t��std::uint32_t�𗘗p���Ă���B
	/// �����͒�`����Ă��鏈���n�ł���΁A�T�C�Y�͕K�������ł���B
	/// </remarks>
	namespace tkmFileFormat {
		//���݂�TKM�t�@�C���̃o�[�W�����B
		std::uint16_t VERSION = 100;
		/// <summary>
		/// �w�b�_�[�t�@�C���B
		/// </summary>
		struct SHeader {
			std::uint8_t	version;		//�o�[�W�����B
			std::uint8_t	isFlatShading;	//�t���b�g�V�F�[�f�B���O�H
			std::uint16_t	numMeshParts;	//���b�V���p�[�c�̐��B
		};
		/// <summary>
		/// ���b�V���p�[�c�w�b�_�[�B
		/// </summary>
		struct SMeshePartsHeader {
			std::uint32_t numMaterial;		//�}�e���A���̐��B
			std::uint32_t numVertex;		//���_���B
			std::uint8_t indexSize;			//�C���f�b�N�X�̃T�C�Y�B2��4�B
			std::uint8_t pad[3];			//�p�f�B���O�B
		};
		/// <summary>
		/// ���_
		/// </summary>
		struct SVertex {
			float pos[3];					//���_���W�B
			float normal[3];				//�@���B
			float uv[2];					//UV���W�B
			float weights[4];				//�X�L���E�F�C�g�B
			std::int16_t indices[4];		//�X�L���C���f�b�N�X�B
		};
	};
	template< class IndexBuffer>
	void BuildTangentAndBiNormalImp(TkmFile::SMesh& mesh, const IndexBuffer& indexBuffer)
	{
		//���_�X���[�X�͋C�ɂ��Ȃ��B
		auto numPolygon = indexBuffer.indices.size() / 3;
		for (auto polyNo = 0; polyNo < numPolygon; polyNo++) {
			auto no = polyNo * 3;
			auto vertNo_0 = indexBuffer.indices[no];
			auto vertNo_1 = indexBuffer.indices[no + 1];
			auto vertNo_2 = indexBuffer.indices[no + 2];

			auto& vert_0 = mesh.vertexBuffer[vertNo_0];
			auto& vert_1 = mesh.vertexBuffer[vertNo_1];
			auto& vert_2 = mesh.vertexBuffer[vertNo_2];

			Vector3 cp0[] = {
				{ vert_0.pos.x, vert_0.uv.x, vert_0.uv.y},
				{ vert_0.pos.y, vert_0.uv.x, vert_0.uv.y},
				{ vert_0.pos.z, vert_0.uv.x, vert_0.uv.y}
			};

			Vector3 cp1[] = {
				{ vert_1.pos.x, vert_1.uv.x, vert_1.uv.y},
				{ vert_1.pos.y, vert_1.uv.x, vert_1.uv.y},
				{ vert_1.pos.z, vert_1.uv.x, vert_1.uv.y}
			};

			Vector3 cp2[] = {
				{ vert_2.pos.x, vert_2.uv.x, vert_2.uv.y},
				{ vert_2.pos.y, vert_2.uv.x, vert_2.uv.y},
				{ vert_2.pos.z, vert_2.uv.x, vert_2.uv.y}
			};

			// ���ʃp�����[�^����UV�����W�Z�o����B
			Vector3 tangent, binormal;
			for (int i = 0; i < 3; ++i) {
				auto V1 = cp1[i] - cp0[i];
				auto V2 = cp2[i] - cp1[i];
				auto ABC = Cross(V1, V2);

				if (ABC.x == 0.0f) {
					tangent.v[i] = 0.0f;
					binormal.v[i] = 0.0f;
				}
				else {
					tangent.v[i] = -ABC.y / ABC.x;
					binormal.v[i] = -ABC.z / ABC.x;
				}
			}

			tangent.Normalize();
			binormal.Normalize();

			vert_0.tangent += tangent;
			vert_1.tangent += tangent;
			vert_2.tangent += tangent;

			vert_0.binormal += binormal;
			vert_1.binormal += binormal;
			vert_2.binormal += binormal;
		}
		//�@���A�ڃx�N�g���A�]�x�N�g���𕽋ω�����B
		for (auto& vert : mesh.vertexBuffer) {
			vert.tangent.Normalize();
			vert.binormal.Normalize();
		}

	}
	std::string TkmFile::LoadTextureFileName(FILE* fp)
	{
		std::string fileName;
		std::uint32_t fileNameLen;
		fread(&fileNameLen, sizeof(fileNameLen), 1, fp);

		if (fileNameLen > 0) {
			char* localFileName = reinterpret_cast<char*>(malloc(fileNameLen + 1));
			//�k�����������ǂݍ��ނ̂Ł{�P
			fread(localFileName, fileNameLen + 1, 1, fp);
			fileName = localFileName;
			free(localFileName);
		}

		return fileName;
	}
	template<class T>
	void TkmFile::LoadIndexBuffer(std::vector<T>& indices, int numIndex, FILE* fp)
	{
		indices.resize(numIndex);
		for (int indexNo = 0; indexNo < numIndex; indexNo++) {
			T index;
			fread(&index, sizeof(index), 1, fp);
			indices[indexNo] = index - 1;	//todo max�̃C���f�b�N�X��1����J�n���Ă���̂ŁA-1����B
										//todo �G�N�X�|�[�^�[�Ō��炷�悤�ɂ��܂��傤�B
		}
	}

	void TkmFile::BuildMaterial(SMaterial& tkmMat, FILE* fp, const char* filePath)
	{
		//�A���x�h�̃t�@�C���������[�h�B
		tkmMat.albedoMapFileName = LoadTextureFileName(fp);
		//�@���}�b�v�̃t�@�C���������[�h�B
		tkmMat.normalMapFileName = LoadTextureFileName(fp);
		//�X�y�L�����}�b�v�̃t�@�C���������[�h�B
		tkmMat.specularMapFileName = LoadTextureFileName(fp);
		//���t���N�V�����}�b�v�̃t�@�C���������[�h�B
		tkmMat.reflectionMapFileName = LoadTextureFileName(fp);
		//���܃}�b�v�̃t�@�C���������[�h�B
		tkmMat.refractionMapFileName = LoadTextureFileName(fp);

		std::string texFilePath = filePath;
		auto loadTexture = [&](
			std::string& texFileName,
			LowTexture*& lowTexture
			) {
			int filePathLength = static_cast<int>(texFilePath.length());
			if (texFileName.length() > 0) {
				//���f���̃t�@�C���p�X���烉�X�g�̃t�H���_��؂��T���B
				auto replaseStartPos = texFilePath.find_last_of('/');
				if (replaseStartPos == std::string::npos) {
					replaseStartPos = texFilePath.find_last_of('\\');
				}
				replaseStartPos += 1;
				auto replaceLen = filePathLength - replaseStartPos;
				texFilePath.replace(replaseStartPos, replaceLen, texFileName);
				//�g���q��dds�ɕύX����B
				replaseStartPos = texFilePath.find_last_of('.') + 1;
				replaceLen = texFilePath.length() - replaseStartPos;
				texFilePath.replace(replaseStartPos, replaceLen, "dds");

				// �e�N�X�`�������\�[�X�o���N����擾����B
				lowTexture = g_engine->GetLowTextureFromBank(texFilePath.c_str());
				if (lowTexture == nullptr) {
					lowTexture = new LowTexture();
					// �o���N����擾�ł��Ȃ������̂ŁA�V�K�e�N�X�`���B
					FILE* texFileFp = fopen(texFilePath.c_str(), "rb");
					if (texFileFp != nullptr) {
						//�t�@�C���T�C�Y���擾�B
						fseek(texFileFp, 0L, SEEK_END);
						lowTexture->dataSize = ftell(texFileFp);
						fseek(texFileFp, 0L, SEEK_SET);

						lowTexture->data = std::make_unique<char[]>(lowTexture->dataSize);
						fread(lowTexture->data.get(), lowTexture->dataSize, 1, texFileFp);
						fclose(texFileFp);
						lowTexture->filePath = texFilePath;
						// ���[�h�����e�N�X�`�����o���N�ɓo�^����B
						g_engine->RegistLowTextureToBank(lowTexture->filePath.c_str(), lowTexture);
					}
					else {
						char errorMessage[256];
						sprintf(errorMessage, "�e�N�X�`���̃��[�h�Ɏ��s���܂����B%s\n", texFilePath.c_str());
						MessageBoxA(nullptr, errorMessage, "�G���[", MB_OK);

					}
				}
				else {
					int hoge = 0;
				}
				
			}
		};
		//�e�N�X�`�������[�h�B
		loadTexture(tkmMat.albedoMapFileName, tkmMat.albedoMap);
		loadTexture(tkmMat.normalMapFileName, tkmMat.normalMap);
		loadTexture(tkmMat.specularMapFileName, tkmMat.specularMap);
		loadTexture(tkmMat.reflectionMapFileName, tkmMat.reflectionMap);
		loadTexture(tkmMat.refractionMapFileName, tkmMat.refractionMap);
	}
	
	void TkmFile::BuildTangentAndBiNormal()
	{
		NormalSmoothing normalSmoothing;
		// ���_�o�b�t�@�̓��b�V�����ƂɓƗ����Ă���̂ŁA�X���[�W���O���S�̃X���b�h�ŕ��S���čs�����Ƃ��ł���B
		for (auto& mesh : m_meshParts) {
			for (auto& indexBuffer : mesh.indexBuffer16Array) {
				normalSmoothing.Execute(mesh, indexBuffer, m_bpsOnVertexPosition);
				
			}
			for (auto& indexBuffer : mesh.indexBuffer32Array) {
				normalSmoothing.Execute(mesh, indexBuffer, m_bpsOnVertexPosition);
				
			}
		}

		if (m_meshParts[0].isFlatShading == false) {
			// �t���b�g�V�F�[�f�B���O�łȂ��Ȃ�A���W�ƌ������������_�̖@���𕽋ω����Ă����B
			// ���b�V���̑S���_���𒲂ׂ�B
			int vertNum = 0;
			for (auto& mesh : m_meshParts) {
				vertNum += (int)mesh.vertexBuffer.size();
			}
			// �X���[�W���O�Ώۂ̒��_���W�߂�B
			std::vector<SSmoothVertex> smoothVertex;
			smoothVertex.reserve(vertNum);
			for (auto& mesh : m_meshParts) {
				for (auto& v : mesh.vertexBuffer) {
					smoothVertex.push_back({ v.normal, &v });
				}
			}

			// ���v4�X���b�h���g���ăX���[�W���O���s���B
			const int NUM_THREAD = 4;
			int numVertexPerThread = smoothVertex.size();
			// �X���[�W���O�֐��B
			auto smoothFunc = [&](int startIndex, int endIndex)
			{
				// �X���[�X���Ă����B
				for (int i = startIndex; i < endIndex; i++) {
					auto& va = smoothVertex[i];
					m_bpsOnVertexPosition.WalkTree(va.vertex->pos, [&](BSP::SLeaf* leaf) {
						if (va.vertex->pos.x == leaf->position.x
							&& va.vertex->pos.y == leaf->position.y
							&& va.vertex->pos.z == leaf->position.z) {
							//�������W�B
							auto* normal = static_cast<Vector3*>(leaf->extraData);
							if (va.vertex->normal.Dot(*normal) > 0.0f) {
								//���������B
								va.newNormal += *normal;
							}
						}
					});
				}
			};

			// ��̃X���b�h������ɏ������s�����_�����v�Z����B
			int perVertexInOneThread = smoothVertex.size() / NUM_THREAD;
			using namespace std;
			using ThreadPtr = unique_ptr < thread >;
			auto smoothingThreadArray = make_unique< ThreadPtr[] >(NUM_THREAD);


			int startVertex = 0;
			int endVertex = perVertexInOneThread;

			// �X���b�h�𗧂Ă�B
			for (int i = 0; i < NUM_THREAD - 1; i++) {
				smoothingThreadArray[i] = make_unique<thread>([&, startVertex, endVertex]() {
					smoothFunc(startVertex, endVertex);
				});
				startVertex = endVertex;
				endVertex += perVertexInOneThread;
			}
			endVertex = (int)smoothVertex.size();
			smoothingThreadArray[NUM_THREAD - 1] = make_unique<thread>([&, startVertex, endVertex]() {
				smoothFunc(startVertex, endVertex);
			});
			
			// �X���[�W���O�X���b�h���S�Ċ�������̂�҂B
			for (int i = 0; i < NUM_THREAD; i++) {
				smoothingThreadArray[i]->join();
			}
		
			for (auto& va : smoothVertex) {
				va.newNormal.Normalize();
				va.vertex->normal = va.newNormal;
			}

		}

		// �ڃx�N�g���Ə]�x�N�g�����v�Z����B
		for (auto& mesh : m_meshParts) {
			for (auto& indexBuffer : mesh.indexBuffer16Array) {
				BuildTangentAndBiNormalImp(mesh, indexBuffer);
			}
			for (auto& indexBuffer : mesh.indexBuffer32Array) {
				BuildTangentAndBiNormalImp(mesh, indexBuffer);
			}
		}
	}
	void TkmFile::Load(const char* filePath)
	{
		FILE* fp = fopen(filePath, "rb");
		if (fp == nullptr) {
			MessageBoxA(nullptr, "tkm�t�@�C�����J���܂���B", "�G���[", MB_OK);
			return;
		}
		//tkm�t�@�C���̃w�b�_�[��ǂݍ��݁B
		tkmFileFormat::SHeader header;
		fread(&header, sizeof(header), 1, fp);
		if (header.version != tkmFileFormat::VERSION) {
			//tkm�t�@�C���̃o�[�W�������Ⴄ�B
			MessageBoxA(nullptr, "tkm�t�@�C���̃o�[�W�������قȂ��Ă��܂��B", "�G���[", MB_OK);
		}
		//���b�V���������[�h���Ă����B
		m_meshParts.resize(header.numMeshParts);
		for (int meshPartsNo = 0; meshPartsNo < header.numMeshParts; meshPartsNo++) {

			auto& meshParts = m_meshParts[meshPartsNo];
			meshParts.isFlatShading = header.isFlatShading;
			tkmFileFormat::SMeshePartsHeader meshPartsHeader;
			fread(&meshPartsHeader, sizeof(meshPartsHeader), 1, fp);
			//�}�e���A�������L�^�ł���̈���m�ہB
			meshParts.materials.resize(meshPartsHeader.numMaterial);
			//�}�e���A�������\�z���Ă����B
			for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
				auto& material = meshParts.materials[materialNo];
				BuildMaterial(material, fp, filePath);
			}

			//�����Ē��_�o�b�t�@�B
			meshParts.vertexBuffer.resize(meshPartsHeader.numVertex);
			for (unsigned int vertNo = 0; vertNo < meshPartsHeader.numVertex; vertNo++) {
				tkmFileFormat::SVertex vertexTmp;
				fread(&vertexTmp, sizeof(vertexTmp), 1, fp);
				auto& vertex = meshParts.vertexBuffer[vertNo];
				vertex.pos.Set(vertexTmp.pos[0], vertexTmp.pos[1], vertexTmp.pos[2]);
				//	vertex.normal.Set(vertexTmp.normal[0], vertexTmp.normal[1], vertexTmp.normal[2]);
				vertex.normal = g_vec3Zero;
				vertex.tangent = g_vec3Zero;
				vertex.binormal = g_vec3Zero;
				vertex.uv.Set(vertexTmp.uv[0], vertexTmp.uv[1]);
				vertex.skinWeights.Set(vertexTmp.weights[0], vertexTmp.weights[1], vertexTmp.weights[2], vertexTmp.weights[3]);
				vertex.indices[0] = vertexTmp.indices[0] != -1 ? vertexTmp.indices[0] : 0;
				vertex.indices[1] = vertexTmp.indices[1] != -1 ? vertexTmp.indices[1] : 0;
				vertex.indices[2] = vertexTmp.indices[2] != -1 ? vertexTmp.indices[2] : 0;
				vertex.indices[3] = vertexTmp.indices[3] != -1 ? vertexTmp.indices[3] : 0;

				m_bpsOnVertexPosition.AddLeaf(vertex.pos, &vertex.normal);
			}

			//�����ăC���f�b�N�X�o�b�t�@�B
			//�C���f�b�N�X�o�b�t�@�̓}�e���A���̐����������݂���񂶂��B
			if (meshPartsHeader.indexSize == 2) {
				//16bit�̃C���f�b�N�X�o�b�t�@�B
				meshParts.indexBuffer16Array.resize(meshPartsHeader.numMaterial);
			}
			else {
				//32bit�̃C���f�b�N�X�o�b�t�@�B
				meshParts.indexBuffer32Array.resize(meshPartsHeader.numMaterial);
			}

			for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
				//�|���S���������[�h�B
				int numPolygon;
				fread(&numPolygon, sizeof(numPolygon), 1, fp);
				//�g�|���W�[�̓g���C�A���O�����X�g�I�����[�Ȃ̂ŁA3����Z����ƃC���f�b�N�X�̐��ɂȂ�B
				int numIndex = numPolygon * 3;
				if (meshPartsHeader.indexSize == 2) {
					LoadIndexBuffer(
						meshParts.indexBuffer16Array[materialNo].indices,
						numIndex,
						fp
					);
				}
				else {
					LoadIndexBuffer(
						meshParts.indexBuffer32Array[materialNo].indices,
						numIndex,
						fp
					);
				}
			}
		}

		// ���_�f�[�^��BSP�c���[���\�z����B
		m_bpsOnVertexPosition.Build();

		// �ڃx�N�g���Ə]�x�N�g�����\�z����B
		BuildTangentAndBiNormal();

		fclose(fp);

	}
}