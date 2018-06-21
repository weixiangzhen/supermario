#include "MarioBullet.h"

CCArray *Bullet::m_Bullets = NULL;

void Bullet::preload()
{
	{//子弹冰爆炸动画
		CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage("skill4.png");		
		CCArray* arr = CCArray::create();
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				/*CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pTexture, CCRectMake(j*(1024 / 5.0f), i*(1024 / 5.0f), 
																									(1024 / 5.0f), (1024 / 5.0f)));*/
				CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pTexture, CCRectMake(j * 192, i * 192 + 32, 192, 160));
				arr->addObject(frame);
			}
		}		
		CCAnimationCache::sharedAnimationCache()->addAnimation(CCAnimation::createWithSpriteFrames(arr, 0.05f), "IceBulletBlast");
	}
}

bool Bullet::init()
{
	if (!Biology::init()){ return false; }

	this->setZOrder(Common::Z_BULLETFRIEND);
	this->setScale(0.5f);	
	//this->setRotation(CCRANDOM_0_1() * 360);//360度随机得到一个角度

	m_IsMove = true;
	m_IsFly = false;

	m_H_s = 0;//计算水平位移，用于判断子弹是否移动一个屏幕的距离

	//在设置方向的时候会再进行赋值
	m_H_v1 = BULLET_H_V1;
	m_H_Max_v1 = BULLET_H_V1;
	//垂直方向的加速度和速度
	m_V_a = -BULLET_V_A;
	m_V_v1 = 0;
	m_V_Max_v1 = BULLET_MAX_V;
	return true;
}

//水平方向上的移动
void Bullet::moveH(float dt)
{
	//dt时间的位移
	float s = m_H_v1 * dt;
	this->setPositionX(this->getPositionX() + s);
	//累计水平位移
	m_H_s += s;
	if (_isCollisionLeft())
	{
		//子弹爆炸
		//CCLog("left collision");
		autoDead();
	}
	else if (_isCollisionRight())
	{
		//子弹爆炸
		//CCLog("right collision");
		autoDead();
	}
}

//垂直方向上的移动
void Bullet::moveV(float dt)
{		
	//dt时间的位移
	float s = m_V_v1 * dt + m_V_a *dt*dt / 2;
	m_V_v1 = m_V_v1 + m_V_a * dt;//dt时的速度
	this->setPositionY(this->getPositionY() + s);

	if (m_V_v1 <= -m_V_Max_v1)
		m_V_v1 = -m_V_Max_v1;	
	if (_isCollisionDown())
	{		
		//CCLog("_isCollisionDown , m_V_v1 = %g", m_V_v1);
		m_V_v1 = BULLET_V_V1;			
	}
	else if (_isCollisionUp())
	{
		m_V_a = -BULLET_V_A;
		m_V_v1 = 0;		
	}	
}

//更新子弹的状态
void Bullet::updateStatus()
{
	if (!m_IsMove)
	{			
		//普通子弹		
		this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("MarioBULLETBLAST1"));
		scheduleOnce(schedule_selector(Bullet::removeSelf), 0.2f);	
		return;
	}	
	//普通子弹
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("MarioBULLET"));	
}
//子弹爆照移除
void Bullet::removeSelf(float)
{
	this->removeFromParent();
	m_Bullets->removeObject(this);
}

//碰撞后最后执行的方法
void Bullet::autoDead()
{
	//CCLog("%g", m_V_v1);
	m_IsMove = false;
	m_Life = 0;
	updateStatus();
}

//判断是否碰撞
bool Bullet::isCollision(Biology *obj)
{	
	return (this->boundingBox().intersectsRect(obj->boundingBox()));
}

//逻辑
void Bullet::logic(float dt)
{
	if (m_IsMove)
	{
		move(dt);		
	}	

	if (m_Life && (m_H_s * m_H_s > winSize.width * winSize.width || this->getPositionX() <= 0 || this->getPositionY() <= getMap()->getTileSize().height))
	{
		autoDead();
	}
}

/************************************************************************/
/* 另外一种子弹对象，爆炸时为冰的                                          */
/************************************************************************/
bool IceBullet::init()
{
	if (!Bullet::init()){ return false; }
	
	m_GrowState = 1;
	return true;
}

void IceBullet::autoDead()
{
	m_IsMove = false;	
	//m_Life = 0;//不能让生命为0，因为爆炸的时候也可以造成伤害
	if (m_GrowState)
	{
		if (Common::isPlayEffect)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(effectNames[5], false);
		}
	}
	updateStatus();	
}

//更新子弹的状态
void IceBullet::updateStatus()
{
	if (!m_IsMove)
	{
		if (m_GrowState)
		{
			//CCLog("IceBulletBlast");
			//this->setRotation(0);//爆炸把角度恢复
			//普通子弹	
			this->setScale(1.0f);
			this->setDisplayFrameWithAnimationName("IceBulletBlast", 0);
			CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("IceBulletBlast"));
			CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(IceBullet::removeSelf));
			this->runAction(CCSequence::create(animate, done, NULL));
			m_GrowState = 0;
		}		
		return;
	}
	//普通子弹
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("MarioBULLET"));
}

void IceBullet::removeSelf()
{
	//CCLog("xxxxxxxxx");
	m_Life = 0;
	Bullet::removeSelf(0);
}

/************************************************************************/
/* 史泰龙子弹对象                                         */
/************************************************************************/
bool FireBullet::init()
{
	if (!Bullet::init()){ return false; }

	m_GrowState = 1;
	return true;
}

void FireBullet::autoDead()
{
	m_IsMove = false;
	m_Life = 0;//让生命为0
	/*if (m_GrowState)
	{
		if (Common::isPlayEffect)
		{
			SimpleAudioEngine::sharedEngine()->playEffect(effectNames[5], false);
		}
	}*/
	updateStatus();
}

//更新子弹的状态
void FireBullet::updateStatus()
{
	if (!m_IsMove)
	{
		if (m_GrowState)
		{
			//CCLog("IceBulletBlast");
			//this->setRotation(0);//爆炸把角度恢复
			//普通子弹	
			this->setScale(1.0f);
			this->setDisplayFrameWithAnimationName("IceBulletBlast", 0);
			CCAnimate *animate = CCAnimate::create(CCAnimationCache::sharedAnimationCache()->animationByName("IceBulletBlast"));
			CCCallFunc *done = CCCallFunc::create(this, callfunc_selector(FireBullet::removeSelf));
			this->runAction(CCSequence::create(animate, done, NULL));
			m_GrowState = 0;
		}
		return;
	}
	//普通子弹
	this->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("MarioBULLET"));
}

void FireBullet::removeSelf()
{
	//CCLog("xxxxxxxxx");
	m_Life = 0;
	Bullet::removeSelf(0);
}