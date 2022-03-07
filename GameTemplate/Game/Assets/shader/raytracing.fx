
////////////////////////////////////////////////
// PBR���C�e�B���O���C���N���[�h�B
////////////////////////////////////////////////

#include "PBRLighting.h"

// ���_�\��
// �t�H�[�}�b�g��TkmFile.h��SVertex�Ɠ����ɂȂ��Ă���K�v������
struct SVertex
{
    float3 pos;
    float3 normal;
    float3 tangent;
    float3 binormal;
    float2 uv;
    int4 indices;
    float4 skinweigths;
};

// �J�����\����
// �萔�o�b�t�@�[�Ȃ̂�16�o�C�g�A���C�����g�ɋC��t���邱��
struct Camera
{
    float4x4 mViewProjInv;  // �r���[�v���W�F�N�V�����s��̋t�s��
    float3 pos;             // �J�������W
    float aspect;           // �A�X�y�N�g��
    float far;              // ������
    float near;             // �ߕ���
};

cbuffer rayGenCB :register(b0)
{
    Camera g_camera; // �J����
    
};

RaytracingAccelerationStructure g_raytracingWorld : register(t0);   // ���C�g�����[���h
Texture2D<float4> gAlbedoTexture : register(t1);                    // �A���x�h�}�b�v
Texture2D<float4> g_normalMap : register(t2);                       // �@���}�b�v
Texture2D<float4> g_specularMap : register(t3);                     // �X�y�L�����}�b�v
Texture2D<float4> g_reflectionMap : register(t4);                   // ���t���N�V�����}�b�v
Texture2D<float4> g_refractionMap : register(t5);                   // ���܃}�b�v
StructuredBuffer<SVertex> g_vertexBuffers : register(t6);           // ���_�o�b�t�@�[�B
StructuredBuffer<int> g_indexBuffers : register(t7);                // �C���f�b�N�X�o�b�t�@�[�B
TextureCube<float4> g_skyCubeMap : register(t8);                    // �X�J�C�L���[�u�}�b�v�B

RWTexture2D<float4> gOutput : register(u0);

SamplerState  s : register(s0);

struct RayPayload
{
    float3 color;               // �J���[
    int hit;
    int depth;
};

// UV���W���擾
float2 GetUV(BuiltInTriangleIntersectionAttributes attribs)
{
    float3 barycentrics = float3(1.0 - attribs.barycentrics.x - attribs.barycentrics.y, attribs.barycentrics.x, attribs.barycentrics.y);

    // �v���~�e�B�uID���擾
    uint primID = PrimitiveIndex();

    // �v���~�e�B�uID���璸�_�ԍ����擾����
    uint v0_id = g_indexBuffers[primID * 3];
    uint v1_id = g_indexBuffers[primID * 3 + 1];
    uint v2_id = g_indexBuffers[primID * 3 + 2];
    float2 uv0 = g_vertexBuffers[v0_id].uv;
    float2 uv1 = g_vertexBuffers[v1_id].uv;
    float2 uv2 = g_vertexBuffers[v2_id].uv;

    float2 uv = barycentrics.x * uv0 + barycentrics.y * uv1 + barycentrics.z * uv2;
    return uv;
}

// �@�����擾
float3 GetNormal(BuiltInTriangleIntersectionAttributes attribs, float2 uv)
{
    float3 barycentrics = float3(1.0 - attribs.barycentrics.x - attribs.barycentrics.y, attribs.barycentrics.x, attribs.barycentrics.y);

    // �v���~�e�B�uID���擾
    uint primID = PrimitiveIndex();

    // �v���~�e�B�uID���璸�_�ԍ����擾����
    uint v0_id = g_indexBuffers[primID * 3];
    uint v1_id = g_indexBuffers[primID * 3 + 1];
    uint v2_id = g_indexBuffers[primID * 3 + 2];

    float3 normal0 = g_vertexBuffers[v0_id].normal;
    float3 normal1 = g_vertexBuffers[v1_id].normal;
    float3 normal2 = g_vertexBuffers[v2_id].normal;
    float3 normal = barycentrics.x * normal0 + barycentrics.y * normal1 + barycentrics.z * normal2;
    normal = normalize(normal);

    float3 tangent0 = g_vertexBuffers[v0_id].tangent;
    float3 tangent1 = g_vertexBuffers[v1_id].tangent;
    float3 tangent2 = g_vertexBuffers[v2_id].tangent;
    float3 tangent = barycentrics.x * tangent0 + barycentrics.y * tangent1 + barycentrics.z * tangent2;
    tangent = normalize(tangent);
    
    float3 binormal0 = g_vertexBuffers[v0_id].binormal;
    float3 binormal1 = g_vertexBuffers[v1_id].binormal;
    float3 binormal2 = g_vertexBuffers[v2_id].binormal;
    float3 binormal = barycentrics.x * binormal0 + barycentrics.y * binormal1 + barycentrics.z * binormal2;
    

    float3 binSpaceNormal = g_normalMap.SampleLevel (s, uv, 0.0f).xyz;    
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;

    float3 newNormal = tangent * binSpaceNormal.x + binormal * binSpaceNormal.y + normal * binSpaceNormal.z ;

    return newNormal;
}

// �����Ɍ����ă��C���΂�
void TraceLightRay(inout RayPayload raypayload, float3 normal)
{
    float hitT = RayTCurrent();
    float3 rayDirW = WorldRayDirection();
    float3 rayOriginW = WorldRayOrigin();

    // Find the world-space hit position
    float3 posW = rayOriginW + hitT * rayDirW;

    RayDesc ray;
    ray.Origin = posW;
    ray.Direction = normalize(float3(0.5, 0.5, 0.2));
    ray.TMin = 0.01f;
    ray.TMax = 100;

    TraceRay(
        g_raytracingWorld,
        0,
        0xFF,
        1,
        0,
        1,
        ray,
        raypayload
    );
}

// ���˃��C���΂�
void TraceReflectionRay(inout RayPayload raypayload, float3 normal)
{
    if(raypayload.depth < 3)
    {
        float hitT = RayTCurrent();
        float3 rayDirW = WorldRayDirection();
        float3 rayOriginW = WorldRayOrigin();

        // ���˃x�N�g�������߂�
        float3 refDir = reflect(rayDirW, normal);

        // Find the world-space hit position
        float3 posW = rayOriginW + hitT * rayDirW;

        RayDesc ray;
        ray.Origin = posW;
        ray.Direction = refDir;
        ray.TMin = 0.01f;
        ray.TMax = 1000000;

        TraceRay(
            g_raytracingWorld,
            0,
            0xFF,
            0,
            0,
            0,
            ray,
            raypayload
        );
    }
}

[shader("raygeneration")]
void rayGen()
{
    uint3 launchIndex = DispatchRaysIndex();
    uint3 launchDim = DispatchRaysDimensions();

    float2 crd = float2(launchIndex.xy);
    float2 dims = float2(launchDim.xy);
    
    float4 worldPos;
    worldPos.xy = float2(crd/dims) * float2(2.0, -2.0) - float2(1.0, -1.0);
    worldPos.zw = float2( 1.0f, 1.0f );
    worldPos = mul( g_camera.mViewProjInv, worldPos );
    worldPos.xyz /= worldPos.w;

    // �s�N�Z�������ɑł��o�����C���쐬����
    RayDesc ray;
    ray.Origin = g_camera.pos;
    ray.Direction = normalize( worldPos.xyz - ray.Origin);


    ray.TMin = 0;
    ray.TMax = 1000000;

    RayPayload payload;
    payload.depth = 0;

    //TraceRay
    TraceRay(g_raytracingWorld, 0 /*rayFlags*/, 0xFF, 0 /* ray index*/, 0, 0, ray, payload);

    float3 col = payload.color;

    gOutput[launchIndex.xy] = float4(col, 1);
}

[shader("miss")]
void miss(inout RayPayload payload)
{
    float3 rayDirW = WorldRayDirection();
    payload.color = g_skyCubeMap.SampleLevel(s, rayDirW, 0.0f);
}

[shader("closesthit")]
void chs(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attribs)
{
    payload.depth++;
    // �q�b�g�����v���~�e�B�u��UV���W���擾
    float2 uv = GetUV(attribs);

    // �q�b�g�����v���~�e�B�u�̖@�����擾
    float3 normal = GetNormal(attribs, uv);
    
    // �����ɂނ����ă��C���΂�
    TraceLightRay(payload, normal);
    float lig = 0.0f;
    if(payload.hit == 0)
    {
        float3 ligDir =  normalize(float3(0.5, 0.5, 0.2));
        float t = max(0.0f, dot(ligDir, normal));
        lig = t;
    }

    //����
    lig += 0.5f;
    RayPayload refPayload;
    refPayload.depth = payload.depth;
    refPayload.color = 0;

    // ���˃��C
    TraceReflectionRay(refPayload, normal);

    // ���̃v���~�e�B�u�̔��˗����擾
    float reflectRate = g_specularMap.SampleLevel(s, uv, 0.0f).a;
    float3 color = gAlbedoTexture.SampleLevel(s, uv, 0.0f).rgb;
    color *= lig;
    if( payload.depth == 1){
        payload.color = refPayload.color;
    }else{
        payload.color = lerp(color, refPayload.color, reflectRate);
    }
    
    payload.depth--;
}

[shader("closesthit")]
void shadowChs(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes attribs)
{
    payload.hit = 1;
}

[shader("miss")]
void shadowMiss(inout RayPayload payload)
{
   payload.hit = 0;
}
