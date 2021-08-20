#include "stdafx.h"
#include "GameCamera.h"

#include "Player.h"

GameCamera::GameCamera()
{

}

GameCamera::~GameCamera()
{

}

bool GameCamera::Start()
{
	g_camera3D->SetPosition({ 0.0f, 100.0f, 400.0f });
	g_camera3D->SetFar(40000.0f);
	return true;
}


void GameCamera::Update()
{
	//�J�������X�V�B
	//�����_���v�Z����B

	//�v���C���̑������炿����Ə�𒍎��_�Ƃ���B
	Vector3 cameraPos = g_camera3D->GetPosition();
	Vector3 cameraPosOld = cameraPos;
	//�p�b�h�̓��͂��g���ăJ�������񂷁B
	float x = g_pad[0]->GetRStickXF();
	float y = g_pad[0]->GetRStickYF();
	//Y������̉�]
	Quaternion qRot;
	qRot.SetRotationDeg(Vector3::AxisY, 0.5f * x);
	qRot.Apply(cameraPos);
	//X������̉�]�B
	Vector3 axisX;
	axisX.Cross(Vector3::AxisY, cameraPos);
	axisX.Normalize();
	qRot.SetRotationDeg(axisX, 0.5f * y);
	qRot.Apply(cameraPos);
	//�J�����̉�]�̏�����`�F�b�N����B
	//�����_���王�_�܂ł̃x�N�g���𐳋K������B
	//���K������ƁA�x�N�g���̑傫�����P�ɂȂ�B
	//�傫�����P�ɂȂ�Ƃ������Ƃ́A�x�N�g�����狭�����Ȃ��Ȃ�A�����݂̂̏��ƂȂ�Ƃ������ƁB
	Vector3 toPosDir = cameraPos;
	toPosDir.Normalize();
	if (toPosDir.y < -0.5f) {
		//�J����������������B
		cameraPos = cameraPosOld;
	}
	else if (toPosDir.y > 0.8f) {
		//�J�����������������B
		cameraPos = cameraPosOld;
	}

	g_camera3D->SetPosition(cameraPos);
}