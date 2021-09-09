#pragma once

#include "cocos2d.h"
#include "../../src/LoadBalancingListener.h"
#include "../../src/BaseView.h"
#include "PlayerView.h"
#include "ListController.h"


class DemoScene : public cocos2d::Layer, public BaseView
{
public:
	DemoScene(void);
	~DemoScene(void);
	static DemoScene* create(void);
	
	void setLBC(ExitGames::LoadBalancing::Client* lbc, LoadBalancingListener* lbl);
	void setupUI(void);
	void tick(float delta);

	virtual void updateState(int state, const ExitGames::Common::JString& stateStr, const ExitGames::Common::JString& joinedRoomName);
	virtual void initPlayers();
	virtual void addPlayer(int playerNr, const ExitGames::Common::JString& playerName, bool local);
	virtual void removePlayer(int playerNr);
	virtual void changePlayerColor(int playerNr, int color);
	virtual void changePlayerPos(int playerNr, int x, int y);
    virtual void setupScene(int gridSize);
	virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames);

	//from cocos2d::Layer
	virtual bool init(void);
	virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);
	virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event* event);

private: 
	int mCanvasSize;
	int mCellSize;

	cocos2d::DrawNode* mpCanvas;
	cocos2d::DrawNode* mpChecker;
	cocos2d::DrawNode* mpGrid;
	cocos2d::Node* mpPlayersNode;

	ExitGames::LoadBalancing::Client* mpLbc;
	LoadBalancingListener* mpLbl;

	cocos2d::Label* mpLabelState;
	ListController* mpRoomList;

	void drawChecker(int gridSize);
	void drawGrid(int gridSize);
	void updatePlayerCellSizes(int gridSize);

	void menuNewGameCallback(Ref* pSender);
	void menuLeaveCallback(Ref* pSender);
	void menuGridSizeCallback(Ref* pSender);
	void menuRandomColorCallback(Ref* pSender);
	void menuAutomoveCallback(bool automove);
	void menuUseGroupsCallback(bool useGroups);

	void roomListCallback(const ExitGames::Common::JString& name);
	
	void onTouch(cocos2d::Touch* touch, cocos2d::Event* event);

	float setupDesktop(bool movable);
};