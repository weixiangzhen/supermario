#ifndef __LAYERSTART_H__
#define __LAYERSTART_H__

#include "LayerBase.h"

class LayerStartLoading : public CCLayer
{
public:
	CREATE_FUNC(LayerStartLoading);
	bool init();
	void onEnter();
	void onExit();
private:	
	void preloadMusic();
	void preloadImage();
	void progress(CCObject*);	

private:
	unsigned char m_Percent;
};


class LayerStart : public LayerBase1
{
public:
	CREATE_FUNC(LayerStart);
	virtual bool init();
	//static CCScene* scene();

	void onEnter();
	void onExit();
private:		
	void initStart();

	void newGame(CCObject*);
	void resumeGame(CCObject*);
	void settingsCallBack(CCObject*);
	void helpCallBack(CCObject*);
	void aboutCallBack(CCObject*);
private:

};

/* 玩家人物选择 */
class LayerPlayer : public LayerBase1
{
public:
	CREATE_FUNC(LayerPlayer);
	virtual bool init();
	
	void startGame(CCObject*);

	virtual bool ccTouchBegan(CCTouch*, CCEvent*);
	virtual void ccTouchEnded(CCTouch*, CCEvent*);

	//bool isClick(CCTouch* pTouch);
private:
	CCTexture2D *m_PanelTexture;

	CCSprite *m_Player1;
	CCSprite *m_Player2;

	CCSprite *m_PlayerBg;

	int m_PlayerID;
};

/* 帮助 */
class LayerHelp : public CCLayer
{
public:
	CREATE_FUNC(LayerHelp);
	virtual bool init()
	{
		if (!CCLayer::init()){ return false; }

		CCSprite *bg = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->addImage("help.png"));
		bg->setAnchorPoint(ccp(0, 0));
		bg->setPosition(ccp(0, -32));
		this->addChild(bg);

		this->setTouchEnabled(true);
		this->setTouchMode(kCCTouchesOneByOne);

		CCDictionary *dict = CCDictionary::createWithContentsOfFile(zh_CN_XML);
		this->addChild(Common::fontLabel(dict, zh_HELPPAUSE, ccp(50, 325), 0.8f));
		this->addChild(Common::fontLabel(dict, zh_HELPLEFT, ccp(65, 220), 0.8f));
		this->addChild(Common::fontLabel(dict, zh_HELPRIGHT, ccp(210, 220), 0.8f));
		this->addChild(Common::fontLabel(dict, zh_HELPGROW, ccp(400, 325), 0.8f));
		this->addChild(Common::fontLabel(dict, zh_HELPSHOOT, ccp(605, 220), 0.8f));
		this->addChild(Common::fontLabel(dict, zh_HELPJUMP, ccp(720, 275), 0.8f));
		this->addChild(Common::fontLabel(dict, zh_HELPSHOP, ccp(765, 325), 0.8f));
		
		return true;
	}

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
	{		
		CCDirector::sharedDirector()->replaceScene(Common::scene(LayerStart::create()));
		return true;
	}
};

/* 关于 */
class LayerAbout : public CCLayer
{
public:
	CREATE_FUNC(LayerAbout);
	virtual bool init()
	{
		if (!CCLayer::init()){ return false; }

		CCSprite *bg = CCSprite::createWithTexture(CCTextureCache::sharedTextureCache()->addImage("about.png"));
		bg->setAnchorPoint(ccp(0, 0));
		//bg->setScale(1.7f);
		bg->setPosition(ccp(0, -40));
		this->addChild(bg);

		this->setTouchEnabled(true);
		this->setTouchMode(kCCTouchesOneByOne);

		CCDictionary *dict = CCDictionary::createWithContentsOfFile(zh_CN_XML);
		this->addChild(Common::fontLabel(dict, zh_ABOUTAUTHOR, ccp(180, 400), 1.0f));
		this->addChild(Common::fontLabel(dict, zh_ABOUTQQ, ccp(250, 350), 1.0f));
		this->addChild(Common::fontLabel(dict, zh_ABOUTEMAIL, ccp(330, 300), 1.0f));
		return true;
	}

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
	{
		CCDirector::sharedDirector()->replaceScene(Common::scene(LayerStart::create()));
		return true;
	}
};

#endif