#include "MonsterBullet.h"

CCArray *MonsterBullet::m_Bullets = NULL;

MonsterBullet* MonsterBullet::create(Biology *obj)
{
	MonsterBullet *pRet = new MonsterBullet;
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

bool MonsterBullet::init(Biology *obj)
{
	if (!Biology::init()){ return false; }
	
	this->setZOrder(Common::Z_FOOD);	

	m_Biology = obj;

	//水平速度
	m_H_a = 0;//水平方向没有加速度
	m_H_Max_v1 = BIOLOGY_H_V * 1.4f;
	m_H_v1 = m_H_Max_v1;

	//垂直速度
	m_V_a = 0;
	m_V_Max_v1 = 0;
	m_V_v1 = m_V_Max_v1;

	m_IsMove = true;

	m_H_s = 0;
	return true;
}

void MonsterBullet::updateStatus()
{
	if (m_H_v1 > 0)
		this->setFlipX(false); 
	else 
		setFlipX(true); 
}

void MonsterBullet::autoDead()
{
	m_Life = 0;
	removeSelf();
	//CCLog("monster bullet remove");
}

bool MonsterBullet::isCollision(Biology *obj)
{
	return this->boundingBox().intersectsRect(obj->boundingBox());
}

void MonsterBullet::logic(float dt, Biology *obj)
{
	move(dt);

	//小于地图一个砖块的宽度，或者地图一个砖块的高度
	float dx = obj->getPositionX() - this->getPositionX();
	if (m_Life && ((dx * dx > winSize.width * winSize.width && m_H_s * m_H_s > winSize.width * winSize.width) || 
										this->getPositionX() <= 0 || this->getPositionY() <= getMap()->getTileSize().height))
	{
		autoDead();
	}
}

//水平方向运动
void MonsterBullet::moveH(float dt)
{
	//dt时间的位移
	float s = m_H_v1 * dt;
	this->setPositionX(this->getPositionX() + s);
	//累计水平位移
	m_H_s += s;	
}

//垂直方向不会运动
void MonsterBullet::moveV(float dt)
{

}

void MonsterBullet::removeSelf()
{
	this->removeFromParent();
	m_Bullets->removeObject(this);
}

/************************************************************************/
/* boss 子弹                                                            */
/************************************************************************/
BossBullet* BossBullet::create(Biology *obj)
{
	BossBullet *pRet = new BossBullet;
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

bool BossBullet::init(Biology *obj)
{
	if (!MonsterBullet::init(obj)){ return false; }

	this->setDisplayFrameWithAnimationName("BossBullet", 0);
	this->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("BossBullet"))));

	return true;
}


/************************************************************************/
/* cannon 子弹                                                          */
/************************************************************************/
CannonBullet* CannonBullet::create(Biology *obj)
{
	CannonBullet *pRet = new CannonBullet;
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

bool CannonBullet::init(Biology *obj)
{
	if (!MonsterBullet::init(obj)){ return false; }
	
	this->setZOrder(Common::Z_GROUND);
	this->setDisplayFrameWithAnimationName("CannonBullet", 0);
	this->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("CannonBullet"))));

	return true;
}
