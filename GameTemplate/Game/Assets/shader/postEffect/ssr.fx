/*!
 * @brief	SSR
 */

cbuffer cb : register(b0)
{
    float4x4 mvp;       // MVP�s��
    float4 mulColor;    // ��Z�J���[
};

struct VSInput
{
    float4 pos : POSITION;
    float2 uv  : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv  : TEXCOORD0;
};

cbuffer ssrCb : register(b1){
	float4x4 mViewProjInv;				//�r���[�v���W�F�N�V�����s��̋t�s��B
	float4x4 mViewProj;					//�r���[�v���W�F�N�V�����s��B
	float4 cameraPosInWorld;			//���[���h���W�n�ł̃J�����̎��_�B
};


Texture2D<float4> sceneTexture : register(t0); 			// �V�[���̃e�N�X�`��
Texture2D<float4> depthTexture : register(t1);			//�[�x�e�N�X�`���B
Texture2D<float4> normalTexture : register(t2);			//�@���e�N�X�`���B
Texture2D<float4> metallicSmoothTexture : register(t3);	//���^���b�N�X���[�X�e�N�X�`���B

sampler Sampler : register(s0);

/*!
 * @brief	UV���W�Ɛ[�x�l���烏�[���h���W���v�Z����B
 *@param[in]	uv				uv���W
 *@param[in]	zInScreen		�X�N���[�����W�n�̐[�x�l
 *@param[in]	mViewProjInv	�r���[�v���W�F�N�V�����s��̋t�s��B
 */
float3 CalcWorldPosFromUVZ( float2 uv, float zInScreen, float4x4 mViewProjInv )
{
	float3 screenPos;
	screenPos.xy = (uv * float2(2.0f, -2.0f)) + float2( -1.0f, 1.0f);
	screenPos.z = zInScreen;//depthMap.Sample(Sampler, uv).r;
	
	float4 worldPos = mul(mViewProjInv, float4(screenPos, 1.0f));
	worldPos.xyz /= worldPos.w;
	return worldPos.xyz;
}
float GetRandomNumber(float2 texCoord, float Seed)
{
	return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}
/*!
 * @brief ���_�V�F�[�_�[
 */
PSInput VSMain(VSInput In)
{
    PSInput psIn;
    psIn.pos = mul(mvp, In.pos);
    psIn.uv = In.uv;
    return psIn;
}
float4 PSMain(PSInput In) : SV_Target0
{
	//�����x�ƃX���[�X���擾�B
	float4 metallicSmooth = metallicSmoothTexture.Sample(Sampler, In.uv);
	// �X���[�X0.5�ȏ�̃T�[�t�F�C�X�ł̂݉f�荞�݂��N�����悤�ɂ���B
	clip( metallicSmooth.a - 0.5f);
	//�s�N�Z���̃��[���h���W���v�Z����B
	float3 worldPos = CalcWorldPosFromUVZ(
		In.uv,
		depthTexture.Sample(Sampler, In.uv).r,
		mViewProjInv
	);

	float4 sceneColor = sceneTexture.Sample(Sampler, In.uv);
	
	//�s�N�Z���̖@�����擾�B
	float3 normal = normalTexture.Sample(Sampler, In.uv).xyz;
	
	//�J�����̎��_����s�N�Z���Ɍ������x�N�g�����v�Z����B
	float3 toPixelDir = normalize( worldPos - cameraPosInWorld.xyz );
	//���˃x�N�g�������߂�B
	float3 toPixelDirReflect = reflect( toPixelDir, normal );
	float depth = depthTexture.Sample(Sampler, In.uv).r;

	//���˃x�N�g�����g���āA���C�}�[�`���O���s���B
	//���C�}�[�`���O�̃C�e���[�V�����񐔂Ɛ�����L�΂�������
	//��Œ萔�o�b�t�@�ɂ�����ŁH
	int maxRayNum = 20;
	float3 raystep = 800.0f/ maxRayNum * toPixelDirReflect;
	float maxThickness = 0.001f / maxRayNum;
	for( int step = 1; step < maxRayNum; step++){ //�������g�ƂԂ���̂ŁAstep��1����i�߂�B
		float3 rayPos = worldPos + step * raystep * lerp( 0.8f, 1.0f, GetRandomNumber(In.uv, 1.0f));
		float4 posInProj = float4(rayPos, 1.0f);
		//�r���[�v���W�F�N�V������Ԃɕϊ�����B
		posInProj = mul(mViewProj, posInProj);
		posInProj.xyz /= posInProj.w;
		float2 reflectUV = posInProj.xy * float2(0.5f, -0.5f) + 0.5f;
		// ���C��i�߂���̃s�N�Z����Z�l���擾
		float pixelZ = depthTexture.Sample(Sampler, reflectUV).r;
		if ( depth < pixelZ 						// ���������X�^�[�g�n�_��Z�l���Ԃ������s�N�Z������O�B
			&& pixelZ < posInProj.z    				// ���C��Z�l���A�s�N�Z����Z�l�̕��������� -> �܂背�C���Ԃ������\��������B
			&& posInProj.z - pixelZ < maxThickness	// �Ԃ������I�u�W�F�N�g�ɂ�����x�̉��s��������Ɖ��肵�āA���̉��s���͈͓̔��B
		) {
			// ���C���Ԃ������Ɣ��f����B
			// ��������Q���T���Ő��x���グ��B
			float sign = -1.0; 	// ����̓��C�̐i�s�����B
			for (int m = 1; m <= 8; ++m) {
				// 0.5��m�悷��B�����m��1�Ȃ烌�C�X�e�b�v��1/2�Am��2�Ȃ烌�C�X�e�b�v��1/4�i�ނ��ƂɂȂ�B
				// ����2���T���ł́A�Ԃ������n�_����A0.5->0.25->0.125->0.0625�ƃX�e�b�v��i�߂Ă����A
				// �Ԃ������s�N�Z���̐��x���グ�Ă����B
				rayPos += raystep * (sign * pow(0.5, m));
				// 0.5^m�i�߂����C�̃X�N���[����ԏ�̍��W�����߂�B
				posInProj = mul(mViewProj, float4(rayPos, 1.0f));
				posInProj.xyz /= posInProj.w;
				reflectUV = posInProj.xy * float2(0.5f, -0.5f) + 0.5f;
				float pixelZ = depthTexture.Sample(Sampler, reflectUV).r;
				// �Փ˂��Ȃ��Ȃ�����A���C�̐i�s�����𔽓]������B
				// �Փ˂��Ă�����A���̂܂ܐi�߂�B
				sign = posInProj.z - pixelZ > 0 ? -1 : 1;
			}
			float4 reflectColor = sceneTexture.Sample(Sampler, reflectUV);
			// SSR�̓e�N�X�`���͈̔͊O���T���v�����O����ƁA���������ʂ𓾂邱�Ƃ͂ł��Ȃ��̂ŁA
			// �͈͊O�ɋ߂Â��قǔ��˂��キ�Ȃ�悤�Ȍv�Z������B
			float t = 0.8f;
			t *= pow( 1.0f - min( 1.0f, abs(posInProj.x) ), 0.5f );
			t *= pow( 1.0f - min( 1.0f, abs(posInProj.y) ), 0.5f );
			return float4(reflectColor.xyz, metallicSmooth.a * t);
		
		}
	}
	clip(-1.0f);
	return float4(sceneColor.xyz, 0.0f);
}

Texture2D<float4> reflectTexture : register(t1);	// ���˃e�N�X�`���B
Texture2D<float4> albedoTexture : register(t2);		// �A���x�h�e�N�X�`���B

//�ŏI�����B
float4 PSFinal(PSInput In) : SV_Target0
{
	
	float4 reflectColor = reflectTexture.Sample(Sampler, In.uv);
	float4 sceneColor = sceneTexture.Sample(Sampler, In.uv);
	float4 albedoColor = albedoTexture.Sample(Sampler, In.uv);

	float3 reflectLig = reflectColor.xyz * reflectColor.a;
	reflectLig *= albedoColor.xyz;
	
	sceneColor.xyz += reflectLig;
	
	return sceneColor;

}
