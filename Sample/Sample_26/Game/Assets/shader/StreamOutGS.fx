// ストリームアウトシェーダー

struct GSPS_Input
{
    float4 pos : SV_Position;   // 頂点座標。
    float3 normal : NORMAL;     // 法線。
    float2 uv : TEXCOORD0;      // UV座標。
    float3 tangent : TANGENT;   // 接ベクトル。
    float3 biNormal : BINORMAL; // 従ベクトル。
};


