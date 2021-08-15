#include "k2EnginePreCompile.h"
#include "LightCulling.h"

namespace nsK2Engine {
    void LightCulling::Init(Texture& depthTexture, ConstantBuffer& lightCB, RWStructuredBuffer& pointLightNoListInTileUAV)
    {
        m_pointLightNoListInTileUAV = &pointLightNoListInTileUAV;
        m_shader.LoadCS("Assets/shader/lightCulling.fx", "CSMain");

        // ���C�g�J�����O�̃J�����p�̒萔�o�b�t�@�[���쐬
        m_cameraDataCB.Init(sizeof(CameraData), nullptr);

        // �f�B�X�N���v�^�q�[�v���������B
        m_descriptroHeap.RegistShaderResource(0, depthTexture);
        m_descriptroHeap.RegistUnorderAccessResource(0, pointLightNoListInTileUAV);
        m_descriptroHeap.RegistConstantBuffer(0, m_cameraDataCB);
        m_descriptroHeap.RegistConstantBuffer(1, lightCB);
        m_descriptroHeap.Commit();

        // ���[�g�V�O�l�`�����쐬�B
        m_rootSignature.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP,
            D3D12_TEXTURE_ADDRESS_MODE_WRAP);

        // �p�C�v���C���X�e�[�g���������B
        D3D12_COMPUTE_PIPELINE_STATE_DESC  psoDesc = { 0 };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.CS = CD3DX12_SHADER_BYTECODE(m_shader.GetCompiledBlob());
        psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
        psoDesc.NodeMask = 0;
        m_pipelineState.Init(psoDesc);

    }
    void LightCulling::Execute(RenderContext& rc)
    {
        CameraData cameraData;
        cameraData.mProj = g_camera3D->GetProjectionMatrix();
        cameraData.mProjInv.Inverse(g_camera3D->GetProjectionMatrix());
        cameraData.mCameraRot = g_camera3D->GetCameraRotation();
        cameraData.screenParam.x = g_camera3D->GetNear();
        cameraData.screenParam.y = g_camera3D->GetFar();
        cameraData.screenParam.z = FRAME_BUFFER_W;
        cameraData.screenParam.w = FRAME_BUFFER_H;
        m_cameraDataCB.CopyToVRAM(cameraData);

        //���C�g�J�����O�̃R���s���[�g�V�F�[�_�[���f�B�X�p�b�`
        rc.SetComputeRootSignature(m_rootSignature);
        rc.SetComputeDescriptorHeap(m_descriptroHeap);
        rc.SetPipelineState(m_pipelineState);

        // �O���[�v�̐��̓^�C���̐�
        rc.Dispatch(
            FRAME_BUFFER_W / TILE_WIDTH,
            FRAME_BUFFER_H / TILE_HEIGHT,
            1);

        // ���\�[�X�o���A
        rc.TransitionResourceState(
            m_pointLightNoListInTileUAV->GetD3DResoruce(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );


    }
}
