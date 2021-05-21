--スケルトンのエクスポーター

/*
*  スケルトンの保存。
*@param[in]  filepath		保存するファイルパス。
*/
fn SaveSkeletonFilePath filepath = 
(
    file = fopen filepath "wb"
        
    try(
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
        )
    )catch(
        Messagebox "ファイルの保存に失敗しました"
    )
    FClose file
)

/*
*  スケルトンの保存。
*/
fn SaveSkeleton = 
(
    filepath = getSaveFileName caption:"" types: "skeleton (*tks)|*.tks|All Files (*.*)|*.*|"
    
    if filepath != undefined then
    (
        SaveSkeletonFilePath filepath
    )
)
/*
*  スキンモディファイアが設定されているノードがあったら、スケルトンを出力する。
*/
fn TrySaveSkeleton nodes filePath = 
(
    --スケルトンを選択出来たら取得する。
    skeletonRoot = GetSkeletonRoot nodes
    if skeletonRoot != undefined  then(
       
        --スケルトンを保存する。
        g_objectParams = #()
        --アニメーション時間を0にする。
        animationRange = interval 0 animationRange.end
        slidertime = 0
        at time 0(
            --アニメーション進行中の処理。
            BuildOutputNodeInfo skeletonRoot -1
            
            --ファイルパスの拡張子をtksに変更する。
            extPos = findString filepath "."
            filepath = replace filepath extPos 4 ".tks"
            
            --保存だ―。
            SaveSkeletonFilePath filepath
               
        )
    )
)