#pragma once
#include "tkFile/TknFile.h"
#include "AI/PathFinding/NaviMesh.h"
#include "AI/PathFinding/Path.h"
#include "AI/PathFinding/PathFinding.h"

class Game : public IGameObject
{
public:
	//////////////////////////////////////
	// ÉÅÉìÉoä÷êîÅB
	//////////////////////////////////////
	Game();
	~Game();
	bool Start();
	void Update();
	void Render(RenderContext& rc);
private:
	CharacterController m_charaCon;
	ModelRender m_bgModelRendedr;
	ModelRender m_charaRender;
	ModelRender m_targetPointRender;
	TknFile m_tknFile;
	PhysicsStaticObject m_bgObject;
	nsAI::NaviMesh m_nvmMesh;
	nsAI::Path m_path;
	nsAI::PathFinding m_pathFiding;
	Vector3 m_position;
	Vector3 m_targetPointPosition;
	PointLight m_targetPointPointLight;
};

