#include "MonsterFriend.h"

CCArray *MonsterFriend::m_MonsterFriends = NULL;

//创建MonsterFriend对象
MonsterFriend* MonsterFriend::create(Biology *obj, CCSprite *spr)
{
	MonsterFriend *pRet = new MonsterFriend;
	if (pRet && pRet->init(obj, spr))
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
//初始化
bool MonsterFriend::init(Biology *obj, CCSprite *spr)
{
	if (!Monster::init(obj)){ return false; }		
	
	m_Sprite = spr;
	m_IsMake = false;
	m_IsRun = false;
	return true;
}

//判断该砖块的底部是不是被碰撞到了
bool MonsterFriend::isCollisionBottom(Biology *obj)
{
	if (obj->getLife() <= 0)	
		return false;
	
	CCRect rect = obj->boundingBox();
	//1、要强转成int是因为地图都是int的，没有小数点后的
	CCPoint p[3] = { ccp(rect.getMidX(), rect.getMaxY()), ccp((int)rect.getMinX(), rect.getMaxY()), ccp((int)rect.getMaxX(), rect.getMaxY()) };

	CCRect rect2 = m_Sprite ? m_Sprite->boundingBox() : this->boundingBox();
	rect2.origin.y -= 1;//因为马里奥先碰撞，然后做了微调，再到这里来，所以要-1，保证能碰到砖块
	rect2.size.width -= 1;//2、比如砖块大小是32，刚好到32时，马里奥向上移动时检测不出碰撞了，而这里可以，所以要微调处理，（两处细节优化1、2）
	for (int i = 0; i < 3; i++)
	{
		if (obj->getVV1() >= 0 && rect2.containsPoint(p[i]))
		{		
			//CCLog("MonsterFriend %g,%g", p[i].x, rect2.origin.x);
			return true;
		}
	}
	return false;
}
//友方怪物的碰撞
bool MonsterFriend::isCollision(Biology *obj)
{	
	if (m_Sprite)
	{
		return (m_Sprite->boundingBox().intersectsRect(obj->boundingBox()));
	}
	return (this->boundingBox().intersectsRect(obj->boundingBox()));
}
//重写removeSelf
void MonsterFriend::removeSelf()
{ 
	if (m_Life)	
		m_Life = 0;	

	if (m_IsMove)
		m_IsMove = false;

	if (m_Sprite)	
		m_Sprite->removeFromParent();	

	if (this->getParent())	
		this->removeFromParent();	
	
	m_MonsterFriends->removeObject(this); 
}

void MonsterFriend::logic(float dt, Biology *obj)
{
	if (!m_IsRun && Common::isOnScreen(getMap(), m_Sprite ? m_Sprite : this))
	{		
		updateStatus();//更新状态，怪物才开始动起来，好处在于不是一初始化就要runAction，优化了逻辑吧
		m_IsRun = true;
	}

	if (m_Life && m_IsMove)
	{
		move(dt);
	}
	logic(obj);
}

/************************************************************************/
/* 金币砖块对象                                                          */
/************************************************************************/
Brick* Brick::create(Biology *obj, CCSprite *spr, bool isNoKill)
{
	Brick *pRet = new Brick;
	if (pRet && pRet->init(obj, spr, isNoKill))
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

bool Brick::init(Biology *obj, CCSprite *spr, bool isNoKill)
{
	if (!MonsterFriend::init(obj, spr)){ return false; }

	m_IsNoKill = isNoKill;
	return true;
}

void Brick::autoDead(Biology*, int)
{
	if (!m_IsMake)
	{
		addAward(this->getPosition(), true, false, 0);
	}
	m_Life = 0;
	if (!m_Sprite->isVisible())
		m_Sprite->setVisible(true);

	m_Sprite->stopAllActions();
	//先向上移动再向下移动，看起来抖动一下
	CCMoveBy *by = CCMoveBy::create(.05f, ccp(0, 10));
	CCMoveBy *by2 = (CCMoveBy*)by->reverse();
	CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(Brick::tiledChanged));
	CCSequence *seq = CCSequence::create(by, by2, done, NULL);
	m_Sprite->runAction(seq);	
}

//砖块被顶了以后，产生金币或蘑菇/花
void Brick::tiledChanged()
{
	if (!m_IsMake)
	{
		CCSprite *makeCoinSpr = CCSprite::create();
		makeCoinSpr->setZOrder(Common::Z_FOOD);
		makeCoinSpr->setScale(0.8f);
		makeCoinSpr->setPosition(ccp(m_Sprite->getPositionX() + m_Sprite->getContentSize().width / 2,
			m_Sprite->getPositionY() + m_Sprite->getContentSize().height / 2));
		makeCoinSpr->setDisplayFrameWithAnimationName("BrickMakeCoin1", 0);
		CCMoveBy *by = CCMoveBy::create(0.1f, ccp(0, 100));
		CCAnimate *animate1 = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("BrickMakeCoin1"));

		CCSprite *coinEffectSpr = CCSprite::create();
		coinEffectSpr->setZOrder(Common::Z_FOOD);
		coinEffectSpr->setPosition(ccp(makeCoinSpr->getPositionX(), makeCoinSpr->getPositionY() + 100));			
		coinEffectSpr->setDisplayFrameWithAnimationName("BrickMakeCoin2", 0);
		CCAnimate *animate2 = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("BrickMakeCoin2"));
		CCTargetedAction *target = CCTargetedAction::create(coinEffectSpr, animate2);
		CCCallFuncND *done = CCCallFuncND::create(this, callfuncND_selector(Brick::removeNode), (void*)coinEffectSpr);
		CCSequence *seq = CCSequence::create(by, animate1, target, done, NULL);
		makeCoinSpr->runAction(seq);
		m_Sprite->getParent()->getParent()->addChild(coinEffectSpr);
		m_Sprite->getParent()->getParent()->addChild(makeCoinSpr);//金币效果添加到地图上	

		if (Common::isPlayEffect)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(effectNames[1], false);
		}
	}
	else
	{
		//出现蘑菇
		FoodMushroom *food = FoodMushroom::create(m_Biology);
		food->setPosition(ccp(m_Sprite->getPositionX() + m_Sprite->getContentSize().width / 2,
												m_Sprite->getPositionY() + m_Sprite->getContentSize().height / 2));
		//地图添加新的食物对象
		m_Sprite->getParent()->getParent()->addChild(food);
		m_MonsterFriends->addObject(food);

		if (Common::isPlayEffect)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(effectNames[2],false);
		}
	}

	CCTMXLayer *objectsLayer = (CCTMXLayer *)m_Sprite->getParent();
	CCTMXTiledMap *tiledMap = (CCTMXTiledMap *)objectsLayer->getParent();
	objectsLayer->setTileGID(17, Common::pointToTile(tiledMap, m_Sprite->getPosition()));
}

void Brick::removeNode(CCNode *node, void *data)
{
	((CCNode*)data)->removeFromParent();
	node->removeFromParent();
}

//重写碰撞的方法，方块只有被底部碰撞了才会有效果
void Brick::logic(Biology *obj)
{
	if (m_Life && isCollisionBottom(obj))
	{		
		Monster::autoDead(obj);
	}

	//每次都判断是否可以移除自己
	if (!m_IsNoKill && Common::isOutScreen(getMap(), m_Sprite ? m_Sprite : this, obj->getPositionX()))
	{
		removeSelf();
	}	
}

void Brick::updateStatus()
{
	m_Sprite->stopAllActions();

	CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("BrickCoin"));
	m_Sprite->runAction(CCRepeatForever::create(animate));
}

/************************************************************************/
/* 岩石砖块对象                                                          */
/************************************************************************/
Rock* Rock::create(Biology *obj, CCSprite *spr)
{
	Rock *pRet = new Rock;
	if (pRet && pRet->init(obj, spr))
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

bool Rock::init(Biology *obj, CCSprite *spr)
{
	if (!MonsterFriend::init(obj, spr)){ return false; }
	
	this->initWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("BrickRock"));	
	this->setPosition(ccp(m_Sprite->getPositionX() + m_Sprite->getContentSize().width / 2,
									m_Sprite->getPositionY() + m_Sprite->getContentSize().height / 2));
	return true;
}

void Rock::autoDead(Biology *obj, int)
{
	//防止被连撞
	m_Life = 0;

	//如果是可以连续碰撞，处理如下
	if (m_IsMake)
	{
		if (m_GrowState)
		{
			CCMoveBy *by1 = CCMoveBy::create(.05f, ccp(0, 10));
			CCMoveBy *by2 = (CCMoveBy*)by1->reverse();
			CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(Rock::tiledChanged3));
			m_Sprite->runAction(CCSequence::create(by1, by2, done, NULL));
			m_GrowState--;
			if (m_GrowState == 0)
			{
				CCTMXLayer *objectsLayer = (CCTMXLayer *)m_Sprite->getParent();
				CCTMXTiledMap *tiledMap = (CCTMXTiledMap *)objectsLayer->getParent();
				objectsLayer->setTileGID(17, Common::pointToTile(tiledMap, m_Sprite->getPosition()));
			}
		}
		return;
	}
	
	//如果马里奥没有长大，只是上下移动一下		
	if (!obj->getGrowState())
	{
		CCMoveBy *by1 = CCMoveBy::create(.05f, ccp(0, 10));
		CCMoveBy *by2 = (CCMoveBy*)by1->reverse();
		CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(Rock::tiledChanged1));
		m_Sprite->runAction(CCSequence::create(by1, by2, done, NULL));
		return;
	}		

	CCMoveBy *by1 = CCMoveBy::create(.05f, ccp(0, 20));	
	CCTargetedAction *target = CCTargetedAction::create(m_Sprite, by1);	
	CCCallFunc *done1 = CCCallFunc::create(this, callfunc_selector(Rock::tiledChanged2));
	//CCMoveBy *by4 = (CCMoveBy*)by3->reverse();
	CCMoveBy *by2 = CCMoveBy::create(0.3f, ccp(obj->getHV1() * 0.3f, 100));
	CCMoveBy *by3 = CCMoveBy::create(1.5f, ccp(obj->getHV1() * 1.5f, -winSize.width));
	CCCallFunc *done2 = CCCallFunc::create(this, callfunc_selector(Rock::removeSelf));	
	CCSequence *seq = CCSequence::create(target, done1, by2, by3, done2, NULL);
	this->runAction(seq);	
	
	CCRotateBy *rotate = CCRotateBy::create(.5f, 360);
	this->runAction(CCRepeatForever::create(rotate));	

	m_Sprite->getParent()->getParent()->addChild(this);//添加到地图上

	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[7], false);
	}
}

void Rock::tiledChanged1()
{
	m_Life = 1;
}

void Rock::tiledChanged2()
{
	this->setPositionY(this->getPositionY() + 20);
	m_Sprite->removeFromParent();
	m_Sprite = NULL;
}

void Rock::tiledChanged3()
{
	if (m_GrowState)
	{
		m_Life = 1;
	}

	addAward(this->getPosition(), true, false, 100);

	CCSprite *makeCoinSpr = CCSprite::create();
	makeCoinSpr->setZOrder(Common::Z_FOOD);
	makeCoinSpr->setScale(0.8f);
	makeCoinSpr->setPosition(ccp(m_Sprite->getPositionX() + m_Sprite->getContentSize().width / 2,
		m_Sprite->getPositionY() + m_Sprite->getContentSize().height / 2));
	makeCoinSpr->setDisplayFrameWithAnimationName("BrickMakeCoin1", 0);
	CCMoveBy *by = CCMoveBy::create(0.1f, ccp(0, 100));
	CCAnimate *animate1 = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("BrickMakeCoin1"));

	CCSprite *coinEffectSpr = CCSprite::create();
	coinEffectSpr->setZOrder(Common::Z_FOOD);
	coinEffectSpr->setPosition(ccp(makeCoinSpr->getPositionX(), makeCoinSpr->getPositionY() + 100));
	coinEffectSpr->setDisplayFrameWithAnimationName("BrickMakeCoin2", 0);
	CCAnimate *animate2 = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("BrickMakeCoin2"));
	CCTargetedAction *target = CCTargetedAction::create(coinEffectSpr, animate2);
	CCCallFuncND *done = CCCallFuncND::create(this, callfuncND_selector(Rock::removeNode), (void*)coinEffectSpr);
	CCSequence *seq = CCSequence::create(by, animate1, target, done, NULL);
	makeCoinSpr->runAction(seq);
	m_Sprite->getParent()->getParent()->addChild(coinEffectSpr);
	m_Sprite->getParent()->getParent()->addChild(makeCoinSpr);//金币效果添加到地图上	

	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[1], false);
	}
}

void Rock::removeNode(CCNode *node, void *data)
{
	((CCNode*)data)->removeFromParent();
	node->removeFromParent();
}

//重写碰撞的方法，方块只有被底部碰撞了才会有效果
void Rock::logic(Biology *obj)
{
	if (m_Life && isCollisionBottom(obj))
	{		
		Monster::autoDead(obj);
	}

	//每次都判断是否可以移除自己
	if (m_Life && Common::isOutScreen(getMap(), m_Sprite ? m_Sprite : this, obj->getPositionX()))
	{
		removeSelf();
	}
}

//重写removeSelf主要是因为砖块生命为0的时候，还可以用，主要是金币砖块和岩石砖块（比较特殊）
/*void Rock::removeSelf()
{
	//就算没有生命了还是不移除，等待马里奥超过一定位置后再进行移除
	if (m_Biology->getPositionX() > m_Sprite->getPositionX() + winSize.width)
	{
		MonsterFriend::removeSelf();
	}
}*/

/************************************************************************/
/* 纯金币对象                                                            */
/************************************************************************/
Prop* Prop::create(Biology *obj, CCSprite *spr)
{
	Prop *pRet = new Prop;
	if (pRet && pRet->init(obj, spr))
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

bool Prop::init(Biology *obj, CCSprite *spr)
{
	if (!MonsterFriend::init(obj, spr)){ return false; }	
	
	return true;
}

void Prop::updateStatus()
{	
	m_Sprite->stopAllActions();

	CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("PropCoin"));
	m_Sprite->runAction(CCRepeatForever::create(animate));
}

void Prop::autoDead(Biology*, int)
{
	addAward(this->getPosition(), true, false, 0);

	m_Life = 0;

	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[1], false);
	}
}

void Prop::logic(Biology *obj)
{
	if (isCollision(obj))
	{
		Monster::autoDead(obj);
	}

	//每次都判断是否可以移除自己
	if (m_Life <= 0 || Common::isOutScreen(getMap(), m_Sprite, obj->getPositionX()))
	{
		removeSelf();
	}
}

/************************************************************************/
/* 食物                                                                 */
/************************************************************************/
FoodMushroom* FoodMushroom::create(Biology *obj)
{
	FoodMushroom *pRet = new FoodMushroom;
	if (pRet && pRet->init(obj))
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


bool FoodMushroom::init(Biology *obj)
{
	if (!MonsterFriend::init(obj, NULL)){ return false; }

	if (m_Biology->getGrowState())
	{
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("FoodFlower"));
		m_GrowState = 1;
	}
	else
	{
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("FoodMushroom"));
		m_GrowState = 0;		
	}

	CCMoveBy *by = CCMoveBy::create(1.0f, ccp(0, 35));
	CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(FoodMushroom::startMove));
	CCSequence *seq = CCSequence::create(by, done, NULL);
	this->runAction(seq);

	m_IsMove = false;	
	return true;
}

void FoodMushroom::startMove()
{
	if (m_GrowState)
		return;

	m_H_Max_v1 = BIOLOGY_H_V;
	m_H_v1 = -m_H_Max_v1;
	m_IsMove = true;	
}

void FoodMushroom::autoDead(Biology *obj, int num)
{
	addAward(this->getPosition(), false, false, 500);

	m_Life = 0;
	//CCLog("%d", obj->getGrowState());
	obj->setGrowState(obj->getGrowState() + 1);	

	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[3], false);
	}
}

void FoodMushroom::logic(Biology *obj)
{	
	if (isCollision(obj))
	{
		//m_Life = 0;
		//autoDead(obj, 0);
		Monster::autoDead(obj);
	}
	else if (isCollisionBrick())
	{		
		CCRect r1 = obj->boundingBox();
		CCRect r2 = this->boundingBox();
		if (r1.getMinX() < r2.getMinX())
		{
			m_H_v1 = m_H_Max_v1;
		}
		else if (r1.getMaxX() > r2.getMaxX())
		{
			m_H_v1 = -m_H_Max_v1;
		}
		jump(200, false);
	}		
	
	//判断是否可以移除
	if (m_Life <= 0 || Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}

/************************************************************************/
/* 水虎鱼 类似于食人花                                                    */
/************************************************************************/
Piranha* Piranha::create(Biology *obj)
{
	Piranha *pRet = new Piranha;
	if (pRet && pRet->init(obj))
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

bool Piranha::init(Biology *obj)
{
	if (!MonsterFriend::init(obj, NULL)){ return false; }

	this->setAnchorPoint(ccp(0, 0));
	this->setZOrder(Common::Z_GROUND);
	this->setDisplayFrameWithAnimationName("Piranha", 0);

	m_Max_V_S = 0;
	m_Min_V_S = 0;

	m_V_a = 0;
	m_V_Max_v1 = 80;
	m_V_v1 = m_V_Max_v1;

	m_H_a = 0;
	m_H_Max_v1 = 0;
	m_H_v1 = 0;

	m_Wait = 1.0f;

	return true;
}

void Piranha::updateStatus()
{
	if (!m_IsRun)//说明是第一次调用，让水虎鱼动起来
	{
		m_IsMove = true;

		CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("Piranha"));
		this->runAction(CCRepeatForever::create(animate));
	}
}

void Piranha::logic(Biology *obj)
{
	if (m_Life)
	{
		if (isCollision(obj))//碰到别人（马里奥）
		{
			obj->loseLife();
		}
	}

	//只要超出设定范围外，自动死亡
	if (Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}
//水平方向不用移动
void Piranha::moveH(float dt)
{}

void Piranha::moveV(float dt)
{
	if (m_Wait > 0)
	{
		m_Wait -= dt;
		return;
	}

	float s = m_V_v1 *dt;
	this->setPositionY(this->getPositionY() + s);

	if (this->getPositionY() > m_Max_V_S)
	{
		m_V_v1 = -m_V_Max_v1;
		m_Wait = 1.0f;
	}
	else if (this->getPositionY() < m_Min_V_S)
	{
		m_Wait = 2.0f;
		m_V_v1 = m_V_Max_v1;
		this->setPositionY(m_Min_V_S);
	}
}

/************************************************************************/
/* 食人花                                                                */
/************************************************************************/
SFlower* SFlower::create(Biology *obj)
{
	SFlower *pRet = new SFlower;
	if (pRet && pRet->init(obj))
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
bool SFlower::init(Biology *obj)
{
	if (!MonsterFriend::init(obj, NULL)){ return false; }
	
	this->setAnchorPoint(ccp(0, 0));
	this->setDisplayFrameWithAnimationName("SFlower", 0);
	return true;
}

void SFlower::updateStatus()
{
	if (!m_IsRun)//说明是第一次调用，让食人花动起来
	{
		CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("SFlower"));
		this->runAction(CCRepeatForever::create(animate));
	}
}

void SFlower::logic(Biology *obj)
{
	if (m_Life)
	{
		if (isCollision(obj))//碰到别人（马里奥）
		{
			obj->loseLife();
		}
	}

	//只要超出设定范围外，自动死亡
	if (Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}

/************************************************************************/
/* 火弹                                                                 */
/************************************************************************/
AnBiao* AnBiao::create(Biology *obj)
{
	AnBiao *pRet = new AnBiao;
	if (pRet && pRet->init(obj))
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

bool AnBiao::init(Biology *obj)
{
	if (!MonsterFriend::init(obj, NULL)){ return false; }

	this->setDisplayFrameWithAnimationName("AnBiao", 0);

	m_V_a = 0;
	m_V_Max_v1 = 200;
	m_V_v1 = m_V_Max_v1;

	m_H_a = 0;
	m_H_Max_v1 = 0;
	m_H_v1 = 0;

	m_Max_V_s = 0;
	m_Min_V_s = 0;

	return true;
}

void AnBiao::updateStatus()
{
	if (!m_IsRun)
	{
		m_IsMove = true;

		CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("AnBiao"));
		this->runAction(CCRepeatForever::create(animate));
	}

	if (m_V_v1 > 0 && this->isFlipY())
	{
		this->setFlipY(false);
	}
	else if (m_V_v1 < 0 && !this->isFlipY())
	{
		this->setFlipY(true);
	}
}

void AnBiao::logic(Biology *obj)
{
	if (m_Life)
	{
		if (isCollision(obj))//碰到别人（马里奥）
		{
			obj->loseLife();
		}
	}

	//只要超出设定范围外，自动死亡
	if (Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}

void AnBiao::moveV(float dt)
{
	if (m_Max_V_s == 0 && m_Min_V_s == 0)	
		return;
	
	this->setPositionY(this->getPositionY() + dt * m_V_v1);
	if (this->getPositionY()>m_Max_V_s)
	{
		m_V_v1 = -m_V_Max_v1;
		updateStatus();
	}else if (this->getPositionY() < m_Min_V_s)
	{
		m_V_v1 = m_V_Max_v1;
		updateStatus();
	}
}

/************************************************************************/
/* 圆盘                                                                 */
/************************************************************************/
Disk* Disk::create(Biology* obj, CCPoint center, float radius, bool isnokill)
{
	Disk *pRet = new Disk;
	if (pRet && pRet->init(obj, center, radius, isnokill))
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

bool Disk::init(Biology *obj, CCPoint center, float radius, bool isnokill)
{
	if (!MonsterFriend::init(obj, NULL)){ return false; }

	m_IsNoKill = isnokill;
	//m_MoveTimes = 0;

	m_Radius = radius;
	m_Center = center;

	m_Radian = 0;
	return true;
}

void Disk::updateStatus()
{
	if (!m_IsRun)
	{
		m_IsMove = true;
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("Disk"));
		CCRotateBy *by = CCRotateBy::create(1.0f, 360);	
		this->runAction(CCRepeatForever::create(by));		
	}
}

void Disk::moveH(float dt)
{
	//做圆形运动+本身旋转
	//float radian = (dt*M_PI * 2 / 3)* m_MoveTimes;
	m_Radian += (dt*M_PI * 2 / 3);

	//计算x,y
	//float fx = m_Radius * sin(radian); 
	//float fy = m_Radius * cos(radian);
	float fx = m_Radius * sinf(m_Radian);
	float fy = m_Radius * cosf(m_Radian);
	//CCLog("%g", m_Radian);
	this->setPosition(ccp(m_Center.x + fx, m_Center.y + fy));

	//m_Angle += 30;
	//CCLog("%g", this->getRotation());
	//this->setRotation(this->getRotation() + 10);
}

void Disk::logic(Biology *obj)
{
	//碰撞检测判断
	if (m_Life)
	{
		if (isCollision(obj))
		{
			obj->loseLife();//马里奥减生命			
		}
	}

	//只要超出设定范围外，自动死亡
	if (!m_IsNoKill && Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}

/************************************************************************/
/* 游戏通过该关卡对象                                                     */
/************************************************************************/
EndBlack* EndBlack::create(Biology *obj)
{
	EndBlack *pRet = new EndBlack;
	if (pRet && pRet->init(obj))
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

bool EndBlack::init(Biology *obj)
{
	if (!MonsterFriend::init(obj, NULL)){ return false; }
	
	initWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("End"));	
	
	setAnchorPoint(ccp(0.5f, 0));	
	m_StartPoint = CCPointZero;
	return true;
}

void EndBlack::updateStatus()
{
	if (!m_IsRun)
	{
		m_StartPoint = this->getPosition();
		CCMoveBy *by1 = CCMoveBy::create(2.0f, ccp(0, -32 * 5));
		CCMoveBy *by2 = (CCMoveBy*)by1->reverse();
		CCSequence *seq = CCSequence::create(by1, by2, NULL);
		this->runAction(CCRepeatForever::create(seq));
	}	
}

//被马里奥碰到以后执行的操作
void EndBlack::autoDead(Biology *obj, int)
{
	float dy = m_StartPoint.y - this->getPositionY();
	//if (dy < 0)dy = 0;
	addAward(this->getPosition(), false, false, 500 - dy * 2);

	m_Life = 0;
	this->stopAllActions();
	CCMoveBy *by = CCMoveBy::create(3.0f, ccp(obj->getHV1(), -winSize.width));
	CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(MonsterFriend::removeSelf));
	CCSequence *seq = CCSequence::create(by, done, NULL);
	this->runAction(seq);
}

//不能删除该方法，因为没有调用removeSelf
void EndBlack::logic(Biology *obj)
{
	if (m_Life && isCollision(obj))
	{		
		Monster::autoDead(obj);
	}	
}

/************************************************************************/
/* 点击蘑菇后生成一个食物（蘑菇）                                       */
/************************************************************************/
DarkFoodMushroom* DarkFoodMushroom::create(Biology *obj)
{
	DarkFoodMushroom *pRet = new DarkFoodMushroom;
	if (pRet && pRet->init(obj))
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

bool DarkFoodMushroom::init(Biology *obj)
{
	if (!MonsterFriend::init(obj, NULL)){ return false; }

	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("DarkFoodMushroom"));
	m_IsMove = true;

	m_H_Max_v1 = 170;
	m_H_v1 = m_H_Max_v1;
	m_H_a = 0;

	m_V_Max_v1 = 130;
	m_V_v1 = m_V_Max_v1;
	m_V_a = 0;

	CCRotateBy *by = CCRotateBy::create(0.5f, 90 + CCRANDOM_0_1() * 90);
	this->runAction(by);
	return true;
}

void DarkFoodMushroom::autoDead(Biology *obj, int)
{
	m_Life = 0;
	obj->setGrowState(obj->getGrowState() + 1);

	removeSelf();
}

void DarkFoodMushroom::logic(Biology *obj)
{
	if (m_Life && isCollision(obj))
	{
		Monster::autoDead(obj);
	}
}

void DarkFoodMushroom::moveH(float dt)
{
	CCRect rect = m_Biology->boundingBox();

	float fx = this->getPositionX();
	if (fx < rect.origin.x + rect.size.width / 2)
	{	
		if (fx < rect.origin.x + rect.size.width / 2 - 4)
			this->setPositionX(fx + dt*m_H_v1);		
	}
	else
	{		
		if (fx > rect.origin.x + rect.size.width / 2 + 4)
			this->setPositionX(fx - dt*m_H_v1);		
	}
}

void DarkFoodMushroom::moveV(float dt)
{
	CCRect rect = m_Biology->boundingBox();
	float fy = this->getPositionY();
	if (fy < rect.origin.y + rect.size.height / 2)
	{
		this->setPositionY(fy + dt*m_V_v1);
	}
	else
	{
		this->setPositionY(fy - dt*m_V_v1);
	}
}
