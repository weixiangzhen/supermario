#include "MonsterEnemy.h"
#include "MonsterBullet.h"

CCArray *MonsterEnemy::m_MonsterEnemys = NULL;

MonsterEnemy* MonsterEnemy::create(Biology *obj)
{
	MonsterEnemy *pRet = new MonsterEnemy;
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

bool MonsterEnemy::init(Biology *obj)
{
	if (!Monster::init(obj)){ return false; }

	//this->setAnchorPoint(ccp(0.5f, 0));
	this->setZOrder(Common::Z_MONSTER);
	m_IsRun = false;//默认没有运行

	//设置速度和方向
	//默认是向左行走，可以放在logic(float, Biology*)这里来，感觉会合理一点
	m_H_Max_v1 = BIOLOGY_H_V / 2;//给怪物设置水平方向的速度
	m_H_v1 = -m_H_Max_v1;	

	//行走的域默认是0
	m_LeftScope = 0;
	m_RightScope = 0;

	m_UpScope = 0;
	m_DownScope = 0;	
	return true;
}
//重写水平方向的移动方法
void MonsterEnemy::moveH(float dt)
{
	if (m_H_v1 < 0)
	{
		calcX(dt);
		if (_isCollisionLeft() || (m_LeftScope != 0 && this->getPositionX() <= m_LeftScope))
		{
			m_H_v1 = m_H_Max_v1;//碰到墙后速度反过来
			updateStatus();//更新一下状态
		}
	}
	else if (m_H_v1 > 0)
	{
		calcX(dt);
		if (_isCollisionRight() || (m_RightScope != 0 && this->getPositionX() >= m_RightScope))
		{
			m_H_v1 = -m_H_Max_v1;//碰到墙后速度反过来
			updateStatus();
		}
	}
	else//不会动
	{

	}
}
//重写垂直方向的移动方法
void MonsterEnemy::moveV(float dt)
{
	if (m_V_v1 <= 0)//下落
	{
		if (m_IsFly)
			calcY(dt);//先下落，在碰撞检测，方便做微调，不然水平移动会出现问题		
		if (_isCollisionDown() || (m_DownScope != 0 && this->getPositionY() <= m_DownScope))
		{
			if (m_IsFly)
			{
				m_IsFly = false;
				m_V_v1 = 0;//每一次停止下落时速度都清0			
				updateStatus();
			}
		}
		else
		{
			if (!m_IsFly)
			{
				m_IsFly = true;
				//m_V_v1 = 0;			
				updateStatus();
			}
		}
	}
	else//上升
	{
		calcY(dt);
		if (_isCollisionUp() || (m_UpScope != 0 && this->getPositionY() >= m_UpScope))
		{
			m_V_v1 = 0;
			m_V_a = -BIOLOGY_V_A;
		}
	}
}

//判断是否被别人碰撞到头了
bool MonsterEnemy::isCollisionHeader(Biology *obj)
{
	if (obj->getLife() <= 0)
		return false;

	if (obj->isGodMode() || obj->getLife() <= 0)//如果马里奥处于无敌状态，马里奥不在判断是否有踩到敌人，避免bug
		return false;

	CCRect rect = obj->boundingBox();
	CCPoint p[3] = { ccp(rect.getMidX(), rect.getMinY()), ccp(rect.getMinX(), rect.getMinY()), ccp(rect.getMaxX(), rect.getMinY()) };

	CCRect rect2 = this->boundingBox();

	for (int i = 0; i < 3; i++)
	{
		if ((obj->getVV1() >= 0 && rect2.containsPoint(p[i]) && rect2.getMaxY() - rect.getMinY() < this->getContentSize().height / 5) ||
					(obj->getVV1() < 0 && rect2.containsPoint(p[i]) && rect2.getMaxY() - rect.getMinY() < this->getContentSize().height / 2))
		{
			//对马里奥进行微调，防止连续碰撞
			obj->setPositionY(rect2.origin.y + rect2.size.height + 2);
			//踩到怪物了，让马里奥升高一点(obj)
			obj->jump(300, true);
			return true;
		}
	}
	return false;
}
//重写removeSelf
void MonsterEnemy::removeSelf()
{ 	
	if (m_Life)
		m_Life = 0;

	if (this->getParent())
	{
		this->removeFromParent();
	}
	
	m_MonsterEnemys->removeObject(this); 
}

//碰撞检测
void MonsterEnemy::logic(Biology *obj)
{
	if (m_Life)
	{
		if(isCollisionHeader(obj))//被踩到了，生命为0
		{			
			autoDead(obj, 0);
		}
		else if (isCollision(obj))//没有被踩到，但是碰到别人了（马里奥）
		{
			obj->loseLife();
		}
		else if (isCollisionBrick())
		{
			autoDead(obj, 1);
		}
	}		

	//每次都判断是否要移除自己
	//this的锚点在中心点，小于0不报错是因为小于地图一个砖块的宽度（32），假如-5/32=0；所以不会报错，大于砖块的宽度的话就会报错了		
	if (Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}
//碰撞检测+移动
void MonsterEnemy::logic(float dt, Biology *obj)
{	
	/*---------------------------以下这段代码不能放在logic(Biology*)这里，因为其它子类会覆写logic(Biology*)这方法----------------------*/
	/*---------------------------提示：一般情况不要覆写logic(float dt, Biology *obj)该方法--------------------------------------------*/
	//判断是否在屏幕内，是的话开始移动	
	if (!m_IsRun && Common::isOnScreen(getMap(), this))
	{		
		updateStatus();//更新状态，怪物才开始动起来，好处在于不是一初始化就要runAction，优化了逻辑吧
		m_IsRun = true;
	}
	/*------------------------------------------------------------------------------------------*/
	if (m_Life && m_IsMove)
	{
		move(dt);
	}
	logic(obj);
}

/************************************************************************/
/* 蘑菇怪对象                                                            */
/************************************************************************/
MonsterSmall* MonsterSmall::create(Biology *obj, bool isDiscolor)
{
	MonsterSmall *pRet = new MonsterSmall;
	if (pRet && pRet->init(obj, isDiscolor))
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

bool MonsterSmall::init(Biology *obj, bool isDiscolor)
{
	if (!MonsterEnemy::init(obj)){ return false; }

	m_IsDiscolor = isDiscolor;
	if (m_IsDiscolor)
		initWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("MonsterSmallBlack"));
	else
		initWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("MonsterSmall"));
	
	return true;
}
//死亡方法：有两种死法，一种被踩死的，另一种被子弹射死的或者被砖块碰死的
void MonsterSmall::autoDead(Biology *obj, int num)
{
	addAward(this->getPosition(), false, true, 100);

	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[8], false);
	}

	m_Life = 0;
	//被砖块顶到		
	if (num)
	{
		CCMoveBy *by = CCMoveBy::create(0.2f, ccp(obj->getHV1() * 0.1f, 20));
		CCMoveBy *by2 = CCMoveBy::create(2.0f, ccp(obj->getHV1(), -winSize.width));
		CCRotateBy *rotate = CCRotateBy::create(.8f, 360);
		CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(MonsterEnemy::removeSelf));
		CCSequence *seq = CCSequence::create(by, by2, done, NULL);
		this->runAction(seq);
		this->runAction(CCRepeatForever::create(rotate));		
		return;
	}
	//被踩到后
	if (m_IsDiscolor)
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("MonsterSmallBlackDead"));
	else
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("MonsterSmallDead"));
	CCFadeOut *fadeout = CCFadeOut::create(3.0f);
	CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(MonsterEnemy::removeSelf));//完成后要移除自己
	CCSequence *seq = CCSequence::create(fadeout, done, NULL);                
	this->runAction(seq);
}

void MonsterSmall::updateStatus()
{	
	if (!m_IsRun)
	{		
		this->schedule(schedule_selector(MonsterSmall::walk), 0.2f);
		m_IsMove = true;
	}
}

void MonsterSmall::walk(float)
{
	if (this->isFlipX())
		this->setFlipX(false);
	else
		this->setFlipX(true);
}

/*void MonsterSmall::logic(Biology *obj)
{
	MonsterEnemy::logic(obj);
}*/

/************************************************************************/
/* 乌龟对象                                                              */
/************************************************************************/
Tortoise* Tortoise::create(Biology *obj, int state)
{
	Tortoise *pRet = new Tortoise;
	if (pRet && pRet->init(obj, state))
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

bool Tortoise::init(Biology *obj, int state)
{
	if (!MonsterEnemy::init(obj)){ return false; }
	//设置锚点
	this->setAnchorPoint(ccp(0.5f, 0));
	
	//乌龟有4条状态,3飞天龟、2正常行走龟、1龟壳、0疯狂的移动	
	m_MoveState = state;
	if (m_MoveState >= 6)
	{		
		m_GrowState = 3;
	}
	else
	{			
		m_GrowState = 2;
	}	
	
	m_IsReviving = false;//默认不用复活	
	return true;
}
//变成龟壳后复活
void Tortoise::revive(float)
{
	m_GrowState = 2;
	m_IsReviving = false;
	updateStatus();
	//CCLog("revive");
}
//跟其他怪物碰撞
bool Tortoise::isCollisionAll(CCArray *arrays)
{
	if (m_GrowState)
		return false;
	CCObject *obj = NULL;
	CCARRAY_FOREACH_REVERSE(arrays, obj)
	{
		Monster *monster = (Monster*)obj;
		if (!monster->getLife() || monster == this)
			continue;

		if (isCollision(monster))
		{				
			monster->autoDead(this, 1);
			if (m_H_v1 < 0)
			{				
				m_H_v1 = m_H_Max_v1;
			}
			else
			{				
				m_H_v1 = -m_H_Max_v1;
			}
			return true;
		}
	}
	return false;
}

//覆写水平方向移动的代码
void Tortoise::moveH(float dt)
{
	if (m_MoveState == 9)//不会水平移动
		return;

	MonsterEnemy::moveH(dt);
}

//覆写垂直方向移动的代码，乌龟比较特殊
void Tortoise::moveV(float dt)
{
	if (m_GrowState == 3)
	{
		if (m_UpScope != 0 || m_DownScope != 0)
		{
			jump(420, 710, false);
		}
		else
		{
			jump();
		}		
	}		
	MonsterEnemy::moveV(dt);
}

void Tortoise::updateStatus()
{
	if (!m_IsRun)//说明是第一次调用，让乌龟动起来	
		m_IsMove = true;	
	
	this->stopAllActions();	
	switch (m_GrowState)
	{
	case 3:
	{
		this->setDisplayFrameWithAnimationName("TortoiseFlyWalk", 0);
		CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("TortoiseFlyWalk"));
		this->runAction(CCRepeatForever::create(animate));

		if (m_H_v1 <= 0)
		{
			this->setFlipX(true);//方向向左
			m_H_Max_v1 = TORTOISE_H_V_FLY;
			m_H_v1 = -m_H_Max_v1;
		}
		else
		{
			this->setFlipX(false);//方向向右		
			m_H_Max_v1 = TORTOISE_H_V_FLY;
			m_H_v1 = m_H_Max_v1;
		}
	}
		break;
	case 2:
	{
		this->setDisplayFrameWithAnimationName("TortoiseWalk", 0);
		CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("TortoiseWalk"));
		this->runAction(CCRepeatForever::create(animate));

		if (m_H_v1 <= 0)
		{
			this->setFlipX(true);//方向向左
			m_H_Max_v1 = TORTOISE_H_V;
			m_H_v1 = -m_H_Max_v1;
		}
		else
		{
			this->setFlipX(false);//方向向右		
			m_H_Max_v1 = TORTOISE_H_V;
			m_H_v1 = m_H_Max_v1;
		}
	}
		break;
	case 1:
		//变成龟壳			
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("TortoiseHull"));
		m_H_v1 = 0;
		m_H_Max_v1 = 0;
		if (!m_IsReviving)
		{
			this->scheduleOnce(schedule_selector(Tortoise::revive), 5.0f);
			if (Common::isPlayEffect)
			{
				SimpleAudioEngine::sharedEngine()->playEffect(effectNames[9], false);
			}
		}			
		//CCLog("m_life = %d", m_Life);
		m_IsReviving = true;
		break;
	case 0:
	{
		//疯狂的移动		
		CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("TortoiseCrazy"));
		this->runAction(CCRepeatForever::create(animate));
		//少做一些没必要的判断，因为疯狂的龟壳碰到墙后会更新一次状态，避免重复赋值
		if (m_H_Max_v1 == 0)
		{
			if (m_H_v1 <= 0)
			{
				m_H_Max_v1 = TORTOISE_H_V_CRAZY;
				m_H_v1 = -m_H_Max_v1;
			}
			else
			{
				m_H_Max_v1 = TORTOISE_H_V_CRAZY;
				m_H_v1 = m_H_Max_v1;
			}
		}
		//CCLog("TortoiseCrazy~~~~~~~~~");
		if (m_IsReviving)
		{
			this->unschedule(schedule_selector(Tortoise::revive));
			m_IsReviving = false;
		}
	}
		break;
	default:
		break;
	} 	
}

void Tortoise::autoDead(Biology *obj , int)
{
	addAward(this->getPosition(), false, true, 200);

	m_Life = 0;
	//变成龟壳	
	this->stopAllActions();
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("TortoiseHull"));
	CCMoveBy *by = CCMoveBy::create(0.2f, ccp(obj->getHV1() * 0.1f, 20));
	CCMoveBy *by2 = CCMoveBy::create(2.0f, ccp(obj->getHV1(), -winSize.width));
	CCRotateBy *rotate = CCRotateBy::create(.8f, 360);
	CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(MonsterEnemy::removeSelf));
	CCSequence *seq = CCSequence::create(by, by2, done, NULL);
	this->runAction(seq);
	this->runAction(CCRepeatForever::create(rotate));

	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[9], false);
	}
}

void Tortoise::logic(Biology *obj)
{
	//碰撞检测判断
	if (m_Life)
	{
		if (isCollisionHeader(obj))//被踩到了，成长状态变化
		{		
			if (m_GrowState == 3)//如果是飞天龟，则不再飞
			{
				//m_V_Max_v1 = TORTOISE_V_V;
				m_V_v1 = 0;
				m_V_a = -BIOLOGY_V_A;//重新给定加速度，恢复默认
				m_IsFly = false;

				//上下域失效
				m_UpScope = 0;
				m_DownScope = 0;
			}

			if (m_GrowState == 0)			
				m_GrowState = 1;//在疯狂的移动的时候被踩中，变为龟壳				
			else			
				m_GrowState--;
			//当乌龟变成疯狂移动时，做一些方向上的处理
			if (!m_GrowState)
			{
				CCRect r1 = obj->boundingBox();
				CCRect r2 = this->boundingBox();
				if (r1.getMinX() < r2.getMinX())
				{
					m_H_v1 = 1;//让乌龟向右疯狂移动
				}
				else if (r1.getMaxX() > r2.getMaxX())
				{
					m_H_v1 = -1;//让乌龟向左疯狂移动
				}
			}
			//CCLog("%d", m_GrowState);
			updateStatus();
		}
		else if (isCollision(obj))
		{
			if (m_GrowState == 1)//在是龟壳的时候被马里奥碰到时，变成疯狂的
			{
				//m_DirRun = obj->getFaceDir();
				//CCLog("%f", obj->getHV1());							
				m_H_v1 = obj->getHV1();//把马里奥的速度给乌龟				
				obj->setHV1(0);//把马里奥的速度降为0
				m_GrowState = 0;//
				updateStatus();//更新一次状态，调整速度
			}
			else
			{
				obj->loseLife();//马里奥减生命
			}
		}
		else if (isCollisionBrick())
		{			
			//autoDead(obj, 0);			
			Monster::autoDead(obj);
		}

		isCollisionAll(MonsterEnemy::m_MonsterEnemys);
		
	}
	
	//只要超出了设定的屏幕外，自动死亡
	if (m_Life && Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}

/************************************************************************/
/* 带刺怪                                                                */
/************************************************************************/
Backstab* Backstab::create(Biology *obj)
{
	Backstab *pRet = new Backstab;
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

bool Backstab::init(Biology *obj)
{
	if (!MonsterEnemy::init(obj)){ return false; }

	this->setDisplayFrameWithAnimationName("Backstab", 0);//可以写到updateStatus这里来

	return true;
}

void Backstab::updateStatus()
{
	if (!m_IsRun)//说明是第一次调用，让乌龟动起来
	{
		m_IsMove = true;
	}

	this->stopAllActions();

	CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("Backstab"));
	this->runAction(CCRepeatForever::create(animate));
	if (m_H_v1 <= 0)
	{
		this->setFlipX(true);//方向向左		
	}
	else
	{
		this->setFlipX(false);//方向向右			
	}
}

void Backstab::autoDead(Biology *obj, int)
{
	addAward(this->getPosition(), false, true, 200);

	m_Life = 0;
	//停止所有动作，旋转向下移动死亡
	this->stopAllActions();
	//this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("Backstab"));
	CCMoveBy *by = CCMoveBy::create(0.2f, ccp(obj->getHV1() * 0.1f, 20));
	CCMoveBy *by2 = CCMoveBy::create(2.0f, ccp(obj->getHV1(), -winSize.width));
	CCRotateBy *rotate = CCRotateBy::create(.8f, 360);
	CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(MonsterEnemy::removeSelf));
	CCSequence *seq = CCSequence::create(by, by2, done, NULL);
	this->runAction(seq);
	this->runAction(CCRepeatForever::create(rotate));
}

void Backstab::logic(Biology *obj)
{
	if (m_Life)
	{
		if (isCollision(obj))//没有被踩到，但是碰到别人了（马里奥）
		{
			obj->loseLife();
		}
		else if (isCollisionBrick())
		{
			autoDead(obj, 1);
		}
	}

	//只要超出设定范围外，自动死亡
	if (Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}

/************************************************************************/
/* 楼梯                                                                */
/************************************************************************/
Stair* Stair::create(Biology *obj, int dir)
{
	Stair *pRet = new Stair;
	if (pRet && pRet->init(obj, dir))
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

bool Stair::init(Biology *obj, int dir)
{
	if (!MonsterEnemy::init(obj)){ return false; }

	m_Dir = dir;
	if (m_Dir == -1)
	{
		//只能向下或向左
		m_H_Max_v1 = BIOLOGY_H_V;
		m_H_v1 = -m_V_Max_v1;
		m_H_a = 0;

		m_V_Max_v1 = BIOLOGY_H_V *1.3f;
		m_V_v1 = -m_V_Max_v1;
		m_V_a = 0;
	}
	else if (m_Dir == 1)
	{
		//只能向上或向右
		m_H_Max_v1 = BIOLOGY_H_V;
		m_H_v1 = m_V_Max_v1;
		m_H_a = 0;

		m_V_Max_v1 = BIOLOGY_H_V *1.3f;
		m_V_v1 = m_V_Max_v1;
		m_V_a = 0;
	}	

	CCSprite *spr1 = CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("Stair"));
	spr1->setAnchorPoint(ccp(0, 0));
	CCSprite *spr2 = CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("Stair"));
	spr2->setPositionX(spr1->getContentSize().width);
	spr2->setAnchorPoint(ccp(0, 0));

	this->setAnchorPoint(ccp(0, 0));
	this->addChild(spr1);
	this->addChild(spr2);
	this->setContentSize(CCSizeMake(spr1->getContentSize().width * 2, spr1->getContentSize().height));	

	//m_IsMove = true;//初始化后就让它移动
	m_IsOnLadders = false;
	return true;
}
/* 状态更新 */
void Stair::updateStatus()
{		
	if (!m_IsRun)
	{
		m_IsMove = true;
	}
}

/* 无特殊逻辑 */
void Stair::logic(Biology *obj)
{
	//只要超出设定范围外，自动死亡
	if (obj->getPositionX() > this->getPositionX() + winSize.width * 3 / 2)
	{
		removeSelf();
	}
}
void Stair::calcX(float dt)
{
	float s = m_H_v1 * dt;
	//CCLog("%p,%g", this, m_H_v1);
	// 判断马里奥是不是踩在梯子上
	CCRect rect = this->boundingBox();
	if (m_H_v1 < 0)
		rect.size.height += -s + 1;
	else
		rect.size.height += s + 1;

	if (rect.intersectsRect(m_Biology->boundingBox()))
	{
		m_Biology->setPositionX(m_Biology->getPositionX() + s);
		if (!m_IsOnLadders)
			m_IsOnLadders = true;
	}
	else if (m_IsOnLadders)
		m_IsOnLadders = false;

	this->setPositionX(this->getPositionX() + s);
	if (m_IsOnLadders)
	{
		this->mapMoveH(m_Biology, s);
	}	
}

//左右移动的阶梯
void Stair::moveH(float dt)
{	
	if (m_LeftScope != 0 || m_RightScope != 0)
	{
		calcX(dt);
	}
	if (m_Dir == -1 && m_LeftScope != 0)
	{		
		if (this->getPositionX() <= m_LeftScope)
		{
			m_Dir = 1;
			m_H_v1 = m_H_Max_v1;//移动到固定位置后速度反过来			
		}
	}
	else if (m_Dir == 1 && m_RightScope != 0)
	{		
		if (this->getPositionX() >= m_RightScope)
		{
			m_Dir = -1;
			m_H_v1 = -m_H_Max_v1;//移动到固定位置后速度反过来			
		}
	}	
}
//上下移动的阶梯
void Stair::moveV(float dt)
{	
	if (m_UpScope != 0 || m_DownScope != 0)
	{
		float s = m_V_v1 * dt;

		/* 判断马里奥是不是踩在梯子上 */
		CCRect rect = this->boundingBox();
		if (m_V_v1 < 0)
			rect.size.height += -s + 1;
		else
			rect.size.height += s + 1;

		if (rect.intersectsRect(m_Biology->boundingBox()))
		{
			m_Biology->setPositionY(m_Biology->getPositionY() + s - 1);//说明马里奥在桥上
			if (!m_IsOnLadders)
				m_IsOnLadders = true;
		}
		else if (m_IsOnLadders)
			m_IsOnLadders = false;

		/*---------------------------------------------*/
		this->setPositionY(this->getPositionY() + s);
		if (m_IsOnLadders)
			this->mapMoveV(m_Biology, s);
	}

	if (m_Dir == 1 && m_UpScope != 0)
	{
		if (this->getPositionY() >= m_UpScope)
		{
			this->setPositionY(m_DownScope);			
			//CCLog("up movev %g,%g,%g", m_DownScope, m_UpScope, m_V_v1);
		}
	}		
	else if (m_Dir == -1 && m_DownScope != 0)
	{		
		if (this->getPositionY() <= m_DownScope)
		{
			this->setPositionY(m_UpScope);		
			//CCLog("down movev %g", m_UpScope);
		}
	}	
}

//重写碰撞检测+移动，出现的条件不一样
void Stair::logic(float dt, Biology *obj)
{	
	//判断是否在屏幕内，是的话开始移动	
	if (!m_IsRun && getMap()->convertToWorldSpace(this->getPosition()).x < winSize.width + this->getContentSize().width / 2)
	{
		updateStatus();//更新状态，怪物才开始动起来，好处在于不是一初始化就要runAction，优化了逻辑吧
		m_IsRun = true;
	}
	/*------------------------------------------------------------------------------------------*/
	if (m_Life && m_IsMove)
	{
		move(dt);
	}
	logic(obj);
}

/************************************************************************/
/* 炮弹                                                                 */
/************************************************************************/
Cannon* Cannon::create(Biology *obj)
{
	Cannon *pRet = new Cannon;
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
bool Cannon::init(Biology *obj)
{
	if (!MonsterEnemy::init(obj)){ return false; }

	m_IsDefaultFace = true;
	return true;
}

void Cannon::updateStatus()
{
	if (!m_IsRun)
	{
		schedule(schedule_selector(Cannon::shoot), 3.0f + CCRANDOM_0_1()*0.5f);
	}
}

void Cannon::shoot(float)
{
	CannonBullet *bullet = CannonBullet::create(m_Biology);
	bullet->setPosition(ccp(this->getPositionX() + 8, this->getPositionY() + 8));
	bullet->setRunDir(m_IsDefaultFace);
	getMap()->addChild(bullet);
	MonsterBullet::m_Bullets->addObject(bullet);
	//CCLog("CannonBullet shoot");
}

void Cannon::logic(Biology *obj)
{
	//碰撞检测判断
	if (m_Life)
	{
		if (isCollision(obj))
		{
			obj->loseLife();//马里奥减生命			
		}

		//判断马里奥是不是超过炮弹
		if (obj->getPositionX() > this->getPositionX() && m_IsDefaultFace)
		{
			m_IsDefaultFace = false;			
		}
		else if (obj->getPositionX() <= this->getPositionX() && !m_IsDefaultFace)
		{
			m_IsDefaultFace = true;		
		}
	}

	//只要超出设定范围外，自动死亡
	if (Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}
/************************************************************************/
/* 钢盔龟对象                                                              */
/************************************************************************/
Helmet* Helmet::create(Biology *obj)
{
	Helmet *pRet = new Helmet;
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

bool Helmet::init(Biology *obj)
{
	if (!MonsterEnemy::init(obj)){ return false; }
	//设置锚点
	this->setAnchorPoint(ccp(0.5f, 0));

	//钢盔龟有3条状态,2正常行走龟、1龟壳、0疯狂的移动		
	m_GrowState = 2;	

	m_IsReviving = false;//默认不用复活	
	return true;
}
//变成龟壳后复活
void Helmet::revive(float)
{
	m_GrowState = 2;
	m_IsReviving = false;
	updateStatus();
	//CCLog("revive");
}

void Helmet::updateStatus()
{
	if (!m_IsRun)//说明是第一次调用，让乌龟动起来	
		m_IsMove = true;

	this->stopAllActions();
	switch (m_GrowState)
	{	
	case 2:
	{
			  this->setDisplayFrameWithAnimationName("HelmetWalk", 0);
			  CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("HelmetWalk"));
			  this->runAction(CCRepeatForever::create(animate));

			  if (m_H_v1 <= 0)
			  {
				  this->setFlipX(true);//方向向左
				  m_H_Max_v1 = TORTOISE_H_V;
				  m_H_v1 = -m_H_Max_v1;
			  }
			  else
			  {
				  this->setFlipX(false);//方向向右		
				  m_H_Max_v1 = TORTOISE_H_V;
				  m_H_v1 = m_H_Max_v1;
			  }
	}
		break;
	case 1:
		//变成龟壳			
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("HelmetHull"));
		m_H_v1 = 0;
		m_H_Max_v1 = 0;
		if (!m_IsReviving)
			this->scheduleOnce(schedule_selector(Helmet::revive), 5.0f);
		//CCLog("m_life = %d", m_Life);
		m_IsReviving = true;
		break;
	case 0:
	{
			  //疯狂的移动		
			  CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("HelmetCrazy"));
			  this->runAction(CCRepeatForever::create(animate));
			  //少做一些没必要的判断，因为疯狂的龟壳碰到墙后会更新一次状态，避免重复赋值
			  if (m_H_Max_v1 == 0)
			  {
				  if (m_H_v1 <= 0)
				  {
					  m_H_Max_v1 = TORTOISE_H_V_CRAZY;
					  m_H_v1 = -m_H_Max_v1;
				  }
				  else
				  {
					  m_H_Max_v1 = TORTOISE_H_V_CRAZY;
					  m_H_v1 = m_H_Max_v1;
				  }
			  }
			  //CCLog("TortoiseCrazy~~~~~~~~~");
			  if (m_IsReviving)
			  {
				  this->unschedule(schedule_selector(Helmet::revive));
				  m_IsReviving = false;
			  }
	}
		break;
	default:
		break;
	}
}

void Helmet::autoDead(Biology *obj, int num)
{
	//被子弹碰到不会死
	if (num)	
		return;	

	addAward(this->getPosition(), false, true, 200);

	m_Life = 0;
	//变成龟壳	
	this->stopAllActions();
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("HelmetHull"));
	CCMoveBy *by = CCMoveBy::create(0.2f, ccp(obj->getHV1() * 0.1f, 20));
	CCMoveBy *by2 = CCMoveBy::create(2.0f, ccp(obj->getHV1(), -winSize.width));
	CCRotateBy *rotate = CCRotateBy::create(.8f, 360);
	CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(MonsterEnemy::removeSelf));
	CCSequence *seq = CCSequence::create(by, by2, done, NULL);
	this->runAction(seq);
	this->runAction(CCRepeatForever::create(rotate));
}

void Helmet::logic(Biology *obj)
{
	//碰撞检测判断
	if (m_Life)
	{
		if (isCollisionHeader(obj))//被踩到了，成长状态变化
		{
			if (m_GrowState == 0)
				m_GrowState = 1;//在疯狂的移动的时候被踩中，变为龟壳				
			else
				m_GrowState--;
			//当乌龟变成疯狂移动时，做一些方向上的处理
			if (!m_GrowState)
			{
				CCRect r1 = obj->boundingBox();
				CCRect r2 = this->boundingBox();
				if (r1.getMinX() < r2.getMinX())
				{
					m_H_v1 = 1;//让乌龟向右疯狂移动
				}
				else if (r1.getMaxX() > r2.getMaxX())
				{
					m_H_v1 = -1;//让乌龟向左疯狂移动
				}
			}
			//CCLog("%d", m_GrowState);
			updateStatus();
		}
		else if (isCollision(obj))
		{
			if (m_GrowState == 1)//在是龟壳的时候被马里奥碰到时，变成疯狂的
			{
				//m_DirRun = obj->getFaceDir();
				//CCLog("%f", obj->getHV1());
				m_H_v1 = obj->getHV1();//把马里奥的速度给乌龟				
				obj->setHV1(0);//把马里奥的速度降为0
				m_GrowState = 0;//
				updateStatus();//更新一次状态，调整速度
			}
			else
			{
				obj->loseLife();//马里奥减生命
			}
		}
		else if (isCollisionBrick())
		{
			//autoDead(obj, 0);			
			Monster::autoDead(obj);
		}
	}

	//只要超出了设定的屏幕外，自动死亡
	if (m_Life && Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}
}

/************************************************************************/
/* boss对象                                                             */
/************************************************************************/
Boss* Boss::create(Biology *obj)
{
	Boss *pRet = new Boss;
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

bool Boss::init(Biology *obj)
{
	if (!MonsterEnemy::init(obj)){ return false; }

	this->setDisplayFrameWithAnimationName("BossWalk", 0);
	
	m_Life = 2;
	m_IsRun = false;
	m_JumpCount = CCRANDOM_0_1() * 2 + 2.0f;//跳跃计数
	m_CurJumpCount = 0;

	m_IsDefaultFace = true;//默认脸朝左
	return true;
}

void Boss::updateStatus()
{
	if (!m_IsRun)
	{
		if (!m_IsMove)
		{
			m_IsMove = true;
			schedule(schedule_selector(Boss::jump), 0.1f);
		}
				
		CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("BossWalk"));
		CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(Boss::shoot));
		this->runAction(CCSequence::create(animate, done, NULL));
		m_IsRun = true;
	}	

	if (m_IsDefaultFace)
	{
		this->setFlipX(true);//方向向左	
	}
	else
	{
		this->setFlipX(false);//方向向右	
	}
}

void Boss::jump(float dt)
{ 
	m_CurJumpCount += dt;
	if (m_CurJumpCount >= m_JumpCount)
	{
		Biology::jump(350, false);
		m_CurJumpCount = 0;
		m_JumpCount = CCRANDOM_0_1() * 2 + 2.0f;
	}	
}

void Boss::shoot()
{
	BossBullet *bullet = BossBullet::create(m_Biology);
	bullet->setPosition(ccp(this->getPositionX(), this->getPositionY() + 5));
	bullet->setRunDir(m_IsDefaultFace);
	getMap()->addChild(bullet);
	MonsterBullet::m_Bullets->addObject(bullet);
	//CCLog("boss shoot");

	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("BossShoot"));
	this->scheduleOnce(schedule_selector(Boss::shootFinish), CCRANDOM_0_1() + 0.6f);
}

void Boss::shootFinish(float)
{
	m_IsRun = false;
	updateStatus();
}

void Boss::autoDead(Biology *obj, int)
{
	if (isGodMode())
		return;
	m_Life--;
	if (m_Life)	//还没有死
	{
		startGodMode();
		return;
	}		
	
	//停止所有动作，旋转向下移动死亡
	this->stopAllActions();	
	CCMoveBy *by = CCMoveBy::create(0.2f, ccp(obj->getHV1() * 0.1f, 20));
	CCMoveBy *by2 = CCMoveBy::create(2.0f, ccp(obj->getHV1(), -winSize.width));
	CCRotateBy *rotate = CCRotateBy::create(.8f, 360);
	CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(MonsterEnemy::removeSelf));
	CCSequence *seq = CCSequence::create(by, by2, done, NULL);
	this->runAction(seq);
	this->runAction(CCRepeatForever::create(rotate));

	this->scheduleOnce(schedule_selector(Boss::deadfinish), 2.0f);
}

void Boss::deadfinish(float)
{
	*m_IsDead = true;
}

void Boss::logic(Biology *obj)
{
	//碰撞检测判断
	if (m_Life)
	{		
		if (isCollision(obj))
		{			
			obj->loseLife();//马里奥减生命			
		}

		//判断马里奥是不是超过boss
		if (obj->getPositionX() > this->getPositionX() && m_IsDefaultFace)
		{
			m_IsDefaultFace = false;
			updateStatus();
		}
		else if (obj->getPositionX() < this->getPositionX() && !m_IsDefaultFace)
		{
			m_IsDefaultFace = true;
			updateStatus();
		}
	}
	
	//只要超出设定范围外，自动死亡，boss不会限制区域
	/*if (Common::isOutScreen(getMap(), this, obj->getPositionX()))
	{
		removeSelf();
	}*/
}
