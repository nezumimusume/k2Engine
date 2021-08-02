/*!
 * @brief	デバッグモードシェーダー。
 */


/////////////////////////////////////////////////////////////
// 定数バッファ。
/////////////////////////////////////////////////////////////
/*!
 * @brief	頂点シェーダー用の定数バッファ。
 */
cbuffer VSCb : register(b0){
	float4x4 mVP;
};

/*!
 * @brief	頂点構造体。
 */
struct VSInput
{
	//float4 Position : SV_Position;			//頂点座標。
	//SV_Positionは等セマンティクスと呼ばれる
	//セマンティクスの名前はSVがつかなければなんでもいい
	float3 position : Position;			//頂点座標。
	float3 color : Color;				//カラー。
};

//SV_Positionで返す用の構造体
struct PSInput
{
	//この構造体はSV_Positionが必要
	//変数名と自分で名付けたセマンティクスはVSInputとかぶってもおｋ
	float4 position : SV_Position;		//頂点座標。
	float3 color : Color;				//カラー。
};

//VSMainの戻り値はSV_Positionが必要
PSInput VSMain(VSInput input)
{
	PSInput output;
	output.position = mul(mVP, float4(input.position, 1.0f));
	//output.position = mul(mVP,input.position);
	output.color = input.color;
	return output;
}
//PSMainの戻り値はSV_Targetが必要
//戻り値は色（float4）である必要がある
float4 PSMain(PSInput input) : SV_Target
{
	//output.positionはこの関数に来るまでに使い終わってる
	//別に使っていいよ

	//float4にする
	//1はアルファ
    return float4(1.0, 0, 0, 1);
	//return float4(input.color,1);
}