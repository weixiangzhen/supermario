#include "LayerGame.h"
#include "LayerStart.h"
#include "MonsterFriend.h"
#include "MonsterEnemy.h"
#include "MarioBullet.h"
#include "MonsterBullet.h"

LayerGameLoading* LayerGameLoading::create(int state)
{
	LayerGameLoading *pRet = new LayerGameLoading;
	if (pRet && pRet->init(state))
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

LayerGameLoading* LayerGameLoading::create(int state, int playerID)
{
	LayerGameLoading *pRet = new LayerGameLoading;
	if (pRet && pRet->init(state, playerID))
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

bool LayerGameLoading::init(int state)
{
	if (!LayerBase1::init()){ return false; }

	m_State = state;
	return true;
}

bool LayerGameLoading::init(int state, int playerID)
{
	if (!LayerGameLoading::init(state)){ return false; }

	m_PlayerID = playerID;
	return true;
}

void LayerGameLoading::onEnter()
{
	LayerBase1::onEnter();
	//正在加载动画
	Common::loadingAnimation(this);

	
	
	//bool isfirst = CCUserDefault::sharedUserDefault()->getBoolForKey("isfirst", true);
	int nowpass = 1;
	if (m_State)
	{
		nowpass = CCUserDefault::sharedUserDefault()->getIntegerForKey("nowpass") + 1;
	}
	else//点了开始游戏或者首次玩
	{
		
	}
	/*else if (m_State == 1)//点了继续游戏
	{
		int life = CCUserDefault::sharedUserDefault()->getIntegerForKey("life");
		if (life == 0)
		{
			CCUserDefault::sharedUserDefault()->setIntegerForKey("nowpass", 0);
			CCUserDefault::sharedUserDefault()->setIntegerForKey("life", 3);			
			CCUserDefault::sharedUserDefault()->setIntegerForKey("currentscore", 0);
		}
	}*/

	//int nowpass = CCUserDefault::sharedUserDefault()->getIntegerForKey("nowpass") + 1;
	CCDictionary *dict = CCDictionary::createWithContentsOfFile(zh_CN_XML);
	CCString *str = (CCString*)dict->objectForKey(zh_PASSHINT);	
	CCLabelBMFont *nowpassLabel = CCLabelBMFont::create(CCString::createWithFormat(str->getCString(), nowpass)->getCString(), GAME_FONT);
	nowpassLabel->setScale(0.8f);
	nowpassLabel->setPosition(ccp(winSize.width / 2, winSize.height - 100));	
	this->addChild(nowpassLabel);
	this->scheduleOnce(schedule_selector(LayerGameLoading::loadingFinish), 1.5f);
}

void LayerGameLoading::onExit()
{
	LayerBase1::onExit();
}

void LayerGameLoading::loadingFinish(float)
{
	LayerGame *layerGame = LayerGame::create(m_State, m_PlayerID);
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1.0f, Common::scene(layerGame)));
}
//重写父类addPopup
void LayerGameLoading::addPopup()
{
	m_PopupCount++;
	if (m_PopupCount == 1)
	{
		this->pauseSchedulerAndActions();//暂停游戏
	}
}
//重写父类subPopup
void LayerGameLoading::subPopup()
{
	m_PopupCount--;
	if (m_PopupCount == 0)
	{
		this->resumeSchedulerAndActions();//继续游戏
	}
}

/************************************************************************/
/* 游戏逻辑初始化                                                        */
/************************************************************************/
CCPoint LayerGame::m_StartPoint = CCPointZero;

LayerGame* LayerGame::create(int state,int playerID)
{
	LayerGame *pRet = new LayerGame;
	if (pRet && pRet->init(state, playerID))
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

bool LayerGame::init(int state, int playerID)
{
	if (!LayerBase2::init()){ return false; }

	if (state == 0)//重新开始游戏
	{
		this->resetData(playerID);
		LayerGame::m_StartPoint = CCPointZero;
	}
	recoverData();
	m_TempScore = m_CurrentScore;
	m_TimeCount = 180;

	preloadRes();

	return true;
}

//重置保存的数据
void LayerGame::resetData(int playerID)
{
	bool isfirst = CCUserDefault::sharedUserDefault()->getBoolForKey("isfirst", true);
	if (isfirst)
		CCUserDefault::sharedUserDefault()->setBoolForKey("isfirst", false);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("playerID", playerID);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("highpass", 0);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("nowpass", 0);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("life", 3);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("growstate", 0);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("growcount", 2);
	if (isfirst)
		CCUserDefault::sharedUserDefault()->setIntegerForKey("currentcoin", 0);//首次玩的时候会清零	
	CCUserDefault::sharedUserDefault()->setIntegerForKey("currentscore", 0);
	//记录金币和怪物计数器
	CCUserDefault::sharedUserDefault()->setIntegerForKey("coincount", 0);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("monstercount", 0);
	//CCUserDefault::sharedUserDefault()->flush();
}
void LayerGame::resetData()
{
	resetData(0);
}

//恢复保存的数据
void LayerGame::recoverData()
{
	m_PlayerID = CCUserDefault::sharedUserDefault()->getIntegerForKey("playerID");
	m_NowPass = CCUserDefault::sharedUserDefault()->getIntegerForKey("nowpass");
	m_life = CCUserDefault::sharedUserDefault()->getIntegerForKey("life");
	m_GrowCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("growcount");
	m_GrowState = CCUserDefault::sharedUserDefault()->getIntegerForKey("growstate");
	m_CurrentCoin = CCUserDefault::sharedUserDefault()->getIntegerForKey("currentcoin");	
	m_CurrentScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("currentscore");

	m_CoinCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("coincount");
	m_MonsterCount = CCUserDefault::sharedUserDefault()->getIntegerForKey("monstercount");
}

//存储要保存的数据
void LayerGame::saveData(bool isSaveGrowState)
{
	CCUserDefault::sharedUserDefault()->setIntegerForKey("playerID", m_PlayerID);
	int curpass = CCUserDefault::sharedUserDefault()->getIntegerForKey("nowpass", 0);
	if (m_NowPass > curpass)
	{
		CCUserDefault::sharedUserDefault()->setIntegerForKey("highpass", m_NowPass);
	}
	CCUserDefault::sharedUserDefault()->setIntegerForKey("nowpass", m_NowPass);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("life", m_life);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("growcount", m_GrowCount);
	if (isSaveGrowState)
		CCUserDefault::sharedUserDefault()->setIntegerForKey("growstate", m_Player->getGrowState());
	CCUserDefault::sharedUserDefault()->setIntegerForKey("currentcoin", m_CurrentCoin);
	if (isSaveGrowState)
		CCUserDefault::sharedUserDefault()->setIntegerForKey("currentscore", m_CurrentScore);
	//记录金币和怪物计数器
	CCUserDefault::sharedUserDefault()->setIntegerForKey("coincount", m_CoinCount);
	CCUserDefault::sharedUserDefault()->setIntegerForKey("monstercount", m_MonsterCount);
	//CCUserDefault::sharedUserDefault()->flush();
}

//层进入以后调用
void LayerGame::onEnter()
{
	LayerBase2::onEnter();

	MonsterFriend::m_MonsterFriends = CCArray::create();
	MonsterFriend::m_MonsterFriends->retain();

	MonsterEnemy::m_MonsterEnemys = CCArray::create();
	MonsterEnemy::m_MonsterEnemys->retain();

	Monster::m_MAwardDatas = CCArray::create();
	Monster::m_MAwardDatas->retain();

	Bullet::m_Bullets = CCArray::create();
	Bullet::m_Bullets->retain();

	MonsterBullet::m_Bullets = CCArray::create();
	MonsterBullet::m_Bullets->retain();

	Biology::m_Stairs = CCArray::create();
	Biology::m_Stairs->retain();	

	addMap();
	addBiology();
	addButton();
	addInfo();

	scheduleUpdate();
	schedule(schedule_selector(LayerGame::timerout), 1.0f);

	setTouchEnabled(true);

	if (m_life == 0)
	{
		//弹出game over对话框
		overDialog(false);		
	}
	else
	{
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(musicNames[0], true);
		if (!Common::isPlayMusic)
		{
			SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
		}		
	}
}

//层退出以后调用
void LayerGame::onExit()
{
	LayerBase2::onExit();
	
	MonsterFriend::m_MonsterFriends->release();	
	MonsterEnemy::m_MonsterEnemys->release();
	Monster::m_MAwardDatas->release();	
	Bullet::m_Bullets->release();
	MonsterBullet::m_Bullets->release();
	Biology::m_Stairs->release();
}

/************************************************************************/
/* 预加载所有动画或帧图片                                                 */
/************************************************************************/
void LayerGame::preloadRes()
{		
	if (m_PlayerID)	
		Stallone::preload();
	else
		Mario::preload();
	
	Monster::preload();
	Bullet::preload();	

	{//特效
		CCTexture2D *pEffectTexture = CCTextureCache::sharedTextureCache()->addImage("effect.png");
		CCArray *arr = CCArray::create();

		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pEffectTexture, CCRectMake(0, 0, 128, 128));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pEffectTexture, CCRectMake(128, 0, 128, 128));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pEffectTexture, CCRectMake(128, 0, 128, 128));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pEffectTexture, CCRectMake(128, 128, 128, 128));
		arr->addObject(frame);

		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.1f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "Effect");
	}
}

void LayerGame::addInfo()
{
	CCDictionary *dict = CCDictionary::createWithContentsOfFile(zh_CN_XML);
	CCLabelBMFont *lifeLabel = Common::fontLabel(dict, zh_LIFETEXT, ccp(120, winSize.height - 40), 0.6f);
	CCLabelBMFont *coinLabel = Common::fontLabel(dict, zh_COINTEXT, ccp(220, winSize.height - 40), 0.6f);
	CCLabelBMFont *scoreLabel = Common::fontLabel(dict, zh_SCORETEXT, ccp(450, winSize.height - 40), 0.6f);
	CCLabelBMFont *timeLabel = Common::fontLabel(dict, zh_TIMETEXT, ccp(650, winSize.height - 40), 0.6f);

	this->addChild(lifeLabel);
	this->addChild(coinLabel);
	this->addChild(scoreLabel);
	this->addChild(timeLabel);	

	m_LifeLabel = Common::fontLabel(Common::format(m_life), ccp(150, winSize.height - 40), 0.6f);
	m_LifeLabel->setAnchorPoint(ccp(0, 0.5f));
	m_GrowLabel = Common::fontLabel(Common::format(m_GrowCount), ccp(winSize.width / 2 - 10, winSize.height - 40), 0.6f);
	m_GrowLabel->setZOrder(150);
	m_CoinLabel = Common::fontLabel(Common::format(m_CurrentCoin), ccp(250, winSize.height - 40), 0.6f);
	m_CoinLabel->setAnchorPoint(ccp(0, 0.5f));
	m_ScoreLabel = Common::fontLabel(Common::format(m_CurrentScore), ccp(480, winSize.height - 40), 0.6f);
	m_ScoreLabel->setAnchorPoint(ccp(0, 0.5f));
	m_TimeLabel = Common::fontLabel(Common::format(m_TimeCount), ccp(680, winSize.height - 40), 0.6f);
	m_TimeLabel->setAnchorPoint(ccp(0, 0.5f));

	this->addChild(m_LifeLabel);
	this->addChild(m_GrowLabel);
	this->addChild(m_CoinLabel);
	this->addChild(m_ScoreLabel);
	this->addChild(m_TimeLabel);
}

void LayerGame::addMap()
{
	//添加背景
	CCSprite *bgColor = CCSprite::create();
	bgColor->setTextureRect(CCRectMake(0, 0, winSize.width, winSize.height));
	bgColor->setColor(ccc3(75, 110, 245));
	bgColor->setAnchorPoint(ccp(0, 0));

	this->addChild(bgColor);

	//创建地图		
	m_Map = CCTMXTiledMap::create(Common::format(m_NowPass + 1, "tiledmap/mary", ".tmx"));
	this->addChild(m_Map);
	//防止地图抖动，地图放大后使用，通过给TiledMap的每个图层的纹理开启抗锯齿效果来解决这个问题
	/*CCArray *pChildrenArray = m_Map->getChildren();
	CCSpriteBatchNode *child = NULL;
	CCObject *pObject = NULL;
	CCARRAY_FOREACH(pChildrenArray, pObject)
	{
		child = (CCSpriteBatchNode *)pObject;
		if (!child)
			break;
		child->getTexture()->setAntiAliasTexParameters();
	}*/
	//所有对象层
	CCTMXLayer *objectsLayer = m_Map->layerNamed("object_image");
	if (objectsLayer)
	{
		objectsLayer->removeFromParent();
	}	

	//各个层的基本设置
	CCTMXLayer *monsterLayer = m_Map->layerNamed("monster_image");
	monsterLayer->setZOrder(Common::Z_MONSTER);
	CCTMXLayer *barrierLayer = m_Map->layerNamed("barrier");
	barrierLayer->setZOrder(Common::Z_BARRIER);
	CCTMXLayer *groundLayer = m_Map->layerNamed("ground");
	groundLayer->setZOrder(Common::Z_GROUND);
	CCTMXLayer *backgroundLayer = m_Map->layerNamed("background");
	if (backgroundLayer)
	{
		backgroundLayer->setZOrder(Common::Z_BACKGROUND);
	}	

	CCTMXLayer *hillLayer = m_Map->layerNamed("hill"); 
	hillLayer->setZOrder(Common::Z_HILL);
	//hillLayer->setTag(1002);
	CCTMXLayer *smallCloudLayer = m_Map->layerNamed("small_cloud"); 
	smallCloudLayer->setZOrder(Common::Z_CLOUD);
	CCTMXLayer *bigCloudLayer = m_Map->layerNamed("big_cloud");
	bigCloudLayer->setScale(2.0f);
	bigCloudLayer->setZOrder(Common::Z_CLOUD);
	//cloudLayer->setTag(1001);
	CCTMXLayer *pillarLayer = m_Map->layerNamed("pillar");
	pillarLayer->setZOrder(Common::Z_PILLAR);
	//pillarLayer->setTag(1000);		
}

void LayerGame::addBiology()
{
	//创建马里奥
	//m_Player = NULL;	
	//CCLog("playerID = %d", m_PlayerID);
	if (m_PlayerID)
	{
		m_Player = Stallone::create();		
		if (m_GrowState)
			m_Player->setGrowState(m_GrowState, true);
		else		
			m_Player->setGrowState(1, true);			
	}
	else
	{
		m_Player = Mario::create();
		m_Player->setGrowState(m_GrowState, true);
	}
	
	m_Map->addChild(m_Player);	

	m_OverPoint = CCPointZero;
	m_IsBossDead = false;
	//所有怪物对象
	CCTMXLayer *monsterLayer = m_Map->layerNamed("monster_image");
	//获取所有对象
	CCTMXObjectGroup *objectGroup = m_Map->objectGroupNamed("object");
	CCArray *objects = objectGroup->getObjects();
	if (objects && objects->count() > 0)
	{
		CCObject* pObj = NULL;
		CCARRAY_FOREACH(objects, pObj)
		{
			CCDictionary* dict = (CCDictionary*)pObj;

			const int x = dict->valueForKey("x")->intValue();
			const int y = dict->valueForKey("y")->intValue();
			const CCString *name = dict->valueForKey("name");
			if (name->m_sString == "dispenser")//出生点
			{
				if (dict->valueForKey("active")->boolValue() && m_Player->getPositionX() == 0)//说明是地图的起点
				{
					m_Player->setPosition(ccp(x, y));
				}
				if (m_StartPoint.x <= m_Player->getPositionX())//比最初出生点还要小就死了
				{
					continue;
				}
				else if (m_StartPoint.x > x)
				{
					if (m_Player->getPositionX() <= x)
					{
						//CCLog("Figure %g,%d", m_Player->getPositionX(), x);
						m_Player->setPosition(ccp(x, y));
					}					
				}
				//CCLog("%g,%g", m_Player->getPositionX(), m_StartPoint.x);
			}
			else if (name->m_sString == "brick")//砖块金币
			{
				CCPoint pp = Common::pointToTile(m_Map, ccp(x, y));

				CCSprite *spr = monsterLayer->tileAt(pp);
				//CCLog("%x,%x", spr->getParent()->getParent(), m_Map);
				if (dict->valueForKey("hide")->boolValue())
				{
					spr->setVisible(false);
				}
				const CCString *m_a = dict->valueForKey("a");
				if (m_a->m_sString == "appbrickcoin")
				{
					const CCString *m_nokill = dict->valueForKey("nokill");
					Brick *brick = Brick::create(m_Player, spr, m_nokill->boolValue());
					const CCString *m_b = dict->valueForKey("b");
					if (m_b->m_sString == "behbrickmakeprop")
					{
						brick->setMake(true);
					}
					MonsterFriend::m_MonsterFriends->addObject(brick);
				}
				else if (m_a->m_sString == "appbrickrock")
				{
					Rock *rock = Rock::create(m_Player, spr);
					const CCString *m_b = dict->valueForKey("b");
					if (m_b->m_sString == "behbrickrepeat")
					{
						rock->setMake(true);
					}
					MonsterFriend::m_MonsterFriends->addObject(rock);
				}
			}
			else if (name->m_sString == "prop")//金币+部分敌人
			{
				CCPoint pp = Common::pointToTile(m_Map, ccp(x, y));

				CCSprite *spr = monsterLayer->tileAt(pp);
				const CCString *m_a = dict->valueForKey("a");
				if (m_a->m_sString == "apppropcoin")
				{
					Prop *prop = Prop::create(m_Player, spr);
					MonsterFriend::m_MonsterFriends->addObject(prop);
				}else if (m_a->m_sString == "appproppiranha")
				{
					Piranha *piranha = Piranha::create(m_Player);
					piranha->setPosition(Common::tileToPoint(m_Map, Common::pointToTile(m_Map, ccp(x, y))));
					//piranha->setPosition(ccp(winSize.width/2, winSize.height/2+100));					
					piranha->setVS(piranha->getPositionY() + m_Map->getTileSize().height * 2, piranha->getPositionY());
					m_Map->addChild(piranha);
					MonsterFriend::m_MonsterFriends->addObject(piranha);
				}else if (m_a->m_sString == "apppropsflow")
				{
					SFlower *sflower = SFlower::create(m_Player);
					sflower->setPosition(Common::tileToPoint(m_Map, Common::pointToTile(m_Map, ccp(x, y))));
					m_Map->addChild(sflower);
					MonsterFriend::m_MonsterFriends->addObject(sflower);
				}else if (m_a->m_sString == "apppropanbiao")
				{
					AnBiao *anbiao = AnBiao::create(m_Player);
					anbiao->setPosition(ccp(x, y));
					anbiao->setRange(anbiao->getPositionY(), anbiao->getPositionY() + m_Map->getTileSize().height * 7);
					m_Map->addChild(anbiao);
					MonsterFriend::m_MonsterFriends->addObject(anbiao);
				}
				else if (m_a->m_sString == "apppropdisk")
				{
					int mwidth = dict->valueForKey("width")->intValue();
					int mheight = dict->valueForKey("height")->intValue();
					Disk *disk = Disk::create(m_Player, ccp(x + mwidth / 2, y + mheight / 2), 100, dict->valueForKey("nokill")->boolValue());
					disk->setPosition(ccp(x + mwidth / 2, y + mheight / 2 - 100));
					m_Map->addChild(disk);
					MonsterFriend::m_MonsterFriends->addObject(disk);
				}
			}
			else if (name->m_sString == "monster")//所有敌人
			{
				const CCString *m_a = dict->valueForKey("a");
				int leftscope = dict->valueForKey("leftscope")->intValue();
				int rightscope = dict->valueForKey("rightscope")->intValue();
				int upscope = dict->valueForKey("upscope")->intValue();
				int downscope = dict->valueForKey("downscope")->intValue();
				//CCLog("%d,%d,%d,%d", leftscope, rightscope, upscope, downscope);
				MonsterEnemy *monster;
				if (m_a->m_sString == "appmonstersmall")
				{
					MonsterSmall *msmall = MonsterSmall::create(m_Player, false);
					monster = msmall;
					msmall->setPosition(ccp(x, y));
					//const CCString *m_b = dict->valueForKey("b");
					m_Map->addChild(msmall);
					MonsterEnemy::m_MonsterEnemys->addObject(msmall);
				}
				else if (m_a->m_sString == "appbackstab")
				{
					Backstab *stab = Backstab::create(m_Player);
					monster = stab;
					stab->setPosition(ccp(x, y));
					m_Map->addChild(stab);
					MonsterEnemy::m_MonsterEnemys->addObject(stab);
				}
				else if (m_a->m_sString == "appmonstersmallblack")
				{
					MonsterSmall *msmall = MonsterSmall::create(m_Player, true);
					monster = msmall;
					msmall->setPosition(ccp(x, y));
					m_Map->addChild(msmall);
					MonsterEnemy::m_MonsterEnemys->addObject(msmall);
				}
				else if (m_a->m_sString == "apptortoise")
				{
					const CCString *m_State = dict->valueForKey("state");
					Tortoise *tortoise = Tortoise::create(m_Player, m_State->intValue());
					monster = tortoise;
					tortoise->setPosition(ccp(x, y));
					m_Map->addChild(tortoise);
					MonsterEnemy::m_MonsterEnemys->addObject(tortoise);
				}
				else if (m_a->m_sString == "apppropstair")
				{
					int dir = dict->valueForKey("dir")->intValue();
					Stair *stair = Stair::create(m_Player, dir);
					monster = stair;
					stair->setPosition(ccp(x, y));
					m_Map->addChild(stair);
					MonsterEnemy::m_MonsterEnemys->addObject(stair);
					Biology::m_Stairs->addObject(stair);
				}else if (m_a->m_sString == "appcannon")
				{
					/*int mwidth = dict->valueForKey("width")->intValue();
					int mheight = dict->valueForKey("height")->intValue();
					CCLog("%d,%d", mwidth, mheight);*/
					Cannon *cannon = Cannon::create(m_Player);
					monster = cannon;
					cannon->setPosition(ccp(x, y));
					m_Map->addChild(cannon);
					MonsterEnemy::m_MonsterEnemys->addObject(cannon);
				}
				else if (m_a->m_sString == "apphelmet")
				{
					Helmet *helmet = Helmet::create(m_Player);
					monster = helmet;
					helmet->setPosition(ccp(x, y));
					m_Map->addChild(helmet);
					MonsterEnemy::m_MonsterEnemys->addObject(helmet);
				}
				else if (m_a->m_sString == "appboss")
				{
					Boss *boss = Boss::create(m_Player);
					monster = boss;
					boss->setPosition(ccp(x, y));
					boss->setDead(&m_IsBossDead);
					m_Map->addChild(boss);
					MonsterEnemy::m_MonsterEnemys->addObject(boss);
				}
				else
				{
					monster = NULL;
				}
				if (monster)
				{
					monster->setLeftScope(leftscope * m_Map->getTileSize().width);
					monster->setRightScope(rightscope * m_Map->getTileSize().width);
					monster->setUpScope(upscope * m_Map->getTileSize().height);
					monster->setDownScope(downscope * m_Map->getTileSize().height);
				}
			}
			else if (name->m_sString == "over")
			{
				m_OverPoint = ccp(x, y);//终点的位置
			}
			else if (name->m_sString == "end")
			{
				EndBlack *end = EndBlack::create(m_Player);
				end->setPosition(ccp(x, y));
				m_Map->addChild(end);
				MonsterFriend::m_MonsterFriends->addObject(end);
			}
		}
	}

	//x轴调整
	float fx = m_Player->getPositionX() - winSize.width / 2;
	if (fx > 0)
	{
		//移动地图
		m_Map->setPositionX(-fx);
		Common::bgMove(m_Map, fx / 2);
	}
	//y轴调整
	float fy = m_Player->getPositionY() - winSize.height * 3 / 5;
	if (fy > 0)
	{		
		m_Map->setPositionY(-fy);//移动地图
		//CCLog("xxxxxxxxxx,%g", fy);
	}
}

void LayerGame::addAchievementEffect(int state)
{
	if (this->getChildByTag(2000))
	{
		//this->removeChildByTag(2000);
		return;
	}
	CCSprite *bg = CCSprite::create("panel.png", CCRectMake(240, 124, 272, 68));
	bg->setPosition(ccp(60, winSize.height - 110));
	bg->setAnchorPoint(ccp(0, 0));
	bg->setScale(0.8f);
	bg->setOpacity(155);
	bg->setTag(2000);
	
	for (int i = 0; i < 3; i++)
	{
		CCSprite *effect = CCSprite::create();
		effect->setDisplayFrameWithAnimationName("Effect", 0);
		//effect->setAnchorPoint(ccp(0, 0));
		effect->setPosition(ccp(64 + i*effect->getContentSize().width / 2, 32));
		CCAnimate *animate1 = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("Effect"));
		effect->runAction(CCRepeatForever::create(animate1));
		bg->addChild(effect);
	}

	CCDictionary *dict = CCDictionary::createWithContentsOfFile(zh_CN_XML);
	bg->addChild(Common::fontLabel(dict, zh_ACHIEVEMENT, ccp(120, 50), 0.6f));

	CCString *str = NULL;
	if (state == 1)
	{
		str = (CCString*)dict->objectForKey(zh_ACHIEVEMENTCOIN);	
	}
	else if (state == 2)
	{
		str = (CCString*)dict->objectForKey(zh_ACHIEVEMENTMONSTER);	
	}	
	if (str)
	{
		CCLabelBMFont *labelFont = CCLabelBMFont::create(str->getCString(), "font/mario1.fnt");
		labelFont->setScale(0.5f);
		labelFont->setPosition(ccp(130, 25));
		bg->addChild(labelFont);
	}	
	
	this->addChild(bg);

	scheduleOnce(schedule_selector(LayerGame::removeEffect), 3.0f);
}

void LayerGame::removeEffect(float)
{
	this->removeChildByTag(2000);
}

/* 注册触摸 */
void LayerGame::registerWithTouchDispatcher()
{
	CCDirector::sharedDirector()->getTouchDispatcher()->addStandardDelegate(this, 0);
}

//多点触摸：开始
void LayerGame::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator iter = pTouches->begin();
	for (; iter != pTouches->end(); iter++)
	{
		CCTouch* pTouch = (CCTouch*)(*iter);
		//CCLog("%d", pTouch->getID());		
		if (!m_LeftButton->isPressed() && !m_RightButton->isPressed())
		{
			if (m_LeftButton->boundingBox().containsPoint(pTouch->getLocation()))//按下左键
			{
				m_LeftButton->setID(pTouch->getID());
				m_LeftButton->setChecked(true);
				m_Player->setRunDir(Figure::MOVE_LEFT);
			}
			else if (m_RightButton->boundingBox().containsPoint(pTouch->getLocation()))//按下右键
			{
				m_RightButton->setID(pTouch->getID());
				m_RightButton->setChecked(true);
				m_Player->setRunDir(Figure::MOVE_RIGHT);
			}
		}

		//按下A键
		if (!m_AButton->isPressed() && m_AButton->boundingBox().containsPoint(pTouch->getLocation()))
		{
			m_AButton->setID(pTouch->getID());
			m_AButton->setChecked(true);
			m_Player->setShoot(true);
		}
		//按下B键
		if (!m_BButton->isPressed() && m_BButton->boundingBox().containsPoint(pTouch->getLocation()))
		{
			m_BButton->setID(pTouch->getID());
			m_BButton->setChecked(true);
			m_Player->setJump(true);
		}
	}
}
//多点触摸：移动
void LayerGame::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator iter = pTouches->begin();
	for (; iter != pTouches->end(); iter++)
	{
		CCTouch* pTouch = (CCTouch*)(*iter);
		//CCLog("%d", pTouch->getID());		
		if (!m_LeftButton->isPressed() && m_LeftButton->boundingBox().containsPoint(pTouch->getLocation()))//移动到左键
		{
			if (m_RightButton->isPressed())
			{
				m_RightButton->setChecked(false);
				m_RightButton->setID(-1);
				//马里奥刹车
			}
			m_LeftButton->setID(pTouch->getID());
			m_LeftButton->setChecked(true);
			m_Player->setRunDir(Figure::MOVE_LEFT);
		}else if (!m_RightButton->isPressed() && m_RightButton->boundingBox().containsPoint(pTouch->getLocation()))//移动到右键
		{
			if (m_LeftButton->isPressed())
			{
				m_LeftButton->setChecked(false);
				m_LeftButton->setID(-1);
				//马里奥刹车
			}
			m_RightButton->setID(pTouch->getID());
			m_RightButton->setChecked(true);
			m_Player->setRunDir(Figure::MOVE_RIGHT);
		}		

		//移动到A键
		/*if (!m_AButton->isPressed() && m_AButton->boundingBox().containsPoint(pTouch->getLocation()))
		{
			m_AButton->setChecked(true);
			m_Player->setShoot(true);
		}
		//移动到B键
		if (!m_BButton->isPressed() && m_BButton->boundingBox().containsPoint(pTouch->getLocation()))
		{
			m_BButton->setChecked(true);
			m_Player->setJump(true);
		}*/

		//移动到大于屏幕的一半，自动松开按键
		/*if (pTouch->getLocation().x > winSize.width / 2)
		{
			if (m_LeftButton->isPressed())
			{
				m_LeftButton->setChecked(false);
				m_Player->setRunDir(Figure::MOVE_NONE);
			}
			else if (m_RightButton->isPressed())
			{
				m_RightButton->setChecked(false);
				m_Player->setRunDir(Figure::MOVE_NONE);
			}
		}
		else
		{
			if (m_AButton->isPressed())
			{
				m_AButton->setChecked(false);
				m_Player->setShoot(false);
			}
			if (m_BButton->isPressed())
			{
				m_BButton->setChecked(false);
				m_Player->setJump(false);
			}
		}*/
	}
}
//多点触摸：松开
void LayerGame::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	CCSetIterator iter = pTouches->begin();
	for (; iter != pTouches->end(); iter++)
	{
		CCTouch* pTouch = (CCTouch*)(*iter);
		//CCLog("%d", pTouch->getID());
		//判断是否松开左键
		if (m_LeftButton->isPressed() && m_LeftButton->getID() != -1 && m_LeftButton->getID() == pTouch->getID())
		{
			m_LeftButton->setChecked(false);
			m_Player->setRunDir(Figure::MOVE_NONE);
		}
		//判断是否松开右键
		if (m_RightButton->isPressed() && m_RightButton->getID() != -1 && m_RightButton->getID() == pTouch->getID())
		{
			m_RightButton->setChecked(false);
			m_Player->setRunDir(Figure::MOVE_NONE);
		}
		//判断是否松开A键
		if (m_AButton->isPressed() && m_AButton->getID() != -1 && m_AButton->getID() == pTouch->getID())
		{
			m_AButton->setChecked(false);
			m_Player->setShoot(false);
		}
		//判断是否松开B键
		if (m_BButton->isPressed() && m_BButton->getID() != -1 && m_BButton->getID() == pTouch->getID())
		{
			m_BButton->setChecked(false);
			m_Player->setJump(false);
		}
	}
}

void LayerGame::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent)
{
	ccTouchesEnded(pTouches, pEvent);
}

void LayerGame::addButton()
{
	CCTexture2D *pControlTexture = CCTextureCache::sharedTextureCache()->addImage("control.png");
	CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pControlTexture, CCRectMake(0, 0, 120, 120));
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "DirNormal");
	frame = CCSpriteFrame::createWithTexture(pControlTexture, CCRectMake(120, 0, 120, 120));
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "DirPressed");

	frame = CCSpriteFrame::createWithTexture(pControlTexture, CCRectMake(240, 0, 120, 120));
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "ANormal");
	frame = CCSpriteFrame::createWithTexture(pControlTexture, CCRectMake(360, 0, 120, 120));
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "APressed");

	frame = CCSpriteFrame::createWithTexture(pControlTexture, CCRectMake(0, 120, 120, 120));
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "BNormal");
	frame = CCSpriteFrame::createWithTexture(pControlTexture, CCRectMake(120, 120, 120, 120));
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "BPressed");

	m_LeftButton = Button::create("DirNormal", "DirPressed");
	m_LeftButton->setPosition(ccp(70, 70));

	m_RightButton = Button::create("DirNormal", "DirPressed");
	m_RightButton->setFlipX(true);
	m_RightButton->setPosition(ccp(210, 70));

	m_AButton = Button::create("ANormal", "APressed");
	m_AButton->setPosition(ccp(600, 70));
	m_BButton = Button::create("BNormal", "BPressed");
	m_BButton->setPosition(ccp(730, 120));

	this->addChild(m_LeftButton);
	this->addChild(m_RightButton);
	this->addChild(m_AButton);
	this->addChild(m_BButton);	
}

void LayerGame::update(float dt)
{
	//判断马里奥是否过关
	if (!m_Player->isAutoRun() && !m_Player->isFly() && m_OverPoint.x && m_Player->getPositionX() > m_OverPoint.x)
	{
		m_Player->setAutoRun(true);
		m_Player->setRunDistance(m_OverPoint.x + 300);//往终点方向继续走300	

		if (Common::isPlayEffect)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(effectNames[11], false);
		}
		if (Common::isPlayMusic)
		{
			SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
		}
	}
	else if (!m_Player->isAutoRun() && m_IsBossDead)
	{
		m_Player->setAutoRun(true);
		//m_Player->setRunDistance(0);
		if (Common::isPlayEffect)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(effectNames[11], false);
		}
		if (Common::isPlayMusic)
		{
			SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(true);
		}
	}
//通过键盘控制操作
#ifdef WIN32

	if (!m_LeftButton->isPressed() && !m_RightButton->isPressed())
	{
		if (GetKeyState('F') < 0)
		{
			m_Player->setRunDir(Figure::MOVE_RIGHT);
		}
		else if (GetKeyState('D') < 0)
		{
			m_Player->setRunDir(Figure::MOVE_LEFT);
		}
		else if (m_Player->getRunDir() != Figure::MOVE_NONE)
		{
			m_Player->setRunDir(Figure::MOVE_NONE);
			//CCLog("padkey none");
		}
	}
	
	if (!m_AButton->isPressed())
	{
		if (GetKeyState('J') < 0)
		{
			m_Player->setShoot(true);
		}
		else if (m_Player->isShoot())
		{
			m_Player->setShoot(false);
			//CCLog("padkey none");
		}
	}

	if (GetKeyState('K') < 0) m_Player->jump();
#endif	

	CCObject *obj = NULL;	
	if (!m_Player->isAutoRun())
	{
		//友方怪物的碰撞检测
		CCARRAY_FOREACH_REVERSE(MonsterFriend::m_MonsterFriends, obj)
		{
			Monster *monster = (Monster*)obj;
			monster->logic(dt);
		}
	}
	//敌方怪物的碰撞检测
	CCARRAY_FOREACH_REVERSE(MonsterEnemy::m_MonsterEnemys, obj)
	{
		Monster *monster = (Monster*)obj;
		monster->logic(dt);	//逻辑	
	}
	
	//敌方怪物与子弹碰撞
	CCARRAY_FOREACH_REVERSE(MonsterEnemy::m_MonsterEnemys, obj)
	{
		Monster *monster = (Monster*)obj;
		CCARRAY_FOREACH_REVERSE(Bullet::m_Bullets, obj)
		{
			Bullet *bullet = (Bullet*)obj;
			if (bullet->getLife() && monster->getLife() && Common::isOnScreen(m_Map, monster) && bullet->isCollision(monster))
			{
				//monster->setLife(0);//把生命写到autoDead函数里
				monster->autoDead(bullet, 1);
				bullet->autoDead();
			}
		}
	}
	//马里奥子弹逻辑，主要是移动
	CCARRAY_FOREACH_REVERSE(Bullet::m_Bullets, obj)
	{
		Bullet *bullet = (Bullet*)obj;
		bullet->logic(dt);
	}
	//敌方子弹碰撞检测
	CCARRAY_FOREACH_REVERSE(MonsterBullet::m_Bullets, obj)
	{
		MonsterBullet *bullet = (MonsterBullet*)obj;

		//与马里奥碰撞不用移除子弹
		if (bullet->isCollision(m_Player))
		{			
			m_Player->loseLife();			
		}		
		bullet->logic(dt);			
	}
	//怪物奖励检测
	CCARRAY_FOREACH_REVERSE(Monster::m_MAwardDatas, obj)
	{
		MAwardData *data = (MAwardData*)obj;
		if (data->isMakeCoin())
		{
			//金币加1
			m_CurrentCoin += 1;
			//有动画效果，新成就，连续吃100金币奖励50金币
			m_CoinCount += 1;
			if (m_CoinCount % 100 == 0)
			{
				addAchievementEffect(1);
				m_CurrentCoin += 50;
			}
			m_CoinLabel->setString(Common::format(m_CurrentCoin));
		}
		if (data->getScore() > 0)
		{
			m_TempScore += data->getScore();
			CCLabelBMFont *label = Common::fontLabel(Common::format(data->getScore(), "+"), data->getPosition(), 0.6f);
			CCMoveBy *by = CCMoveBy::create(1.0f, ccp(0, 80));
			CCCallFuncN *done = CCCallFuncN::create(this, callfuncN_selector(LayerGame::removeNode));
			CCSequence *seq = CCSequence::create(by, done, NULL);
			label->runAction(seq);
			m_Map->addChild(label, Common::Z_MONSTER);

			if (data->isMonster())
			{
				//有动画效果，新成就，连续杀死100个怪物奖励50金币
				m_MonsterCount += 1;
				if (m_MonsterCount % 100 == 0)
				{
					addAchievementEffect(2);
					m_CurrentCoin += 50;
					m_CoinLabel->setString(Common::format(m_CurrentCoin));
				}
			}
		}
		Monster::m_MAwardDatas->removeObject(data);
	}

	//判断是否要增加分数
	if (m_CurrentScore < m_TempScore)
	{
		m_CurrentScore += 10;
		if (m_CurrentScore >= m_TempScore)
		{
			m_CurrentScore = m_TempScore;
		}		
		
		m_ScoreLabel->setString(Common::format(m_CurrentScore));
	}
	else if (m_Player->isAutoRun())
	{
		m_TimeCount-=4;

		m_CurrentScore += 4*5;
		m_ScoreLabel->setString(Common::format(m_CurrentScore));
		if (m_TimeCount <= 0)
		{
			m_TimeCount = 0;
			unscheduleUpdate();//停止计时器
			m_StartPoint = CCPointZero;//过关的时候把开始点清零
			m_NowPass++;
			saveData(true);

			this->runAction(CCSequence::create(CCDelayTime::create(1.0f), CCCallFunc::create(this, callfunc_selector(LayerGame::nextPass)), NULL));
		}

		m_TimeLabel->setString(Common::format(m_TimeCount));		
	}

	if (m_Player->isDead() && m_life)//说明马里奥已经完全死亡了
	{	
		unscheduleUpdate();//停止计时器
		m_StartPoint = m_Player->getPosition();//获取马里奥死亡的位置
		--m_life;
		m_Player->setGrowState(0, true);
		saveData(true);
		//替换场景
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, Common::scene(LayerGameLoading::create(2))));
		
	}
}

void LayerGame::nextPass()
{
	if (m_NowPass == 15)
	{		
		resetData();		
		CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, Common::scene(LayerWin::create())));
		return;
	}
	//切换到下一关
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, Common::scene(LayerGameLoading::create(2))));
}

void LayerGame::removeNode(CCNode *node)
{
	node->removeFromParent();
}

//暂停游戏
void LayerGame::pauseGame(CCNode *node)
{
	node->pauseSchedulerAndActions();

	CCObject * obj;
	CCARRAY_FOREACH(node->getChildren(), obj)
	{
		CCNode *n = (CCNode*)obj;
		pauseGame(n);
	}
}

void LayerGame::pauseGame()
{
	CCNode *effectNode = this->getChildByTag(2000);
	if (effectNode)
	{
		pauseGame(effectNode);
	}
	pauseGame(m_Map);
	this->pauseSchedulerAndActions();
}

//恢复游戏
void LayerGame::resumeGame(CCNode *node)
{
	node->resumeSchedulerAndActions();

	CCObject * obj;
	CCARRAY_FOREACH(node->getChildren(), obj)
	{
		CCNode *n = (CCNode*)obj;
		resumeGame(n);
	}
}

void LayerGame::resumeGame()
{
	CCNode *effectNode = this->getChildByTag(2000);
	if (effectNode)
	{
		resumeGame(effectNode);
	}
	resumeGame(m_Map);
	this->resumeSchedulerAndActions();
}

void LayerGame::addPopup()
{
	//LayerBase1::addPopup();
	m_PopupCount++;
	if (m_PopupCount == 1)
	{
		pauseGame();
	}
}

void LayerGame::subPopup()
{
	//LayerBase1::subPopup();
	m_PopupCount--;
	if (m_PopupCount == 0)
	{
		resumeGame();
	}
}

//成长蘑菇，点击使用
void LayerGame::growCallBack(CCObject *sender)
{
	LayerBase2::growCallBack(sender);
	if (m_Player->getLife() <= 0)
		return;
	
	if (m_GrowCount)
	{
		m_GrowCount--;
		m_GrowLabel->setString(Common::format(m_GrowCount));
		//m_Player->setGrowState(m_Player->getGrowState() + 1);
		DarkFoodMushroom *fm = DarkFoodMushroom::create(m_Player);
		fm->setPosition(m_Map->convertToNodeSpace(m_GrowLabel->getPosition()));
		m_Map->addChild(fm);
		MonsterFriend::m_MonsterFriends->addObject(fm);
		CCUserDefault::sharedUserDefault()->setIntegerForKey("growstate", m_Player->getGrowState() + 1);
	}	
}

void LayerGame::timerout(float)
{
	if (m_Player->getLife() <= 0 || m_Player->isAutoRun() || !m_TimeCount)
	{
		unschedule(schedule_selector(LayerGame::timerout));
		return;
	}

	m_TimeCount--;
	m_TimeLabel->setString(Common::format(m_TimeCount));
	if (m_TimeCount == 0)
	{
		m_Player->setLife(0);
		m_Player->dead();
	}
}

#if 0
CCTMXObjectGroup *objGroup = map_physical->tmxMap->objectGroupNamed("shan");
CCArray *objArray = objGroup->getObjects();


if (objArray && objArray->count() > 0)
{
	CCObject* pObj = NULL;
	CCARRAY_FOREACH(objArray, pObj)
	{
		CCDictionary* spawnPoint = (CCDictionary*)pObj;

		CCDictElement *keyString = NULL;
		CCDICT_FOREACH(spawnPoint, keyString)
		{
			const char *xps = keyString->getStrKey();
			CCLog("dict:key %s", xps);
			CCString  *xps1 = (CCString *)(keyString->getObject());
			CCLog("dict:value %f", xps1->floatValue());
		}
	}
}
#endif

void LayerWin::backGame(CCObject*)
{
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(0.5f, Common::scene(LayerStart::create())));
}