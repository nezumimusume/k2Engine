#include "k2EngineLowPreCompile.h"
#include "tkFile/TkmFile.h"

namespace nsK2EngineLow {
	
	namespace {
		struct SSmoothVertex {
			Vector3 newNormal = g_vec3Zero;
			TkmFile::SVertex* vertex = nullptr;
		};
	}
	//法線スムージング。
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

			//ステップ１面法線を計算していく。
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

				//法線を計算する。
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

			//ステップ２　法線の平均化
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
	/// TKMファイルフォーマット。
	/// </summary>
	/// <remarks>
	/// 処理系によっては1バイトが8bitでないことがあり、
	/// int型やshort型が必ずしも、4バイト、2バイトであるとは限らない。
	/// そのため、std::uint16_tやstd::uint32_tを利用している。
	/// これらは定義されている処理系であれば、サイズは必ず同じである。
	/// </remarks>
	namespace tkmFileFormat {
		//現在のTKMファイルのバージョン。
		std::uint16_t VERSION = 100;
		/// <summary>
		/// ヘッダーファイル。
		/// </summary>
		struct SHeader {
			std::uint8_t	version;		//バージョン。
			std::uint8_t	isFlatShading;	//フラットシェーディング？
			std::uint16_t	numMeshParts;	//メッシュパーツの数。
		};
		/// <summary>
		/// メッシュパーツヘッダー。
		/// </summary>
		struct SMeshePartsHeader {
			std::uint32_t numMaterial;		//マテリアルの数。
			std::uint32_t numVertex;		//頂点数。
			std::uint8_t indexSize;			//インデックスのサイズ。2か4。
			std::uint8_t pad[3];			//パディング。
		};
		/// <summary>
		/// 頂点
		/// </summary>
		struct SVertex {
			float pos[3];					//頂点座標。
			float normal[3];				//法線。
			float uv[2];					//UV座標。
			float weights[4];				//スキンウェイト。
			std::int16_t indices[4];		//スキンインデックス。
		};
	};
	template< class IndexBuffer>
	void BuildTangentAndBiNormalImp(TkmFile::SMesh& mesh, const IndexBuffer& indexBuffer)
	{
		//頂点スムースは気にしない。
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

			// 平面パラメータからUV軸座標算出する。
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
		//法線、接ベクトル、従ベクトルを平均化する。
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
			//ヌル文字分も読み込むので＋１
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
			indices[indexNo] = index - 1;	//todo maxのインデックスは1から開始しているので、-1する。
										//todo エクスポーターで減らすようにしましょう。
		}
	}

	void TkmFile::BuildMaterial(SMaterial& tkmMat, FILE* fp, const char* filePath)
	{
		//アルベドのファイル名をロード。
		tkmMat.albedoMapFileName = LoadTextureFileName(fp);
		//法線マップのファイル名をロード。
		tkmMat.normalMapFileName = LoadTextureFileName(fp);
		//スペキュラマップのファイル名をロード。
		tkmMat.specularMapFileName = LoadTextureFileName(fp);
		//リフレクションマップのファイル名をロード。
		tkmMat.reflectionMapFileName = LoadTextureFileName(fp);
		//屈折マップのファイル名をロード。
		tkmMat.refractionMapFileName = LoadTextureFileName(fp);

		std::string texFilePath = filePath;
		auto loadTexture = [&](
			std::string& texFileName,
			std::unique_ptr<char[]>& ddsFileMemory,
			unsigned int& fileSize,
			std::string& filePath
			) {
			int filePathLength = static_cast<int>(texFilePath.length());
			if (texFileName.length() > 0) {
				//モデルのファイルパスからラストのフォルダ区切りを探す。
				auto replaseStartPos = texFilePath.find_last_of('/');
				if (replaseStartPos == std::string::npos) {
					replaseStartPos = texFilePath.find_last_of('\\');
				}
				replaseStartPos += 1;
				auto replaceLen = filePathLength - replaseStartPos;
				texFilePath.replace(replaseStartPos, replaceLen, texFileName);
				//拡張子をddsに変更する。
				replaseStartPos = texFilePath.find_last_of('.') + 1;
				replaceLen = texFilePath.length() - replaseStartPos;
				texFilePath.replace(replaseStartPos, replaceLen, "dds");

				//テクスチャをロード。
				FILE* texFileFp = fopen(texFilePath.c_str(), "rb");
				if (texFileFp != nullptr) {
					//ファイルサイズを取得。
					fseek(texFileFp, 0L, SEEK_END);
					fileSize = ftell(texFileFp);
					fseek(texFileFp, 0L, SEEK_SET);

					ddsFileMemory = std::make_unique<char[]>(fileSize);
					fread(ddsFileMemory.get(), fileSize, 1, texFileFp);
					fclose(texFileFp);
					filePath = texFilePath;
				}
				else {
					char errorMessage[256];
					sprintf(errorMessage, "テクスチャのロードに失敗しました。%s\n", texFilePath.c_str());
					MessageBoxA(nullptr, errorMessage, "エラー", MB_OK);

				}
			}
		};
		//テクスチャをロード。
		loadTexture(tkmMat.albedoMapFileName, tkmMat.albedoMap, tkmMat.albedoMapSize, tkmMat.albedoMapFilePath);
		loadTexture(tkmMat.normalMapFileName, tkmMat.normalMap, tkmMat.normalMapSize, tkmMat.normalMapFilePath);
		loadTexture(tkmMat.specularMapFileName, tkmMat.specularMap, tkmMat.specularMapSize, tkmMat.specularMapFilePath);
		loadTexture(tkmMat.reflectionMapFileName, tkmMat.reflectionMap, tkmMat.reflectionMapSize, tkmMat.reflectionMapFilePath);
		loadTexture(tkmMat.refractionMapFileName, tkmMat.refractionMap, tkmMat.refractionMapSize, tkmMat.refractionMapFilePath);
	}
	
	void TkmFile::BuildTangentAndBiNormal()
	{
		NormalSmoothing normalSmoothing;
		// 頂点バッファはメッシュごとに独立しているので、スムージングを４つのスレッドで分担して行うことができる。
		for (auto& mesh : m_meshParts) {
			for (auto& indexBuffer : mesh.indexBuffer16Array) {
				normalSmoothing.Execute(mesh, indexBuffer, m_bpsOnVertexPosition);
				
			}
			for (auto& indexBuffer : mesh.indexBuffer32Array) {
				normalSmoothing.Execute(mesh, indexBuffer, m_bpsOnVertexPosition);
				
			}
		}

		if (m_meshParts[0].isFlatShading == false) {
			// フラットシェーディングでないなら、座標と向きが同じ頂点の法線を平均化していく。
			// メッシュの全頂点数を調べる。
			int vertNum = 0;
			for (auto& mesh : m_meshParts) {
				vertNum += (int)mesh.vertexBuffer.size();
			}
			// スムージング対象の頂点を集める。
			std::vector<SSmoothVertex> smoothVertex;
			smoothVertex.reserve(vertNum);
			for (auto& mesh : m_meshParts) {
				for (auto& v : mesh.vertexBuffer) {
					smoothVertex.push_back({ v.normal, &v });
				}
			}

			// 合計4スレッドを使ってスムージングを行う。
			const int NUM_THREAD = 4;
			int numVertexPerThread = smoothVertex.size();
			// スムージング関数。
			auto smoothFunc = [&](int startIndex, int endIndex)
			{
				// スムースしていく。
				for (int i = startIndex; i < endIndex; i++) {
					auto& va = smoothVertex[i];
					m_bpsOnVertexPosition.WalkTree(va.vertex->pos, [&](BSP::SLeaf* leaf) {
						if (va.vertex->pos.x == leaf->position.x
							&& va.vertex->pos.y == leaf->position.y
							&& va.vertex->pos.z == leaf->position.z) {
							//同じ座標。
							auto* normal = static_cast<Vector3*>(leaf->extraData);
							if (va.vertex->normal.Dot(*normal) > 0.0f) {
								//同じ向き。
								va.newNormal += *normal;
							}
						}
					});
				}
			};

			// 一個のスレッドあたりに処理を行う頂点数を計算する。
			int perVertexInOneThread = smoothVertex.size() / NUM_THREAD;
			using namespace std;
			using ThreadPtr = unique_ptr < thread >;
			auto smoothingThreadArray = make_unique< ThreadPtr[] >(NUM_THREAD);


			int startVertex = 0;
			int endVertex = perVertexInOneThread;

			// スレッドを立てる。
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
			
			// スムージングスレッドが全て完了するのを待つ。
			for (int i = 0; i < NUM_THREAD; i++) {
				smoothingThreadArray[i]->join();
			}
		
			for (auto& va : smoothVertex) {
				va.newNormal.Normalize();
				va.vertex->normal = va.newNormal;
			}

		}

		// 接ベクトルと従ベクトルを計算する。
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
			MessageBoxA(nullptr, "tkmファイルが開けません。", "エラー", MB_OK);
			return;
		}
		//tkmファイルのヘッダーを読み込み。
		tkmFileFormat::SHeader header;
		fread(&header, sizeof(header), 1, fp);
		if (header.version != tkmFileFormat::VERSION) {
			//tkmファイルのバージョンが違う。
			MessageBoxA(nullptr, "tkmファイルのバージョンが異なっています。", "エラー", MB_OK);
		}
		//メッシュ情報をロードしていく。
		m_meshParts.resize(header.numMeshParts);
		for (int meshPartsNo = 0; meshPartsNo < header.numMeshParts; meshPartsNo++) {

			auto& meshParts = m_meshParts[meshPartsNo];
			meshParts.isFlatShading = header.isFlatShading;
			tkmFileFormat::SMeshePartsHeader meshPartsHeader;
			fread(&meshPartsHeader, sizeof(meshPartsHeader), 1, fp);
			//マテリアル情報を記録できる領域を確保。
			meshParts.materials.resize(meshPartsHeader.numMaterial);
			//マテリアル情報を構築していく。
			for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
				auto& material = meshParts.materials[materialNo];
				BuildMaterial(material, fp, filePath);
			}

			//続いて頂点バッファ。
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

			//続いてインデックスバッファ。
			//インデックスバッファはマテリアルの数分だけ存在するんじゃよ。
			if (meshPartsHeader.indexSize == 2) {
				//16bitのインデックスバッファ。
				meshParts.indexBuffer16Array.resize(meshPartsHeader.numMaterial);
			}
			else {
				//32bitのインデックスバッファ。
				meshParts.indexBuffer32Array.resize(meshPartsHeader.numMaterial);
			}

			for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
				//ポリゴン数をロード。
				int numPolygon;
				fread(&numPolygon, sizeof(numPolygon), 1, fp);
				//トポロジーはトライアングルリストオンリーなので、3を乗算するとインデックスの数になる。
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

		// 頂点データのBSPツリーを構築する。
		m_bpsOnVertexPosition.Build();

		// 接ベクトルと従ベクトルを構築する。
		BuildTangentAndBiNormal();

		fclose(fp);

	}
}