#pragma once

namespace nsK2Engine {
	// �V�[���̃f�B���N�V�������C�g�̍ő吔
	const int MAX_DIRECTIONAL_LIGHT = 4;
	// �V�[���̃|�C���g���C�g�̍ő吔�B
	const int MAX_POINT_LIGHT = 256;
	// �V�[���̃X�|�b�g���C�g�̍ő吔�B
	const int MAX_SPOT_LIGHT = 256;

	// TBR�p�̃p�����[�^�B
	const int TILE_WIDTH = 16;  // �^�C���̕�
	const int TILE_HEIGHT = 16; // �^�C���̍���
	const int NUM_TILE = (FRAME_BUFFER_W / TILE_WIDTH) * (FRAME_BUFFER_H / TILE_HEIGHT); // �^�C���̐�

	
	// �V���h�E�}�b�v
	enum {
		SHADOW_MAP_AREA_NEAR,		//�߉e�͈͂̃V���h�E�}�b�v�B
		SHADOW_MAP_AREA_MIDDLE,		//���e�͈͂̃V���h�E�}�b�v�B
		SHADOW_MAP_AREA_FAR,		//���e�͈͂̃V���h�E�}�b�v�B
		NUM_SHADOW_MAP
	};

	// �\�t�g�V���h�E�ŗ��p����J���[�o�b�t�@�̃t�H�[�}�b�g�B
	// �����_�����O�^�[�Q�b�g�̃t�H�[�}�b�g�B
	struct RenderTargetFormat {
		DXGI_FORMAT colorBufferFormat;	// �J���[�o�b�t�@�̃t�H�[�}�b�g�B
		DXGI_FORMAT depthBufferFormat;	// �[�x�o�b�t�@�̃t�H�[�}�b�g�B
	};
	// �\�t�g�V���h�E�p�̃V���h�E�}�b�v�̃t�H�[�}�b�g�B
	const RenderTargetFormat g_softShadowMapFormat = {
		DXGI_FORMAT_R32G32_FLOAT,	// �J���[�o�b�t�@
		DXGI_FORMAT_D32_FLOAT		// �[�x�o�b�t�@
	};
	// �n�[�h�V���h�E�p�̃V���h�E�}�b�v�̃t�H�[�}�b�g�B
	const RenderTargetFormat g_hardShadowMapFormat = {
		DXGI_FORMAT_R32_FLOAT,		// �J���[�o�b�t�@
		DXGI_FORMAT_D32_FLOAT		// �[�x�o�b�t�@�B
	};
	// ���C�������_�����O�^�[�Q�b�g�̃t�H�[�}�b�g�B
	const RenderTargetFormat g_mainRenderTargetFormat = {
		DXGI_FORMAT_R16G16B16A16_FLOAT,
		DXGI_FORMAT_UNKNOWN
	};
	// �{�����[�����C�g�}�b�v�̃t�H�[�}�b�g�B
	const RenderTargetFormat g_drawVolumeLightMapFormat = {
		DXGI_FORMAT_R32_FLOAT,
		DXGI_FORMAT_D32_FLOAT
	};
	
}