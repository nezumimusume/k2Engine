/*!
 * @brief	tkmエクスポーター。
 */
 

--tkmファイルのバージョン
TKM_VERSION = 100

struct Vertex(
	pos = Point3,		--頂点座標
	normal = Point3,	--法線
	uv = Point2,		--UV座標
	weights = #(),		--スキンウェイト
	indices = #(),		--ボーンインデックス
	texVertNo
)
--マテリアル。
struct tkMaterial(
	id,						--マテリアルID
	albedoMap = string,		--アルベドマップ
	normalMap = string,		--法線マップ
	specMap = string,		--スペキュラマップ
	reflectionMap = string,	--リフレクションマップ
	refractionMap = string	--屈折マップ
)
struct IndexBuffer(
	indices = #()
)
--メッシュパーツ。
struct MeshParts(
	materials = #(),		--マテリアル。
	vertexBuffer = #(),		--頂点バッファ。
	indexBufferArray = #()	--インデックスバッファの配列(マテリアルの数分だけあります。)
)

--モデルデータ。
struct ModelData(
	meshParts = #()		--メッシュパーツ。
)

struct TkmExporterForScript(
private
	outputFolder,		--出力先のフォルダ。
	md = ModelData,		--メッシュデータ。
	ms = Meshparts,		--メッシュパーツ。
	newVertNosInFace = #(),	--
	textureFilePaths = #(),	--テクスチャのファイルパスのリスト。
	cloneMeshObj,		--クローンされたメッシュ
	m_isFlatShading,	--フラットシェーディングを行う？
	--ファイルパスからファイル名を抽出して、ファイルに出力する。
	fn WriteFileToFileNameFromFilePath file filePath = 
	(
		hoge = filePath as string
		tokens = FilterString hoge "\/"
		nameLen = 0
		if tokens.count != 0 then( 
			nameLen = tokens[tokens.count].count
			WriteLong file nameLen --ファイル名の長さを出力。
			WriteString file tokens[tokens.count]
			
		)else(
			WriteLong file 0 --ファイル名の長さを出力。
		)

	),
	--頂点バッファの構築。
	fn BuildMeshPatrs_VertexBuffer c_node meshObj numFace =(
		--maxでは、テクスチャ頂点という概念があり、
		--UV座標のみ別の頂点バッファになっている。
		--tkEngine2では別の頂点バッファになっていると非効率なため、
		--二つの頂点バッファを結合して一つにする。

		--3DSMAX上での頂点バッファを構築する。
		skinMod = c_node.modifiers[skin]
		if skinMod != undefined then(
			--スキンあり。
			max modify mode
			modPanel.setCurrentObject skinMod
		)
		maxVertexBuffer = #()
		numVert = getNumVerts meshObj
		for vertNo = 1 to numVert do(
			--頂点座標
			pos = getVert meshObj vertNo
			--法線
			normal = getNormal meshObj vertNo
			--正規化されてないね。
			normal = normalize normal
			
			if skinMod != undefined then(
				--スキンがあるときは、骨で動かされているのでトランスフォームでの動きを除去する？
				--ホンマか？要確認。
				invTrans = inverse c_node.transform
				pos = pos * invTrans
				invRot = c_node.rotation as Matrix3
			--	invRot = inverse invRot
				normal = normal * invRot
			)
			
		
			--UV座標
		--	uvw = gettvert meshObj vertNo
		--	uv = Point2 uvw.x uvw.y
			--スキンウェイト
			skinWeight = #(0.0, 0.0, 0.0, 0.0)
			indices = #(-1, -1, -1, -1)
			
			if skinMod != undefined then(
				--スキンあり。
				numWeight = skinOps.GetVertexWeightCount skinMod vertNo
				if numWeight > 4 then (
					--スキンウェイトの数が4を超えています。
					numWeight = 4
				)
				for skinIndex = 1 to numWeight do (
					skinWeight[skinIndex] = skinOps.GetVertexWeight skinMod vertNo skinIndex
					sysytemBoneIndex = skinOps.GetVertexWeightBoneID skinMod vertNo skinIndex
					--第三引数は0でいいのかどうかか不明。
					--なにか問題が起きたら1にしてみよう。
					boneName = skinOps.GetBoneName skinMod sysytemBoneIndex 0
					boneNo = GetBoneIndex boneName
					indices[skinIndex] = boneNo
				)
				--スキンウェイトが4以上なら正規化する？
			)	
			--頂点作成
			v = vertex pos:pos normal:normal weights:skinWeight indices:indices
			append maxVertexBuffer v
		)
		--続いてテクスチャ頂点バッファを作成する。
		maxTVertexBuffer = #()
		numTVert = getNumTVerts meshObj
		for vertNo = 1 to numTVert do(
			--UV座標
			uvw = gettvert meshObj vertNo
			uv = Point2 uvw.x uvw.y
			append maxTVertexBuffer uv
		)
		--tkEngineで利用する頂点バッファを構築する。
		newVertNo = numVert + 1
		
		for faceNo = 1 to numFace do (
			tvertNos = getTVFace meshObj faceNo
			vertNos = getFace meshObj faceNo
			for i = 1 to 3  do(
				maxVertex = maxVertexBuffer[vertNos[i]]
				maxTVertex = maxTVertexBuffer[tvertNos[i]]
				v = vertex pos:maxVertex.pos normal:maxVertex.normal weights:maxVertex.weights indices:maxVertex.indices uv:maxTVertex texVertNo:tvertNos[i]
				--すでに何かのポリゴンで使用されている頂点　かつ　テクスチャ頂点番号が違う。
				--新しい頂点として追加する。
				vertNoInFace = vertNos[i]
				if ms.vertexBuffer[vertNos[i]] == undefined then(
					--未登録の頂点
					ms.vertexBuffer[vertNos[i]] = v
				)else if m_isFlatShading == true then(
					--フラットシェーディングの場合は重複頂点は許さないので、頂点を増やす。
					ms.vertexBuffer[newVertNo] = v
					vertNoInFace = newVertNo
					newVertNo = newVertNo + 1
				)else if ms.vertexBuffer[vertNos[i]].texVertNo != tvertNos[i] then(
					--登録済みだけど、テクスチャ番号が異なっているので頂点を増やす。
					ms.vertexBuffer[newVertNo] = v
					vertNoInFace = newVertNo
					newVertNo = newVertNo + 1
				)
				append newVertNosInFace vertNoInFace
			)
		)
	),
	fn GetMaterialType material = 
	(
		if material.classID[1] == 249140708  then(
			if material.classID[2] == 1630788338  then(
				--DirectXシェーダーマテリアル
				return 1
			)
		)
		if material.classID[1] == 512  then(
			if material.classID[2] == 0  then(
				--マルチマテリアル
				return 2
			)
		)
		--未対応のマテリアル
		return 0
	),
	--マテリアル情報を構築する。
	fn BuildMeshParts_Material c_node meshObj numFace  =
	(
		
		if c_node.material != undefined then(
			matType = GetMaterialType c_node.material
			if matType == 1 then(
				--DirectXシェーダーマテリアル
				diffMap = c_node.material.g_albedo
				normalMap = c_node.material.g_normal 
				specMap = c_node.material.g_metallicAndSmoothMap
				
				albedoFileName = ""
				if diffMap != undefined then(
					albedoFileName = diffMap.fileName
				)
				normalMapFileName = "" 
				if c_node.material.g_useNormalMap == true then(
					if normalMap != undefined then(
						normalMapFileName = normalMap.fileName
					)
				)
				specMapFileName = "" 
				if specMap != undefined then(
					specMapFileName = specMap.fileName
				)

				-- 反射マップと屈折マップはオミット。
				reflectionMapFileName = ""
				refractionMapFileName = ""
				
				newMat = tkMaterial refractionMap:refractionMapFileName reflectionMap:reflectionMapFileName albedoMap:albedoFileName normalMap:normalMapFileName specMap:specMapFileName id:-1
				append ms.materials newMat
			)else if matType == 2 then(
				--マルチマテリアル。
				--面ごとに異なるマテリアルが貼り付けられている。
				--まずマテリアル数を取得。
				numSubMaterial = getNumSubMtls c_node.material
				matIds = #()
				for faceNo = 1 to numFace do (
					matId  = getFaceMatID meshObj faceNo
					
					if numSubMaterial > 0 then(
						regist = 0
						--登録済みのマテリアルかどうか調べる。
						for tkMat in ms.materials do (
							if matId == tkMat.id then(
								--登録済み
								regist = 1
							)
						)
						if regist == 0 then(
							--未登録のマテリアルなので、追加する。
							mat = getSubMtl c_node.material matId
							--マテリアルタイプを取得
							matType = GetMaterialType mat
							if matType == 0 then(
								throw "DirectXマテリアル以外は使えません。\nK2Engineについて.pdfの「K2Engine用3Dモデルのmaxマテリアルの設定」を参照してください。"
							)
					
							diffMap = mat.g_albedo
							normalMap = mat.g_normal 
							specMap = mat.g_metallicAndSmoothMap

							albedoFileName = ""
							if diffMap != undefined then(
								albedoFileName = diffMap.fileName
							)
							normalMapFileName = "" 
							if normalMap != undefined then(
								normalMapFileName = normalMap.fileName
							)
							specMapFileName = "" 
							if specMap != undefined then(
								specMapFileName = specMap.fileName
							)

							-- 反射マップと屈折マップはオミット。
							reflectionMapFileName = ""
							refractionMapFileName = ""
							
							newMat = tkMaterial refractionMap:refractionMapFileName reflectionMap:reflectionMapFileName albedoMap:albedoFileName normalMap:normalMapFileName specMap:specMapFileName id:matId
							append ms.materials newMat
						)
					)
				)
			)else(
				throw  "DirectXマテリアル以外は使えません。\nK2Engineについて.pdfの「K2Engine用3Dモデルのmaxマテリアルの設定」を参照してください。"
			)
		)else(
			throw  "マテリアルが割り当てられていないオブジェクトを出力することはできません。\n"
		)
	),
	--インデックスバッファを構築する。
	--インデックスバッファの数はマテリアルの数分だけ構築されます。
	
	fn BuildMeshParts_IndexBuffer c_node meshObj numFace = 
	(
		--マテリアルの数分だけインデックスバッファを作成する。
		for tkMat in ms.materials do (
			indices = #()
			no = 1
			
			for faceNo = 1 to numFace do (
				matId  = getFaceMatID meshObj faceNo
				if tkMat.id == matId then(
					--現在調査中のマテリアルIDと同じ？
					--三角形を構成するインデックスを取得する。
					append indices newVertNosInFace[ no ]
					append indices newVertNosInFace[ no + 1 ]
					append indices newVertNosInFace[ no + 2 ]
				)else if tkMat.id == -1 then(
					--三角形を構成するインデックスを取得する。
					append indices newVertNosInFace[ no ]
					append indices newVertNosInFace[ no + 1 ]
					append indices newVertNosInFace[ no + 2 ]
				)
				no = no + 3
			)	
			--インデックスバッファを追加＾－＾
			append ms.indexBufferArray indices
		)
	),
	--メッシュパーツをビルドする。
	fn BuildMeshParts c_node = 
	(

		newVertNosInFace = #()
		--メッシュ―パーツを作成。
		ms = Meshparts materials:#() indexBufferArray:#() vertexBuffer:#()	
		--集約しないとサブマテリアルＩＤが取れないぞいぞい。
		cloneMeshObj = copy c_node 
		cloneMeshObj = convertToMesh cloneMeshObj
		numFace = getNumFaces cloneMeshObj
		numTVert = getNumTVerts cloneMeshObj

		if numTVert > 0 then (
			--テクスチャ頂点があれば出力する。
			--マテリアル情報から構築する。
			BuildMeshParts_Material c_node cloneMeshObj numFace

			--頂点バッファを構築する。
			BuildMeshPatrs_VertexBuffer c_node cloneMeshObj numFace

			--インデックスバッファを構築する。
			BuildMeshParts_IndexBuffer c_node cloneMeshObj numFace

			--メッシュパーツを追加。
			append md.meshParts ms
		)
		delete cloneMeshObj
		cloneMeshObj=undefined
	),
	--テクスチャをDDSファイルにコンバートしていく。
	fn ConvertTextureToDDS srcTexFilePath =(
		if srcTexFilePath != "" then(
			currentDir = pathConfig.GetDir(#userScripts)
			command = currentDir+"\mk.bat"
			arg = "\"" + srcTexFilePath + "\" \"" + outputFolder + "\""
			ShellLaunch command arg
		)
	),
	fn InternalSave filePath = (
		
		--保存だよ
		file = fopen filePath "wb"
		--ヘッダーを出力
		WriteByte file TKM_VERSION
		if m_isFlatShading == true then(
			WriteByte file 1 
		)else(
			WriteByte file 0
		)
		--メッシュパーツの数を出力
		WriteShort file md.meshParts.count
		--メッシュパーツを出力していく。
		for meshNo = 1 to md.meshParts.count do(
			--マテリアルの数を出力する。
			numMat = md.meshParts[meshNo].materials.count
			WriteLong file numMat
			
			--頂点数を出力。
			numVertex = md.meshParts[meshNo].vertexBuffer.count
			WriteLong file numVertex

			--インデックスのサイズを出力
			indexSize = 0
			if numVertex < 65536  then (
				--2byte
				indexSize = 2
			)else(
				--4byte
				indexSize = 4
			)
			WriteByte file indexSize
			--パディング
			WriteByte file 0
			WriteByte file 0
			WriteByte file 0

			--マテリアルの情報を出力する。
			for tkMat in md.meshParts[meshNo].materials do (
				--ファイル名だけを抽出する。
				--アルベド。
				WriteFileToFileNameFromFilePath file tkMat.albedoMap
				--法線マップ
				WriteFileToFileNameFromFilePath file tkMat.normalMap
				--スペキュラマップ。
				WriteFileToFileNameFromFilePath file tkMat.specMap
				--リフレクションマップ。
				WriteFileToFileNameFromFilePath file tkMat.reflectionMap
				--屈折マップ
				WriteFileToFileNameFromFilePath file tkMat.refractionMap

				--テクスチャをコンバートリストに積んでいく
				appendIfUnique textureFilePaths tkMat.albedoMap
				appendIfUnique textureFilePaths tkMat.normalMap
				appendIfUnique textureFilePaths tkMat.specMap
				appendIfUnique textureFilePaths tkMat.reflectionMap
				appendIfUnique textureFilePaths tkMat.refractionMap
			)

			--頂点バッファを出力
			for v in md.meshParts[meshNo].vertexBuffer do (
				if v != undefined then (
					--頂点座標を出力。
					WriteFloat file v.pos.x
					WriteFloat file v.pos.y
					WriteFloat file v.pos.z
					--頂点法線を出力。
					WriteFloat file v.normal.x
					WriteFloat file v.normal.y
					WriteFloat file v.normal.Z
					--UV座標を出力
					WriteFloat file v.uv.x
					WriteFloat file v.uv.y
					--スキンウェイトを出力
					WriteFloat file v.weights[1]
					WriteFloat file v.weights[2]
					WriteFloat file v.weights[3]
					WriteFloat file v.weights[4]
					--ボーンインデックスを出力。
					WriteShort file v.indices[1]
					WriteShort file v.indices[2]
					WriteShort file v.indices[3]
					WriteShort file v.indices[4]
				)else(
					-- ダミーの頂点を出力する
					WriteFloat file 0.0f
					WriteFloat file 0.0f
					WriteFloat file 0.0f
					--頂点法線を出力。
					WriteFloat file 0.0f
					WriteFloat file 0.0f
					WriteFloat file 0.0f
					--UV座標を出力
					WriteFloat file 0.0f
					WriteFloat file 0.0f
					--スキンウェイトを出力
					WriteFloat file 0.0f
					WriteFloat file 0.0f
					WriteFloat file 0.0f
					WriteFloat file 0.0f
					--ボーンインデックスを出力。
					WriteShort file 0
					WriteShort file 0
					WriteShort file 0
					WriteShort file 0
				)
			)

			--インデックスバッファを出力する。
			for indexBuffer in md.meshParts[meshNo].indexBufferArray do (
				--ポリゴンの数を出力する。
				numPoly = indexBuffer.count / 3
				WriteLong file numPoly
				--インデックスバッファを出力。
				for index in indexBuffer do (
					if indexSize == 2 then (
						WriteShort file index #unsigned
					)else(
						WriteLong file index #unsigned
					)
				)
				
			)
		)			
	
		fclose file
		--テクスチャのコンバード
		for texFilePath in textureFilePaths do(
			ConvertTextureToDDS texFilePath
		)
	),
public
	fn SaveTkm nodes filePath isFlatShading=
	(
		m_isFlatShading = isFlatShading
		--選択しているノードを退避させる。
		nodesTmp = #()
		for p in nodes do (
			append nodesTmp p
		) 
		
		
		filePath = getSaveFileName caption:"" types: " (*.tkm)|*.tkm|All Files (*.*)|*.*|"
		if filePath != undefined then(
			try(
				--出力先のフォルダを取得。
				outputFolder = pathConfig.removePathLeaf filePath
				
				--スケルトンが取得できる？
				TrySaveSkeleton nodesTmp filePath

				md = ModelData meshParts:#()

				for p in nodesTmp do (
					--編集可能頂点に変換できるか調べる。
					if canConvertTo p Mesh then(
						--メッシュパーツをビルド				
						BUildMeshParts p
					)
				) 
				--保存。
				InternalSave filePath
				currentDir = pathConfig.GetDir(#userScripts)
				command = currentDir+"\mkTkm.bat"
				arg = "\"" + filePath + "\""
				ShellLaunch command arg
			)catch(
				if cloneMeshObj != undefined then(
					delete cloneMeshObj
					cloneMeshObj=undefined
				)
				errorMessage = getCurrentException()
				messageBox errorMessage
			)
	
		)
	)

)