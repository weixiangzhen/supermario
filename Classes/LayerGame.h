#ifndef __LAYERGAME_H__
#define __LAYERGAME_H__

#include "LayerBase.h"
#include "Mario.h"

/* 加载资源 */
class LayerGameLoading : public LayerBase1
{
public:
	//CREATE_FUNC(LayerGameLoading);
	static LayerGameLoading* create(int state);
	static LayerGameLoading* create(int state, int playerID);
	virtual bool init(int state);
	virtual bool init(int state, int playerID);

	void onEnter();
	void onExit();

	void loadingFinish(float);

	virtual void addPopup();
	virtual void subPopup();
private:
	int m_State;
	int m_PlayerID;
};

/* 按钮 */
class Button : public CCSprite
{
public:
	static Button* create(const char *normal, const char *pressed)
	{
		Button *pRet = new Button;
		if (pRet&&pRet->init(normal, pressed))
		{
			pRet->autorelease();
		}
		else
		{
			delete pRet;
			pRet = NULL;
		}
		return pRet;
	}
	bool init(const char *normal, const char *pressed)
	{
		if (!CCSprite::init()){ return false; }

		sprintf(m_Normal, "%s", normal);
		sprintf(m_Pressed, "%s", pressed);

		m_ID = -1;
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(normal));
		m_IsPressed = false;

		return true;
	}

	void setChecked(bool b)
	{
		m_IsPressed = b;
		if (b)
		{
			this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(m_Pressed));
		}
		else
		{
			this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(m_Normal));
		}
	}
	
	bool isPressed(){ return m_IsPressed; }

	void setID(int id){ this->m_ID = id; }
	int getID(){ return m_ID; }
private:
	bool m_IsPressed;

	int m_ID;

	char m_Normal[32];
	char m_Pressed[32];
};

/* 游戏逻辑 */
class LayerGame : public LayerBase2
{
public:
	static CCPoint m_StartPoint;

	//CREATE_FUNC(LayerGame);
	static LayerGame* create(int, int);
	virtual bool init(int, int);

	void onEnter();
	void onExit();

	virtual void registerWithTouchDispatcher();
	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent);

	void preloadRes();

	void setLife(int life){ m_life = life; m_LifeLabel->setString(Common::format(m_life)); }
	void setGrowCount(int growCount){ m_GrowCount = growCount; m_GrowLabel->setString(Common::format(m_GrowCount)); }
	void setCoin(int coin){ m_CurrentCoin = coin; m_CoinLabel->setString(Common::format(m_CurrentCoin)); }

	void update(float dt);
	void nextPass();
	void removeNode(CCNode *node);

	CCTMXTiledMap* getMap(){ return m_Map; }

	void resetData();
	void resetData(int playerID);
	void recoverData();
	void saveData(bool);
private:
	void pauseGame(CCNode*);
	void resumeGame(CCNode*);
public:
	void pauseGame();
	void resumeGame();

	virtual void addPopup();
	virtual void subPopup();

	virtual void shopCallBack(CCObject *obj){ saveData(false); LayerBase2::shopCallBack(obj); }
	//virtual void settingsCallBack(CCObject*);
	virtual void growCallBack(CCObject*);
	void timerout(float);

	//virtual void overDialog(bool b);	
private:
	void addMap();
	void addBiology();
	void addButton();
	void addInfo();

	void addAchievementEffect(int);
	void removeEffect(float);
private:	
	CCTMXTiledMap *m_Map;
	Figure *m_Player;
	int m_PlayerID;
	
	Button *m_LeftButton;
	Button *m_RightButton;
	Button *m_AButton;
	Button *m_BButton;

	//终点位置标志
	CCPoint m_OverPoint;
	bool m_IsBossDead;

	unsigned char m_NowPass;
	unsigned char m_life;
	unsigned int m_GrowCount;
	unsigned char m_GrowState;
	unsigned int m_CurrentCoin;	
	unsigned int m_CurrentScore;
	unsigned int m_TempScore;
	int m_TimeCount;	

	unsigned int m_CoinCount;
	unsigned int m_MonsterCount;

	CCLabelBMFont *m_LifeLabel;
	CCLabelBMFont *m_GrowLabel;
	CCLabelBMFont *m_CoinLabel;
	CCLabelBMFont *m_ScoreLabel;
	CCLabelBMFont *m_TimeLabel;	
};

/* 游戏通关界面 */
class LayerWin : public LayerBase1
{
public:
	CREATE_FUNC(LayerWin);	
	virtual bool init()
	{
		if (!LayerBase1::init()){ return false; }		

		return true;
	}

	virtual void onEnter()
	{
		LayerBase1::onEnter();
		addChild(Common::fontLabel("You Win!", ccp(winSize.width / 2, winSize.height / 2 + 50), 1.0f));

		CCTexture2D *m_PanelTexture = CCTextureCache::sharedTextureCache()->addImage("panel.png");
		CCSprite *backGameBg = CCSprite::createWithTexture(m_PanelTexture, CCRectMake(240, 124, 272, 68));
		backGameBg->setPosition(ccp(winSize.width / 2, 60));
		backGameBg->setScale(0.8f);
		addChild(backGameBg);

		CCDictionary *dict = CCDictionary::createWithContentsOfFile(zh_CN_XML);
		CCMenuItem *backGameitem = Common::itemLabel(dict, zh_BACKGAME, ccp(winSize.width / 2, 60));
		backGameitem->setTarget(this, menu_selector(LayerWin::backGame));
		CCMenu *menu = CCMenu::create(backGameitem, NULL);
		menu->setPosition(CCPointZero);
		addChild(menu);
	}

	virtual void onExit()
	{
		LayerBase1::onExit();
	}	

	void backGame(CCObject*);
};

#endif


