#include "k2EnginePreCompile.h"
#include "LightCulling.h"

namespace nsK2Engine {
    void LightCulling::Init(
        Texture& depthTexture, 
        ConstantBuffer& lightCB, 
        RWStructuredBuffer& pointLightNoListInTileUAV,
        RWStructuredBuffer& spotLightNoListInTileUAV
    ){
        m_pointLightNoListInTileUAV = &pointLightNoListInTileUAV;
        m_spotLightNoListInTileUAV = &spotLightNoListInTileUAV;

        m_shader.LoadCS("Assets/shader/preProcess/lightCulling.fx", "CSMain");

        // ���C�g�J�����O�̃J�����p�̒萔�o�b�t�@�[���쐬
        m_cameraDataCB.Init(sizeof(CameraData), nullptr);

        // �f�B�X�N���v�^�q�[�v���������B
        m_descriptorHeap.RegistShaderResource(0, depthTexture);
        m_descriptorHeap.RegistUnorderAccessResource(0, pointLightNoListInTileUAV);
        m_descriptorHeap.RegistUnorderAccessResource(1, spotLightNoListInTileUAV);
        m_descriptorHeap.RegistConstantBuffer(0, m_cameraDataCB);
        m_descriptorHeap.RegistConstantBuffer(1, lightCB);
        m_descriptorHeap.Commit();

        // ���[�g�V�O�l�`�����쐬�B
        m_rootSignature.Init(
            D3D12_FILTER_MIN_MAG_MIP_LINEAR,
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
        BeginGPUEvent("LightCulling");

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
        rc.SetComputeDescriptorHeap(m_descriptorHeap);
        rc.SetPipelineState(m_pipelineState);

        // �t���[���o�b�t�@�̕��ƍ�����TILE_WIDTH�ATILE_HEIGHT�̔{���ɐ؂�グ��B
        UINT FRAME_BUFFER_W_ROUNDUP = ((FRAME_BUFFER_W + TILE_WIDTH -1) / TILE_WIDTH) * TILE_WIDTH;
        UINT FRAME_BUFFER_H_ROUNDUP = ((FRAME_BUFFER_H + TILE_HEIGHT -1) / TILE_HEIGHT) * TILE_HEIGHT;
        // �O���[�v�̐��̓^�C���̐�
        rc.Dispatch(
            FRAME_BUFFER_W_ROUNDUP / TILE_WIDTH,
            FRAME_BUFFER_H_ROUNDUP / TILE_HEIGHT,
            1);

        // ���\�[�X�o���A
        rc.TransitionResourceState(
            m_pointLightNoListInTileUAV->GetD3DResoruce(),
            D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );

        EndGPUEvent();
    }
}
