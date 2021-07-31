#include "k2EnginePreCompile.h"
#include "LightCulling.h"

void LightCulling::Init(Texture& depthTexture, ConstantBuffer& lightCB)
{
	m_shader.LoadCS("Assets/shader/lightCulling.fx", "CSMain");

    // ���C�g�J�����O�̃J�����p�̒萔�o�b�t�@�[���쐬
    CameraData cameraData;
    cameraData.mProj = g_camera3D->GetProjectionMatrix();
    cameraData.mProjInv.Inverse(g_camera3D->GetProjectionMatrix());
    cameraData.mCameraRot = g_camera3D->GetCameraRotation();
    cameraData.screenParam.x = g_camera3D->GetNear();
    cameraData.screenParam.y = g_camera3D->GetFar();
    cameraData.screenParam.z = FRAME_BUFFER_W;
    cameraData.screenParam.w = FRAME_BUFFER_H;
    
    m_cameraDataCB.Init(sizeof(cameraData), &cameraData);

    // �f�B�X�N���v�^�q�[�v���������B
    m_descriptroHeap.RegistShaderResource(0, depthTexture);
    m_descriptroHeap.RegistUnorderAccessResource(0, m_pointLightNoListInTileUAV);
    m_descriptroHeap.RegistConstantBuffer(0, m_cameraDataCB);
    m_descriptroHeap.RegistConstantBuffer(1, lightCB);
    m_descriptroHeap.Commit();
}