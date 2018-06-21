#include "LayerStart.h"
#include "LayerGame.h"
#include "LayerPopup.h"

static const char* imageNames[] = {"menu.png",
								"panel.png",
								"shopMain.png",
								"help.png",
								"home.png",
								"about.png",
								"switchbg.png",
								"playermario.png",
								"playerstallone.png",
								"control.png",
								"dialog.png",
								"effect.png",
								"mario.png",
								"stallone.png",
								"tiledmap/object.png",								
								"tiledmap/scene.png",
								//"skill2.png",
								//"skill3.png",
								"skill4.png"
								//"skill5.png",
								//"tile.png"};
								};
//音乐和音效初始化
bool Common::isPlayEffect = true;
bool Common::isPlayMusic = true;

/************************************************************************/
/* 资源图片加载界面                                                       */
/************************************************************************/
bool LayerStartLoading::init()
{
	if (!CCLayer::init()){ return false; }

	m_Percent = 0;
	return true;
}

void LayerStartLoading::onEnter()
{
	CCLayer::onEnter();

	//正在加载动画
	Common::loadingAnimation(this);

	preloadImage();
}

void LayerStartLoading::onExit()
{
	CCLayer::onExit();
}

//预加载音乐
void LayerStartLoading::preloadMusic()
{		
	for (int i = 0; i < sizeof(musicNames) / sizeof(*musicNames); i++)
	{
		SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(musicNames[i]);
	}
	for (int i = 0; i < sizeof(effectNames) / sizeof(*effectNames); i++)
	{
		SimpleAudioEngine::sharedEngine()->preloadEffect(effectNames[i]);
	}
}

//预加载图片
void LayerStartLoading::preloadImage()
{
	for (int i = 0; i < sizeof(imageNames) / sizeof(*imageNames); i++)
	{
		CCTextureCache::sharedTextureCache()->addImageAsync(imageNames[i], this, callfuncO_selector(LayerStartLoading::progress));
	}
}

void LayerStartLoading::progress(CCObject *sender)
{	
	m_Percent++;
	if (m_Percent == sizeof(imageNames) / sizeof(*imageNames))
	{		
		//预加载音乐
		preloadMusic();
		//加载音乐数据			
		Common::isPlayEffect = CCUserDefault::sharedUserDefault()->getBoolForKey("isPlayEffect", true);
		Common::isPlayMusic = CCUserDefault::sharedUserDefault()->getBoolForKey("isPlayMusic", true);
		//图片加载完成，替换场景
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.0f, Common::scene(LayerStart::create())));
	}
}

/************************************************************************/
/* 游戏开始界面                                                           */
/************************************************************************/
bool LayerStart::init()
{
	if (!LayerBase1::init()){ return false;}	

	return true;
}

void LayerStart::onEnter()
{
	LayerBase1::onEnter();	
	
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic(musicNames[1], true);
	if (!Common::isPlayMusic)
	{
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	}		
	initStart();
}

void LayerStart::onExit()
{
	LayerBase1::onExit();
}

//游戏界面
void LayerStart::initStart()
{
	//添加背景图
	CCTexture2D *bgTexture = CCTextureCache::sharedTextureCache()->addImage("home.png");	
	CCSprite *bg = CCSprite::createWithTexture(bgTexture);
	bg->setAnchorPoint(ccp(0, 0));
	bg->setPositionY(-32);
	addChild(bg);

	float fx = 140;
	float fy = 395;
	float dis = 75;
	CCDictionary *dict = CCDictionary::createWithContentsOfFile(zh_CN_XML);
	CCMenuItem *item1 = Common::itemLabel(dict, zh_STARTGAME, ccp(fx, fy));
	item1->setTarget(this, menu_selector(LayerStart::newGame));
	CCMenuItem *item2 = Common::itemLabel(dict, zh_RESUMEGAME, ccp(fx, fy - dis));
	item2->setTarget(this, menu_selector(LayerStart::resumeGame));
	CCMenuItem *item3 = Common::itemLabel(dict, zh_SETTINGS, ccp(fx, fy - 2 * dis));
	item3->setTarget(this, menu_selector(LayerStart::settingsCallBack));
	CCMenuItem *item4 = Common::itemLabel(dict, zh_HELP, ccp(fx, fy - 3 * dis));
	item4->setTarget(this, menu_selector(LayerStart::helpCallBack));
	CCMenuItem *item5 = Common::itemLabel(dict, zh_ABOUT, ccp(fx, fy - 4 * dis));
	item5->setTarget(this, menu_selector(LayerStart::aboutCallBack));

	CCMenu *menu = CCMenu::create(item1, item2, item3, item4, item5, NULL);
	menu->setPosition(CCPointZero);
	addChild(menu);
}

void LayerStart::newGame(CCObject*)
{
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}	
	//CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.8f, Common::scene(LayerGameLoading::create(0))));
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.8f, Common::scene(LayerPlayer::create())));
}

void LayerStart::resumeGame(CCObject*)
{
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}

	int life = CCUserDefault::sharedUserDefault()->getIntegerForKey("life", 0);	
	if (life)
	{
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.8f, Common::scene(LayerGameLoading::create(1))));
	}
	else
	{		
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.8f, Common::scene(LayerPlayer::create())));
	}	
}

void LayerStart::settingsCallBack(CCObject*)
{
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}
	this->addChild(LayerSettings::create());
}

void LayerStart::helpCallBack(CCObject*)
{
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}
	CCDirector::sharedDirector()->replaceScene(Common::scene(LayerHelp::create()));
}

void LayerStart::aboutCallBack(CCObject*)
{
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.0f, Common::scene(LayerAbout::create())));
}

/************************************************************************/
/* 玩家人物选择界面                                                     */
/************************************************************************/
bool LayerPlayer::init()
{
	if (!LayerBase1::init()){ return false; }

	m_PlayerID = 0;

	CCSprite *bg = CCSprite::create("switchbg.png");
	bg->setAnchorPoint(CCPointZero);
	addChild(bg);

	CCDictionary *dict = CCDictionary::createWithContentsOfFile(zh_CN_XML);

	addChild(Common::fontLabel("figure switch", ccp(winSize.width / 2, winSize.height - 50)));	

	m_PanelTexture = CCTextureCache::sharedTextureCache()->addImage("panel.png");	
	m_Player1 = CCSprite::createWithTexture(m_PanelTexture, CCRectMake(0, 0, 200, 220));
	m_Player1->setPosition(ccp(250, 250));
	m_Player2 = CCSprite::createWithTexture(m_PanelTexture, CCRectMake(0, 0, 200, 220));
	m_Player2->setPosition(ccp(495, 250));

	CCSprite *m_Player11 = CCSprite::create("playermario.png");
	m_Player11->setPosition(ccp(250, 250));
	CCSprite *m_Player21 = CCSprite::create("playerstallone.png");
	m_Player21->setPosition(ccp(495, 250));

	addChild(m_Player1);
	addChild(m_Player2);
	addChild(m_Player11);
	addChild(m_Player21);
	addChild(Common::fontLabel("Mario", ccp(250, 350)));
	addChild(Common::fontLabel("Stallone", ccp(500, 350)));

	m_PlayerBg = CCSprite::create();
	m_PlayerBg->setTextureRect(CCRectMake(0, 0, 250, 250));
	m_PlayerBg->setColor(ccGRAY);
	m_PlayerBg->setPosition(ccp(250, 250));
	m_PlayerBg->setOpacity(100);
	addChild(m_PlayerBg);

	CCSprite *startGameBg = CCSprite::createWithTexture(m_PanelTexture, CCRectMake(240, 124, 272, 68));
	startGameBg->setPosition(ccp(winSize.width / 2 + 5, 58));
	//startGameBg->setScale(0.9f);
	addChild(startGameBg);
	CCMenuItem *startGameitem = Common::itemLabel(dict, zh_ENTERGAME, ccp(winSize.width / 2, 60));
	startGameitem->setTarget(this, menu_selector(LayerPlayer::startGame));
	CCMenu *menu = CCMenu::create(startGameitem, NULL);
	menu->setPosition(CCPointZero);
	addChild(menu);

	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);
	return true;
}

void LayerPlayer::startGame(CCObject*)
{
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}
	//CCUserDefault::sharedUserDefault()->setIntegerForKey("playerID", m_PlayerID);
	LayerGameLoading *layer = LayerGameLoading::create(0, m_PlayerID);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.8f, Common::scene(layer)));
}

bool LayerPlayer::ccTouchBegan(CCTouch *pTouch, CCEvent*)
{
	return true;
}

void LayerPlayer::ccTouchEnded(CCTouch *pTouch, CCEvent*)
{
	if (m_Player1->boundingBox().containsPoint(pTouch->getLocation()))
	{
		if (Common::isPlayEffect)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
		}
		m_PlayerBg->setPosition(ccp(250, 250));
		m_PlayerID = 0;
	}
	else if (m_Player2->boundingBox().containsPoint(pTouch->getLocation()))
	{
		if (Common::isPlayEffect)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
		}
		m_PlayerBg->setPosition(ccp(500, 250));
		m_PlayerID = 1;
	}
}

/*bool LayerPlayer::isClick(CCTouch* pTouch)
{
	CCPoint ptStartLocation = pTouch->getStartLocation();
	CCPoint ptLoacation = pTouch->getLocation();
	return ptLoacation.getDistanceSq(ptStartLocation) < 25;
}*/