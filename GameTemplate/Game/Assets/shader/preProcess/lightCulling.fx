/*!
 * @brief ポイントライトの影響範囲をタイルベースで計算するコンピュートシェーダー
 */

// 一度に実行されるスレッド数
#define TILE_WIDTH  16
#define TILE_HEIGHT 16

// タイルの総数
#define TILE_SIZE (TILE_WIDTH * TILE_HEIGHT)

// ディレクションライト
struct DirectionalLight
{
    float3 color;       // ライトのカラー
    float3 direction;   // ライトの方向
};

// ポイントライト
struct PointLight
{
    float3 position;        // 座標
    int isUse;              // 使用中フラグ。
    float3 positionInView;  // カメラ空間での座標
    float3 color;           // カラー
    float3 attn;            // 減衰パラメータ。
};

// スポットライト
struct SpotLight
{
    float3 position;        // 座標
    int isUse;              // 使用中フラグ。
    float3 positionInView;  // カメラ空間での座標。
    int no ;                // ライトの番号。
    float3 direction;       // 射出方向。
    float range;            // 影響範囲。
    float3 color;           // ライトのカラー。
    float3 color2;          // 二つ目のカラー。
    float3 color3;          // 三つ目のカラー。
    float3 directionInView; // カメラ空間での射出方向。
    float3 rangePow;        // 距離による光の影響率に累乗するパラメーター。1.0で線形の変化をする。
                            // xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー。
    float3 angle;           // 射出角度(単位：ラジアン。xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー)。
    float3 anglePow;        // スポットライトとの角度による光の影響率に累乗するパラメータ。1.0で線形に変化する。
                            // xが一つ目のカラー、yが二つ目のカラー、zが三つ目のカラー。
};

static const int NUM_DIRECTIONAL_LIGHT = 4;  // ディレクションライトの数
static const int MAX_POINT_LIGHT = 256;      // ポイントライトの最大数
static const int MAX_SPOT_LIGHT = 256;      // スポットライトの最大数

// 定数バッファー
cbuffer cbCameraParam : register(b0)
{
    float4x4 mtxProj : packoffset(c0); // 投影行列
    float4x4 mtxProjInv : packoffset(c4); //  投影行列の逆行列
    float4x4 mtxViewRot : packoffset(c8);
    float4 screenParam : packoffset(c12); // スクリーンパラメーター（near, far, screenWidth, screenHeight）
};

cbuffer Light : register(b1)
{
    DirectionalLight directionalLight[NUM_DIRECTIONAL_LIGHT];   // ディレクションライト
    PointLight pointLight[MAX_POINT_LIGHT];                     // ポイントライト。
    SpotLight spotLight[MAX_SPOT_LIGHT];                        // スポットライト。
    float4x4 mViewProjInv;  // ビュープロジェクション行列の逆行列
    float3 eyePos;          // カメラの視点
    int numPointLight;      // ポイントライトの数。    
    float3 ambientLight;    // 環境光
    int numSpotLight;       // スポットライトの数。
};

// 入力
// 深度テクスチャ
Texture2D depthTexture : register(t0);

// 出力用のバッファー
RWStructuredBuffer<uint> rwPointLightIndices : register(u0); // ポイントライトインデックスバッファー
RWStructuredBuffer<uint> rwSpotLightIndices : register(u1);  // スポットライトインデックスバッファー

// 共有メモリ
groupshared uint sMinZ; // タイルの最小深度
groupshared uint sMaxZ; // タイルの最大深度
groupshared uint sTilePointLightIndices[MAX_POINT_LIGHT];   // タイルに接触しているポイントライトのインデックス
groupshared uint sTileNumPointLights;                       // タイルに接触しているポイントライトの数
groupshared uint sTileSpotLightIndices[MAX_SPOT_LIGHT];     //　タイルに接触しているスポットライトのインデックス
groupshared uint sTileNumSpotLights;                        //　タイルに接触しているスポットライトの数。
groupshared uint ligNum = 0;

/*!
 * @brief タイルごとの視推台平面を求める
 */
void GetTileFrustumPlane(out float4 frustumPlanes[6], uint3 groupId)
{
    // タイルの最大・最小深度を浮動小数点に変換
    float minTileZ = asfloat(sMinZ);
    float maxTileZ = asfloat(sMaxZ);

    
    float2 tileScale = screenParam.zw * rcp(float(2 * TILE_WIDTH));
    float2 tileBias = tileScale - float2(groupId.xy);

    float4 c1 = float4(mtxProj._11 * tileScale.x, 0.0, tileBias.x, 0.0);
    float4 c2 = float4(0.0, -mtxProj._22 * tileScale.y, tileBias.y, 0.0);
    float4 c4 = float4(0.0, 0.0, 1.0, 0.0);

    frustumPlanes[0] = c4 - c1; // Right
    frustumPlanes[1] = c4 + c1; // Left
    frustumPlanes[2] = c4 - c2; // Top
    frustumPlanes[3] = c4 + c2; // Bottom
    frustumPlanes[4] = float4(0.0, 0.0, 1.0, -minTileZ);
    frustumPlanes[5] = float4(0.0, 0.0, -1.0, maxTileZ);

    // 法線が正規化されていない4面についてだけ正規化する
    [unroll]
    for (uint i = 0; i < 4; ++i)
    {
        frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));
    }
}

/*!
 * @brief カメラ空間での座標を計算する
 */
float3 ComputePositionInCamera(uint2 globalCoords)
{
    float2 st = ((float2)globalCoords + 0.5) * rcp(screenParam.zw);
    st = st * float2(2.0, -2.0) - float2(1.0, -1.0);
    float3 screenPos;
    screenPos.xy = st.xy;
    screenPos.z = depthTexture.Load(uint3(globalCoords, 0.0f));
    float4 cameraPos = mul(mtxProjInv, float4(screenPos, 1.0f));

    return cameraPos.xyz / cameraPos.w;
}
/*!
 * @brief 現在調査中のタイルに含まれているスポットライトのインデックスの配列を作成
 */
void CreateSpotLightIndexArrayInTile(uint threadNoInTile, float4 frustumPlanes[6])
{
    for (uint lightIndex = threadNoInTile; lightIndex < MAX_SPOT_LIGHT; lightIndex += TILE_SIZE)
    {
        SpotLight light = spotLight[lightIndex];
        if( light.isUse ){
            // タイルとの判定
            bool inFrustum = true;
            for (uint i = 0; i < 6; ++i)
            {
                // ライトの座標と平面の法線とで内積を使って、
                // ライトと平面との距離（正負あり）を計算する
                float4 lp = float4(light.positionInView, 1.0f);
                float d = dot(frustumPlanes[i], lp);

                if( d < 0.0f){
                    // スポットライトが視推台の外にあるので、
                    // スポットライトが視推台の方向を向いているか調べる。
                    float t = dot(frustumPlanes[i].xyz, light.directionInView);
                    if( t < 0.0f){
                        // タイルに当たらない。
                        inFrustum = false;
                    }
                }
                // ライトと平面の距離を使って、衝突判定を行う
                inFrustum = inFrustum && (d >= -light.range);
            }

            // タイルと衝突している場合
            if (inFrustum)
            {
                // 衝突したポイントライトの番号を影響リストに積んでいく
                uint listIndex;
                InterlockedAdd(sTileNumSpotLights, 1, listIndex);
                sTileSpotLightIndices[listIndex] = lightIndex;
            }
        }
    }
}
/*!
 * @brief 現在調査中のタイルに含まれているポイントライトのインデックスの配列を作成。
 */
void CreatePointLightIndexArrayInTile(uint threadNoInTile, float4 frustumPlanes[6])
{
    for (uint lightIndex = threadNoInTile; lightIndex < MAX_POINT_LIGHT; lightIndex += TILE_SIZE)
    {
        PointLight light = pointLight[lightIndex];
        if( light.isUse ){
            // タイルとの判定
            bool inFrustum = true;
            for (uint i = 0; i < 6; ++i)
            {
                // ライトの座標と平面の法線とで内積を使って、
                // ライトと平面との距離（正負あり）を計算する
                float4 lp = float4(light.positionInView, 1.0f);
                float d = dot(frustumPlanes[i], lp);

                // ライトと平面の距離を使って、衝突判定を行う
                inFrustum = inFrustum && (d >= -light.attn.x);
            }

            // タイルと衝突している場合
            if (inFrustum)
            {
                // 衝突したポイントライトの番号を影響リストに積んでいく
                uint listIndex;
                InterlockedAdd(sTileNumPointLights, 1, listIndex);
                sTilePointLightIndices[listIndex] = lightIndex;
            }
        }
    }
}
/*!
 * @brief タイルに含まれているライトの番号を出力用のリストに書き込み。
 */
void WriteLightIindexInTileToList(uint threadNoInTile, uint2 frameUV)
{
    uint numCellX = (screenParam.z + TILE_WIDTH - 1) / TILE_WIDTH;
    uint tileIndex = floor(frameUV.x / TILE_WIDTH) + floor(frameUV.y / TILE_WIDTH) * numCellX;

    // ポイントライトの番号を出力  
    uint lightStart = numPointLight * tileIndex;
    for (uint lightIndex = threadNoInTile; lightIndex < sTileNumPointLights; lightIndex += TILE_SIZE)
    {
        rwPointLightIndices[lightStart + lightIndex] = sTilePointLightIndices[lightIndex];
    }
    // 最後に番兵を設定する
    if ((threadNoInTile == 0) && (sTileNumPointLights < numPointLight))
    {
        // -1で番兵
        rwPointLightIndices[lightStart + sTileNumPointLights] = 0xffffffff;
    }
    // 続いてスポットライトの番号を出力。
    lightStart = numSpotLight * tileIndex;
    for (uint lightIndex = threadNoInTile; lightIndex < sTileNumSpotLights; lightIndex += TILE_SIZE)
    {
        rwSpotLightIndices[lightStart + lightIndex] = sTileSpotLightIndices[lightIndex];
    }
    // 最後に番兵を設定する
    if ((threadNoInTile == 0) && (sTileNumSpotLights < numSpotLight))
    {
        // -1で番兵
        rwSpotLightIndices[lightStart + sTileNumSpotLights] = 0xffffffff;
    }
}
/*!
 * @brief CSMain
 */
[numthreads(TILE_WIDTH, TILE_HEIGHT, 1)]
void CSMain(
    uint3 groupId          : SV_GroupID,
    uint3 dispatchThreadId : SV_DispatchThreadID,
    uint3 groupThreadId    : SV_GroupThreadID)
{   
    // step-7 タイル内でのインデックスを求める
    // groupThreadIdはグループ内でのスレッド番号
    // これを使って、グループ内でのスレッド番号を計算する
    uint groupIndex = groupThreadId.y * TILE_WIDTH + groupThreadId.x;

    // 共有メモリを初期化する
    if(groupIndex == 0)
    {
        sTileNumPointLights = 0;
        sTileNumSpotLights = 0;
        sMinZ = 0x7F7FFFFF; // floatの最大値
        sMaxZ = 0;
    }

    // このスレッドが担当するピクセルのカメラ空間での座標を計算する
    uint2 frameUV = dispatchThreadId.xy;

    // ビュー空間での座標を計算する
    float3 posInView = ComputePositionInCamera(frameUV);

    // すべてのスレッドがここに到達するまで同期を取る
    GroupMemoryBarrierWithGroupSync();

    if(groupThreadId.x <= screenParam.z && groupThreadId.y <= screenParam.w){
        // タイルの最大・最小深度を求める
        // この処理は並列するスレッドすべてで排他的に処理される
        InterlockedMin(sMinZ, asuint(posInView.z));
        InterlockedMax(sMaxZ, asuint(posInView.z));
    }
    
    // ここで同期を取ることでタイルの最大・最小深度を正しいものにする
    GroupMemoryBarrierWithGroupSync();

    if(groupThreadId.x <= screenParam.z && groupThreadId.y <= screenParam.w){
        // タイルの視錘台を構成する6つの平面を求める
        float4 frustumPlanes[6];

        // この関数の中で、錘台を構成する6つ平面を計算している
        GetTileFrustumPlane(frustumPlanes, groupId);

        // タイルに含まれているポイントライトのインデックスの配列を作成する。
        CreatePointLightIndexArrayInTile( groupIndex, frustumPlanes);
        // タイルに含まれているスポットライトのインデックスの配列を作成する。
        CreateSpotLightIndexArrayInTile( groupIndex, frustumPlanes);
    }
    // ここで同期を取ると、sTileSpotLightIndicesとsTilePointLightIndicesに
    // タイルと衝突しているライトのインデックスが積まれている
    GroupMemoryBarrierWithGroupSync();

    if(groupThreadId.x <= screenParam.z && groupThreadId.y <= screenParam.w){
        // ライトインデックスを出力バッファーに出力
        WriteLightIindexInTileToList( groupIndex, frameUV);
    }
}
