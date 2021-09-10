#pragma once

#include "cocos2d.h"
#include "../../src/LoadBalancingListener.h"
#include "../../src/BaseView.h"
#include "ListController.h"

class View : public cocos2d::Layer, public BaseView, public cocos2d::TextFieldDelegate
{
public:
	View(void);
	~View(void);

	static View* create(void);
	
	// BaseView
	virtual void onStateChange(int state, const ExitGames::Common::JString& stateStr, bool inLobby, bool inRoom);
	virtual void refreshStateInfo(const ExitGames::Common::JString& stateInfo);
	virtual void refreshMessage(const ExitGames::Common::JString& msg);

    virtual void setup(void);
    virtual void setupBoard(int tileCols, int tileRows);
	virtual void updateBoard(unsigned char* tiles, unsigned char flippedTiles[2]);
	virtual void updateRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& roomNames);
	virtual void updateSavedRoomList(const ExitGames::Common::JVector<ExitGames::Common::JString>& savedRoomNames);

	//cocos2d::Layer
	virtual bool init(void);
	virtual void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
	virtual void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
	virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);

	virtual void keyBackClicked(void);

	// cocos2d::TextFieldDelegate
	virtual bool onTextFieldInsertText(cocos2d::TextFieldTTF* pSender, const ExitGames::Common::JString& text, size_t nLen);


	// self
	void init(LoadBalancingListener* lbl);
private: 
	void setupUI(void);

	LoadBalancingListener* mpLbl;
	float mBoardSize;
	float mTileSize;
	float mTileMargin;
	int mTileRows;
	int mTileCols;

	void switchScene(cocos2d::Node*);
	int sceneOrder(cocos2d::Node*);
	const char* getInputUserName(void);

	// scenes as nodes let keep unmovable info overlay
	cocos2d::Node* mpSceneConnect;
	cocos2d::Node* mpSceneLobby;
	cocos2d::Node* mpSceneGame;

	cocos2d::Node* mpCurrentScene;

	// all scenes
	cocos2d::Label* mpLabelState;
	cocos2d::Label* mpLabelLog;
	void menuLeaveCallback(Ref* pSender);
	ExitGames::Common::JString mStateString;

	// mpSceneConnect
	cocos2d::TextFieldTTF* mpInputUserName;
	void menuInputUserNameTouchCallback(Ref* pSender);
	void menuConnectCallback(Ref* pSender);

	// mpSceneLobby
	cocos2d::Label* mpLobbyStats1;
	cocos2d::Label* mpLobbyStats2;
	ListController* mpRoomList;
	ListController* mpSavedRoomList;
	void roomListCallback(const ExitGames::Common::JString& name);
	void savedRoomListCallback(const ExitGames::Common::JString& name);

	void menuNewGameCallback(Ref* pSender);
	void menuRandomGameCallback(Ref* pSender);
	void menuRefreshCallback(Ref* pSender);	
	
	// mpSceneGame
	cocos2d::Label* mpLabelMessage;
	cocos2d::DrawNode* mpBoard;
	cocos2d::Node* mpTiles;

	void menuSuspendCallback(Ref* pSender);
	void menuAbandonCallback(Ref* pSender);
};