#include "stdafx.h"
#include "PyramidRender.h"

bool PyramidRender::Start()
{
	//�f�B�t�@�[�h�����_�����O�B
	m_modelRender.Init(
		"Assets/modelData/pyramid.tkm",
		nullptr,
		0,
		enModelUpAxisZ,
		false,
		m_maxPyramid
	);
	return true;
}
void PyramidRender::Update()
{
}
void PyramidRender::Render(RenderContext& rc)
{
	m_modelRender.Draw(rc);
}