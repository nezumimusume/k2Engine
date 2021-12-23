/*!
 * @brief	tkEngine関係のエクスポーター。
 */

pluginManager.loadClass FBXIMPORTER 
pluginManager.loadClass FbxExporter

include "sub/tkExporterCommon.ms"
include "sub/tksExporter.ms"
include "sub/tkaExporter.ms"
include "sub/tkmExporter.ms"
include "sub/tklExporter.ms"



rollout tkExporter "tkExporter" width:194 height:248
(
	editText 'animClipStartFrame' "" pos:[85,502] width:58 height:21 align:#left
	label 'lbl1' "start frame" pos:[17,505] width:60 height:16 align:#left
	groupBox 'grp1' "Animation clip" pos:[9,476] width:147 height:134 align:#left
	button 'btn_saveAnimationClip' "Save" pos:[22,571] width:120 height:28 align:#left
	label 'lbl2' "end frame" pos:[17,536] width:60 height:16 align:#left
	editText 'animClipEndFrame' "" pos:[85,534] width:58 height:21 align:#left
	button 'btn_saveSkeleton' "Save Skeleton" pos:[17,439] width:120 height:28 align:#left
	label 'lbl3' "Label" pos:[28,498] width:0 height:0 align:#left
	groupBox 'grp5' "Model" pos:[1,13] width:157 height:138 align:#left
	button 'btn_save_fbx' "Save(*.tkm)" pos:[22,33] width:120 height:28 align:#left
	groupBox 'grp10' "Level" pos:[2,159] width:158 height:242 align:#left
	button 'btn_load_locObject' "Load Object" pos:[21,250] width:120 height:28 align:#left
	editText 'levelName' "" pos:[18,216] width:120 height:24 align:#left
	label 'lbl4' "edit level name" pos:[18,195] width:121 height:16 align:#left
	button 'SaveLoc' "Save Level" pos:[20,319] width:120 height:28 align:#left
	groupBox 'grp6' "Animation" pos:[1,411] width:160 height:200 align:#left
	button 'btn_copy_locObject' "Copy Object" pos:[21,283] width:120 height:28 align:#left
	button 'btn_save_selected_fbx' "Save Selected(*.tkm)" pos:[22,79] width:120 height:28 align:#left
	checkButton 'ckb1' "CheckButton" pos:[42,133] width:0 height:0 align:#left
	checkbox 'flat_shading' "flat shading" pos:[22,116] width:128 height:29 align:#left
	button 'LoadLoc' "Load Level" pos:[20,362] width:120 height:28 align:#left
	groupBox 'grp21' "NaviMesh" pos:[0,619] width:160 height:70 align:#left
	button 'btn_save_tkn' "Save(*.tkn)" pos:[22,644] width:120 height:28 align:#left
	
	on  tkExporter open do
		(
			levelName.text = "level00"
		)
	
		/*
		 *  スケルトンの出力のボタンが押されたときの処理。
		 */
		on btn_saveSkeleton pressed do
		(
			if selection[1] == undefined then(
				Messagebox "出力するスケルトンのルートのオブジェクトを選択して下さい。。。"
			)else(
				g_objectParams = #()
				--アニメーション時間を0にする。
				animationRange = interval 0 animationRange.end
				slidertime = 0
				at time 0(
					--アニメーション進行中の処理。
					BuildOutputNodeInfo selection[1] -1
					SaveSkeleton()
				)
			)
		)
		/*
		* Levelを保存。
		*/
		on SaveLoc pressed do
		(
			--現在編集中のルートのレベルを取得。
			editLevelObj = getNodeByName levelName.text
			if editLevelObj == undefined then(
				Messagebox "保存するレベルが存在しません。"
			)else(
				select editLevelObj
			
				--スケルトンとして保存。
				g_objectParams = #()
				--アニメーション時間を0にする。
				animationRange = interval 0 animationRange.end
				slidertime = 0
				at time 0(
					--アニメーション進行中の処理。
					BuildOutputNodeInfo selection[1] -1
					SaveLevel()
				)
			)
		)

		fn LoadFbxFile filepath= 
		(
				if filepath != undefined then(
					print filepath
					editLevelObj = getNodeByName levelName.text
					if editLevelObj == undefined then(
						--ダミーを作る。
						editLevelObj = Dummy()
						editLevelObj.name = levelName.text
					)
					
					FBXImporterSetParam "ScaleConversion" false
					FBXImporterSetParam "UpAxis" "Z"
					ImportFile filepath #noPrompt
					importObj = $
			
					filename = getFilenameFile filepath
					
					--全てを編集可能ポリゴンに変換する。
					delObjArray = #()
					for i=1 to $selection.count do
					(
						p = convertToMesh($selection[i]);
						--編集可能ポリゴンに変換できなかった場合は、削除リストに積む。
						if p == undefined then(
							append delObjArray $selection[i]
						)
					)
					--編集可能ポリゴンに変換できなかったオブジェクトを削除。
					for i=1 to delObjArray.count do
					(
						delete delObjArray[i]
					)
					parentBox = box length:1 width:1 height:1
					p = convertToMesh(parentBox)
					importObj.parent = parentBox
					select p
					selectMore $.children
					--結合。
					while  $selection.count>1 do
					(
						attach p $selection[$selection.count];
					)
	
					--ダミーをレベルの下にぽいっとな。
					$selection[1].pivot = Point3 0 0 0
					selection[1].name = filename
					--addModifier selection[1] maxPlugin()	
					$selection[1].parent = editLevelObj
					return selection[1]
				)

		)

		/*
		* Levelを読み込み。
		*/
		on LoadLoc pressed do
		(
			filepath = getOpenFileName caption:"" types: "tkl (*tkl)|*.tkl|All Files (*.*)|*.*|"
			if filepath != undefined then
			(
			pos = findString filepath "level"
			modelPath = substring filepath 1 (pos-1)
			modelPath = modelPath + "modelData\\"

			if filepath != undefined then
			(
				file = fopen filepath "rb"
				try(
					--ヘッダーを読み込み。
					--tklバージョンを読み込み。
					fileTklVersion = ReadLong file
					if TKL_VERSION != fileTklVersion do (
						messageBox "tkExporterとtklファイルのバージョンが異なります。"
						--tklに機能追加があった時に旧バージョンの読み込み処理は別の何処かでやるとか？
						--if文で読み込みスキップとかするかぁ～。
					)
					paramCount = ReadLong file

					print paramCount
					for i = 1 to paramCount do(
						objParam = ObjectParam "" true true 0 #() 0 #() 0 #() #() 0 #() Matrix3 Matrix3 -1 Node

						nameCount = ReadByte file
						objParam.objName = ReadString file
						objParam.parentId = ReadLong file

						--バインドポーズを読み込み。
						bindx = #(0,0,0)
						bindy = #(0,0,0)
						bindz = #(0,0,0)
						for i = 1 to 4 do(
							bindx[i] = ReadFloat file 
							bindy[i] = ReadFloat file 
							bindz[i] = ReadFloat file 
						)

						--バインドポーズの逆行列を読み込み。
						for i = 1 to 4 do(
							x = ReadFloat file 
							y = ReadFloat file 
							z = ReadFloat file
						)
						objParam.isShadowCaster = ReadByte file
						objParam.isShadowReceiver = ReadByte file
						--intパラメータの数
						objParam.numIntParam = ReadLong file
						--intパラメータ
						for i = 1 to objParam.numIntParam do (
							intParam = ReadLong file
							append objParam.intParams  intParam
						)
						--floatパラメータの数
						objParam.numFloatParam = ReadLong file
						--floatパラメータ
						for i = 1 to objParam.numFloatParam do (
							floatParam = ReadFloat file
							append objParam.floatParams  floatParam
						)
						 --stringパラメータ
						objParam.numStringParam = ReadLong file
						for i = 1 to objParam.numStringParam do (
							--stringパラメータの長さ。
							strlength = ReadLong file 
							append objParam.stringParamLength strlength
							--stringパラメータ。
							str = ReadString file
							append objParam.stringParams str
						)
						--vector3パラメータ
						objParam.numVec3Param = ReadLong file
						for i = 1 to objParam.numVec3Param do (
							x = ReadFloat file 
							y = ReadFloat file 
							z = ReadFloat file
							append objParam.vec3Params [x,y,z]
						)
						--オブジェクトを配置
						if objParam.parentId == -1 do continue
						fbxPath = modelPath + objParam.objName  + ".FBX"
						levelObj = LoadFbxFile(fbxPath)

						myTransform = levelObj.transform
						myTransform.row1 = [bindx[1],bindy[1],bindz[1]]
						myTransform.row2 = [bindx[2],bindy[2],bindz[2]]
						myTransform.row3 = [bindx[3],bindy[3],bindz[3]]
						myTransform.row4 = [bindx[4],bindy[4],bindz[4]]
						levelObj.transform = myTransform

						levelObj.modifiers[maxPlugin].caster = objParam.isShadowCaster
						levelObj.modifiers[maxPlugin].receiver = objParam.isShadowReceiver	
						
						levelObj.modifiers[maxPlugin].floatList = objParam.floatParams
						levelObj.modifiers[maxPlugin].strList = objParam.stringParams
						levelObj.modifiers[maxPlugin].vec3List = objParam.vec3Params

					)	
				)catch(
					Messagebox "ファイルの読み込みに失敗しました"
					format "*** % ***\n" (getCurrentException())
				)
				FClose file
			)
			)
		)
		
		/*
		 *  アニメーションクリップの保存のボタンが押されたときの処理。
		 */
		on btn_saveAnimationClip pressed do
		(
			--とりあえず全クリップ出力する。
			skeletonRoot = GetSkeletonRoot objects
			
			if skeletonRoot == undefined  then(
				--シーンにスキンモディファイアが見つからなかった。
				if selection[1] == undefined  then(
					Messagebox "スケルトンのルートのオブジェクトの自動判別ができませんでした。\nスケルトンのルートのオブジェクトを選択してアニメーションクリップを保存してください。\n"
					return 0
				)
				skeletonRoot = selection[1]
			)
			g_objectParams = #()
			--アニメーション時間を0にする。
			startFrame = animClipStartFrame.text as integer
			endFrame = animClipEndFrame.text as integer
			animationRange = interval startFrame endFrame
			slidertimer = 0
			animate on(
				--アニメーション進行中の処理。
				BuildOutputNodeInfo skeletonRoot -1
				try(
					select skeletonRoot
					SaveAnimationClip()
				)catch(
					messageBox "ファイルの保存に失敗しました"
				)
			)
		)
		
		function selectHierarchy =
		(
			for p in selection do
			(
				if p.children != undefined do
				(
					selectmore p.children
				)
			)
		)
		
		/*
		 *　配置するオブジェクトをコピー
		 */
		on btn_copy_locObject pressed do
		(
			if selection.count == 0  then(
				Messagebox "コピーするオブジェクトを選択してください。"
			)else(
				selectHierarchy()
				parentObj = copy selection[1]
				parentObj.name = selection[1].name
				for obj in 2 to selection.count do (
					cloneObj = copy selection[obj]
					cloneObj.name = selection[obj].name
					cloneObj.parent = parentObj
					parentObj = merge( parentObj cloneObj )
					deselect cloneObj
				)
				select parentObj
			)
		)


		/*
		 * 配置するオブジェクトのロード。
		 */
		 on btn_load_locObject pressed do
		(
			if  levelName.text == "" then(
				Messagebox "編集するレベルの名前を入力して下さい。"
			)else(
				filepath = getOpenFileName caption:"" types: "fbx (*fbx)|*.fbx|All Files (*.*)|*.*|"
				LoadFbxFile(filepath)
			)
		)

		/*
		 *  FBX出力用の設定を行う。
		 */
		fn SetupFbxExportSettings = 
		(
			--他にもオプションが必要なら追加していってください。
			FBXExporterSetParam "ResetExport"
			FBXExporterSetParam "ScaleFactor" 1.0
			FBXExporterSetParam "UpAxis" "Z"
			FBXExporterSetParam "TangentSpaceExport" true
			FBXExporterSetParam "Triangulate" true
			FBXExporterSetParam "ASCII" true
		)
		
		
		/*!
		 * @brief	FBXのSaveのボタンが押されたときの処理。
		 */
		on btn_save_fbx pressed do
		(
			--全オブジェクトをtkmファイルとして出力する。
			expoter = TkmExporterForScript()
			expoter.SaveTkm objects filepath flat_shading.checked
		)
		
		/*!
		 * @brief	FBXのSave Selectedのボタンが押されたときの処理。
		 */
		on btn_save_selected_fbx pressed do
		(
			--選択されているオブジェクトをtkmファイルとして出力する。
			expoter = TkmExporterForScript()
			expoter.SaveTkm selection filepath flat_shading.checked
		)
		/*!
		 * @brief	NaviMeshのSaveのボタンが押されたときの処理。
		 */
		on btn_save_tkn pressed do
		(
			filePath = getSaveFileName caption:"" types: " (*.tkn)|*.tkn|All Files (*.*)|*.*|"
			if filePath != undefined then(
				-- tknファイルを保存する。
				exportFile(filePath) #noPrompt
			)
		)
	)
	addrollout tkExporter style:#(#style_titlebar, #style_sysmenu)