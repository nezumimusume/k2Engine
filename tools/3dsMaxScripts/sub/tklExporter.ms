--レベルデータのエクスポータ。
TKL_VERSION = 100
     /*
	 * レベルデータを保存。
	 */
     fn SaveLevel = 
     (
         filepath = getSaveFileName caption:"" types: "level (*.tkl)|*.tkl|All Files (*.*)|*.*|"
         
         if filepath != undefined then
         (
             
             file = fopen filepath "wb"
             
             try(
                 --バージョンを出力
                 WriteLong file TKL_VERSION
                 --ヘッダーを出力
                 WriteLong file g_objectParams.count
                 
                 for o in g_objectParams do (
                     WriteByte file o.objName.count
                     WriteString file o.objName
                     WriteLong file o.parentId
                     --バインドポーズを出力。
                     for i = 1 to 4 do(
                         WriteFloat file o.bindPose[i].x
                         WriteFloat file o.bindPose[i].y
                         WriteFloat file o.bindPose[i].z
                     )
                     --バインドポーズの逆数を出力。
                     for i = 1 to 4 do(
                         WriteFloat file o.invBindPose[i].x
                         WriteFloat file o.invBindPose[i].y
                         WriteFloat file o.invBindPose[i].z
                     )
                     --シャドウキャスターフラグを出力。
                     if o.isShadowCaster == true then
                        WriteByte file 1
                     else
                        WriteByte file 0
                    --シャドウレシーバーフラグを出力。
                    if o.isShadowReceiver == true then
                        WriteByte file 1
                    else
                        WriteByte file 0
                    --intパラメータの数を出力。
                    WriteLong file o.numIntParam
                    --intパラメータを出力。
                    for i = 1 to o.numIntParam do (
                        WriteLong file o.intParams[i]
                    )
                    --floatパラメータの数を出力。
                    WriteLong file o.numFloatParam
                    --floatパラメータを出力。
                    for i = 1 to o.numFloatParam do (
                        WriteFloat file o.floatParams[i]
                    )
                    --stringパラメータの数を出力。
                    WriteLong file o.numStringParam
                    for i = 1 to o.numStringParam do (
                      --stringパラメータの長さ。
                      WriteLong file o.stringParamLength[i]
                      --stringパラメータ。
                      WriteString file o.stringParams[i]
                    )
                    --vector3パラメータの数。
                    WriteLong file o.numVec3Param
                    --vector3パラメータ。
                    for i = 1 to o.numVec3Param do (
                        WriteFloat file o.vec3Params[i].x
                        WriteFloat file o.vec3Params[i].y
                        WriteFloat file o.vec3Params[i].z
                    )

                 )
             )catch(
                 Messagebox "ファイルの保存に失敗しました"
                 format "*** % ***\n" (getCurrentException())
             )
             FClose file
         )
     )