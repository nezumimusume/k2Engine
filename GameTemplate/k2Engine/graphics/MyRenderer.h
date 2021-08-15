#pragma once

namespace nsK2Engine {
	// �V�[���̃f�B���N�V�������C�g�̍ő吔
	const int MAX_DIRECTIONAL_LIGHT = 4;
	// �V�[���̃|�C���g���C�g�̍ő吔�B
	const int MAX_POINT_LIGHT = 1000;


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
}
