#include "k2EngineLowPreCompile.h"
#include "tkFile/TkmFile.h"
#include "util/Util.h"
#include <format>

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
			if (vert.tangent.Length() < 0.001f) {
				if (vert.normal.y > 0.998f) {
					// 法線がほぼY軸を向いている。
					vert.tangent = g_vec3AxisX;
				}
				else {
					vert.tangent = g_vec3AxisY;
				}
				vert.binormal.Cross(vert.normal, vert.tangent);
				vert.binormal.Normalize();
			}
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

	void TkmFile::BuildMaterial(
		SMaterial& tkmMat, 
		FILE* fp, 
		const char* filePath, 
		bool isLoadTexture, 
		bool isOutputErrorCodeTTY
	){
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
			LowTexture*& lowTexture
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

				// テクスチャをリソースバンクから取得する。
				lowTexture = g_engine->GetLowTextureFromBank(texFilePath.c_str());
				if (lowTexture == nullptr) {
					lowTexture = new LowTexture();
					// バンクから取得できなかったので、新規テクスチャ。
					FILE* texFileFp = fopen(texFilePath.c_str(), "rb");
					if (texFileFp != nullptr) {
						//ファイルサイズを取得。
						fseek(texFileFp, 0L, SEEK_END);
						lowTexture->dataSize = ftell(texFileFp);
						fseek(texFileFp, 0L, SEEK_SET);

						lowTexture->data = std::make_unique<char[]>(lowTexture->dataSize);
						fread(lowTexture->data.get(), lowTexture->dataSize, 1, texFileFp);
						fclose(texFileFp);
						lowTexture->filePath = texFilePath;
						// ロードしたテクスチャをバンクに登録する。
						g_engine->RegistLowTextureToBank(lowTexture->filePath.c_str(), lowTexture);
					}
					else {
						char errorMessage[256];
						sprintf(errorMessage, "テクスチャのロードに失敗しました。%s\n", texFilePath.c_str());
						if (isOutputErrorCodeTTY == false) {
							MessageBoxA(nullptr, errorMessage, "エラー", MB_OK);
						}
						else {
							printf(errorMessage);
						}
					}
				}
			}
		};
		if (isLoadTexture) {
			// テクスチャをロード。
			loadTexture(tkmMat.albedoMapFileName, tkmMat.albedoMap);
			loadTexture(tkmMat.normalMapFileName, tkmMat.normalMap);
			loadTexture(tkmMat.specularMapFileName, tkmMat.specularMap);
			loadTexture(tkmMat.reflectionMapFileName, tkmMat.reflectionMap);
			loadTexture(tkmMat.refractionMapFileName, tkmMat.refractionMap);
		}
		// マテリアルのユニークIDを生成する。
		std::string sourceName = tkmMat.albedoMapFileName;
		if (!tkmMat.normalMapFileName.empty()) {
			sourceName += tkmMat.normalMapFileName;
		}
		if (!tkmMat.specularMapFileName.empty()) {
			sourceName += tkmMat.specularMapFileName;
		}
		if (!tkmMat.reflectionMapFileName.empty()) {
			sourceName += tkmMat.reflectionMapFileName;
		}
		if (!tkmMat.refractionMapFileName.empty()) {
			sourceName += tkmMat.refractionMapFileName;
		}
		// テクスチャ名からユニークIDを生成する。
		tkmMat.uniqID = MakeHash(sourceName.c_str());
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
			int numVertexPerThread = static_cast<int>(smoothVertex.size());
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
			int perVertexInOneThread = static_cast<int>(smoothVertex.size() / NUM_THREAD);
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
	bool TkmFile::Load(const char* filePath, bool isOptimize, bool isLoadTexture, bool isOutputErrorCodeTTY)
	{
		FILE* fp = fopen(filePath, "rb");
		if (fp == nullptr) {
			char errorMessage[256];
			sprintf(errorMessage, "tkmファイルのオープンに失敗しました。filePath = %s\n", filePath);
			
			if (!isOutputErrorCodeTTY) {
				MessageBoxA(nullptr, errorMessage, "エラー", MB_OK);
			}
			else {
				printf(errorMessage);
			}
			// 失敗。
			return false;
		}
		//tkmファイルのヘッダーを読み込み。
		tkmFileFormat::SHeader header;
		fread(&header, sizeof(header), 1, fp);
		if (header.version != tkmFileFormat::VERSION) {
			std::string errorMessage = "tkmファイルのバージョンが異なっています。";
			if (!isOutputErrorCodeTTY) {
				//tkmファイルのバージョンが違う。
				MessageBoxA(nullptr, errorMessage.c_str(), "エラー", MB_OK);
			}
			else {
				printf(errorMessage.c_str());
			}
			return false;
		}
		
		//メッシュ情報をロードしていく。
		m_meshParts.resize(header.numMeshParts);
		for (int meshPartsNo = 0; meshPartsNo < header.numMeshParts; meshPartsNo++) {

			auto& meshParts = m_meshParts[meshPartsNo];
			meshParts.isFlatShading = header.isFlatShading != 0;
			tkmFileFormat::SMeshePartsHeader meshPartsHeader;
			fread(&meshPartsHeader, sizeof(meshPartsHeader), 1, fp);
			//マテリアル情報を記録できる領域を確保。
			meshParts.materials.resize(meshPartsHeader.numMaterial);
			//マテリアル情報を構築していく。
			for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
				auto& material = meshParts.materials[materialNo];
				BuildMaterial(material, fp, filePath, isLoadTexture, isOutputErrorCodeTTY);
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
				vertex.indices[0] = vertexTmp.indices[0];
				vertex.indices[1] = vertexTmp.indices[1];
				vertex.indices[2] = vertexTmp.indices[2];
				vertex.indices[3] = vertexTmp.indices[3];

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
		// ファイルを閉じる。
		fclose(fp);

		// 頂点データのBSPツリーを構築する。
		m_bpsOnVertexPosition.Build();

		// 接ベクトルと従ベクトルを構築する。
		BuildTangentAndBiNormal();
		
		if (isOptimize) {
			// 最適化を行う。
			Optimize();
		}
		return true;
	}
	bool TkmFile::Save(const char* filePath)
	{
		FILE* fp = fopen(filePath, "wb");
		if (fp == nullptr) {
			printf("出力用のtkmファイルのオープンに失敗しました。%s\n", filePath);
			return false;
		}
		if (m_meshParts.empty()) {
			printf("オリジナルのtkmファイルがロードされていません。%s\n", filePath);
			return false;
		}
		// ヘッダー情報の構築。
		tkmFileFormat::SHeader header;
		header.isFlatShading = m_meshParts[0].isFlatShading ? 1 : 0;
		header.numMeshParts = m_meshParts.size();
		header.version = tkmFileFormat::VERSION;
		fwrite(&header, sizeof(header), 1, fp);

		// 続いてメッシュパーツ本体のデータを書き込んでいく。
		for (int meshPartsNo = 0; meshPartsNo < header.numMeshParts; meshPartsNo++) {
			tkmFileFormat::SMeshePartsHeader meshPartsHeader;
			meshPartsHeader.numMaterial = m_meshParts[meshPartsNo].materials.size();
			meshPartsHeader.numVertex = m_meshParts[meshPartsNo].vertexBuffer.size();
			meshPartsHeader.indexSize = 4; // 32ビット固定。
			fwrite(&meshPartsHeader, sizeof(meshPartsHeader), 1, fp);
			// マテリアル情報を書き込んでいく。
			for (int matNo = 0; matNo < m_meshParts[meshPartsNo].materials.size(); matNo++) {
				SMaterial& mat = m_meshParts[meshPartsNo].materials[matNo];
				// テクスチャのファイル名情報を書き込む匿名関数。
				auto WriteTextureFileNameInfo = [&](const std::string& fineName)
				{
					std::uint32_t fileNameLen = fineName.length();
					// ファイル名情報を書き込む。
					if (fineName.empty()) {
						fileNameLen = 0;
						fwrite(&fileNameLen, sizeof(fileNameLen), 1, fp);
					}
					else {
						fileNameLen = fineName.length();
						fwrite(&fileNameLen, sizeof(fileNameLen), 1, fp);
						fwrite(fineName.c_str(), fileNameLen + 1, 1, fp);
					}
				};
				// アルベドテクスチャのファイル名情報を書き込む。
				WriteTextureFileNameInfo(mat.albedoMapFileName);
				// 法線マップ
				WriteTextureFileNameInfo(mat.normalMapFileName);
				// スペキュラマップのファイル名情報を書き込む。
				WriteTextureFileNameInfo(mat.specularMapFileName);
				// リフレクションマップ。
				WriteTextureFileNameInfo(mat.reflectionMapFileName);
				// 屈折マップ。
				WriteTextureFileNameInfo(mat.refractionMapFileName);

			}
			// 続いて頂点バッファを書き込んでいく。
			for( int vertNo = 0; vertNo < m_meshParts[meshPartsNo].vertexBuffer.size(); vertNo++){
				tkmFileFormat::SVertex vertex;
				auto& vertexTmp = m_meshParts[meshPartsNo].vertexBuffer[vertNo];
				vertex.pos[0] = vertexTmp.pos.x;
				vertex.pos[1] = vertexTmp.pos.y;
				vertex.pos[2] = vertexTmp.pos.z;
				vertex.normal[0] = vertexTmp.normal.x;
				vertex.normal[1] = vertexTmp.normal.y;
				vertex.normal[2] = vertexTmp.normal.z;

				vertex.uv[0] = vertexTmp.uv.x;
				vertex.uv[1] = vertexTmp.uv.y;

				vertex.weights[0] = vertexTmp.skinWeights.x;
				vertex.weights[1] = vertexTmp.skinWeights.y;
				vertex.weights[2] = vertexTmp.skinWeights.z;
				vertex.weights[3] = vertexTmp.skinWeights.w;

				vertex.indices[0] = vertexTmp.indices[0];
				vertex.indices[1] = vertexTmp.indices[1];
				vertex.indices[2] = vertexTmp.indices[2];
				vertex.indices[3] = vertexTmp.indices[3];
				
				// 頂点を書き込む
				fwrite( &vertex, sizeof(vertex), 1, fp);
			}

			// 続いてインデックスバッファ。
			// 最適化後は32ビットしかサポートしない。
			for (int matNo = 0; matNo < meshPartsHeader.numMaterial; matNo++) {
				std::uint32_t numPolygon = m_meshParts[meshPartsNo].indexBuffer32Array[matNo].indices.size() / 3;
				fwrite(&numPolygon, sizeof(numPolygon), 1, fp);
				const auto& indeces = m_meshParts[meshPartsNo].indexBuffer32Array[matNo].indices;
				for (int i = 0; i < indeces.size(); i++) {
					// インデックスバッファを書き込む
					int index = indeces[i] + 1;	// 3dsMaxから出力される際に＋１されているので、それに合わせて戻す。
					fwrite(
						&index,
						sizeof(std::uint32_t),
						1,
						fp
					);
				}
			}
		}

		fclose(fp);
		return true;
	}
	void TkmFile::Optimize()
	{
		// 同じマテリアルを使っているメッシュをひとまとめにする。
		// 最悪のケースでマテリアルの数分だけメッシュが存在するので、
		// メッシュの最大数を調べておく。
		int maxMesh = 0;
		for (SMesh& mesh : m_meshParts) {
			maxMesh += mesh.materials.size();
		}
		std::vector< SMesh > optimizeMeshParts;
		// 最適化済みのメッシュを記憶する領域を最悪のケースで確保しておく。
		optimizeMeshParts.reserve(maxMesh);
		
		std::map<int, SMesh*> meshMap;
		for (SMesh& mesh : m_meshParts) {
			for (int matNo = 0; matNo < mesh.materials.size(); matNo++) {
				int matId = mesh.materials[matNo].uniqID;
				auto it = meshMap.find(matId);
				if (it == meshMap.end()) {
					// 新規マテリアル。
					SMesh optMesh;
					optMesh.materials.emplace_back(mesh.materials[matNo]);
					optMesh.vertexBuffer = mesh.vertexBuffer;
					optMesh.indexBuffer32Array.resize(1);
					optMesh.isFlatShading = m_meshParts[0].isFlatShading;
					// もう16bitのイ0ンデックスバッファは使わない。
					if (mesh.indexBuffer32Array.size() != 0) {
						for (int index : mesh.indexBuffer32Array[matNo].indices) {
							optMesh.indexBuffer32Array[0].indices.emplace_back(index);
						}
					}
					if (mesh.indexBuffer16Array.size() != 0) {
						for (int index : mesh.indexBuffer16Array[matNo].indices) {
							optMesh.indexBuffer32Array[0].indices.emplace_back(index);
						}
					}
					optimizeMeshParts.emplace_back(optMesh);
					meshMap.insert(std::pair<int, SMesh*>(matId, &optimizeMeshParts.back()));
				}
				else {
					// 重複マテリアルなので統合する。
					// 頂点バッファを連結。
					SMesh* optMesh = it->second;
					int baseIndex = optMesh->vertexBuffer.size();
					optMesh->vertexBuffer.insert(
						optMesh->vertexBuffer.end(), 
						mesh.vertexBuffer.begin(),
						mesh.vertexBuffer.end()
					);

					// インデックスバッファを連結。
					if (mesh.indexBuffer32Array.size() != 0) {
						for (int index : mesh.indexBuffer32Array[matNo].indices) {
							optMesh->indexBuffer32Array[0].indices.emplace_back(index + baseIndex);
						}
					}
					if (mesh.indexBuffer16Array.size() != 0) {
						for (int index : mesh.indexBuffer16Array[matNo].indices) {
							optMesh->indexBuffer32Array[0].indices.emplace_back(index + baseIndex);
						}
					}
				}
			}
		}
		// 最適化済みメッシュに差し替える。
		m_meshParts = optimizeMeshParts;
	}
}