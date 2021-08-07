--アニメーションクリップエクスポーター。

--アニメーションキーフレーム
struct KeyFrame(
	boneId,
	t,
	localTransform
)
--アニメーションクリップ
struct AnimationClip(
	animClipName,		--アニメーションクリップ名
	keyFrames = #()		--キーフレームのリスト
)


startFrame = 0
endFrame = 0

/*
*  アニメーションクリップの保存。
*/
fn SaveAnimationClip=
(
    filepath = getSaveFileName caption:"" types: "animation clip (*tka)|*.tka|All Files (*.*)|*.*|"

    if filepath != undefined then
    (
        
        numKey = endFrame - startFrame + 1
        if numKey <= 0 then(
            --1フレームだけ出す。
            endFrame = startFrame
            numKey = 1
        )
        deltaTime = 1.0/30
        keyTime = 0.0
        
        numKey *= g_objectParams.Count
        matTransList = #()
        matRotList = #()

        file = fopen filepath "wb"
        --ヘッダーを出力
        WriteLong file numKey
        --アニメーションイベントの数を出力
        numNoteTrack = numNoteTracks selection[1]

        if numNoteTrack > 0 then(
            --ノートキーの数を出力
            nt = getNoteTrack selection[1] 1
            WriteLong file nt.keys.count
            for k in nt.keys do(
                --イベントが発生する時間を出力
                if k.time >= numKey then(
                    exit
                )
                invokeTime = k.time * deltaTime
                
                WriteFloat file invokeTime
                WriteLong file k.value.count
                WriteString file k.value
            )
        )else(
            WriteLong file 0
        )
        
        fclose file
        --アニメーションの情報を出力。
        slidertime=startFrame

        file = fopen filepath "ab"
        
        for i=startFrame to endFrame do(
            boneId = 0
            keyTime = (i - startFrame) * deltaTime
            
            for o in g_objectParams do (
                n = o.n

                --出力
                WriteLong file boneId -- 骨ID
                WriteFloat file keyTime -- 時間
                
                --ボーンローカル行列に変換する。
                
                transformMatrix = Matrix3 1
                if n.parent == undefined then(
                    transformMatrix =n.transform
                )else(
                    transformMatrix = n.transform * inverse n.parent.transform
                )
                for i = 1 to 4 do(
                    
                    WriteFloat file transformMatrix[i].x
                    WriteFloat file transformMatrix[i].y
                    WriteFloat file transformMatrix[i].z
                )
                boneId +=1
                
            )
            slidertime += 1
        )
        
        fclose file

    )
)