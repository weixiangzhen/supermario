#include "Biology.h"

CCArray *Biology::m_Stairs = NULL;

bool Biology::init()
{
	if (!CCSprite::init()){ return false; }	

	//初始成长状态为0
	m_GrowState = 0;
	//默认生命值为1
	m_Life = 1;
	//默认不是无敌
	m_IsGodMode = false;
	m_GodModeRepeat = 0;
	//速度方面
	m_H_a = 0;//水平方向没有加速度
	m_H_Max_v1 = 0;
	m_H_v1 = m_H_Max_v1;	

	m_V_a = -BIOLOGY_V_A;
	m_V_Max_v1 = BIOLOGY_V_MAX_V;
	m_V_v1 = 0;	
	//默认没有跳跃，但会下落
	m_IsFly = false;
	
	return true;
}

void Biology::onEnter()
{
	CCSprite::onEnter();	
}

void Biology::onExit()
{
	CCSprite::onExit();
}
//减生命，规则：先减去成长状态，再减去生命
void Biology::loseLife()
{
	if (m_GrowState)
	{
		m_GrowState--;//成长状态-1		
	}
	else if (m_Life)
	{
		m_Life--;//生命减1，即为0了，代表死亡
	}
}
//开启无敌模式
void Biology::startGodMode()
{
	stopGodMode();

	m_IsGodMode = true;
	this->schedule(schedule_selector(Biology::godmodeing), 0.1f);//每0.1秒执行一次共30次无敌时间为3秒钟
}
//停止无敌模式
void Biology::godmodeing(float)
{
	m_GodModeRepeat++;
	if (m_GodModeRepeat == 20)
	{
		stopGodMode();
	}	
}

void Biology::stopGodMode()
{ 
	if (m_IsGodMode)//如果已经开启了无敌模式，则停止计时
	{
		m_GodModeRepeat = 0;
		m_IsGodMode = false;
		this->unschedule(schedule_selector(Biology::godmodeing));
	}	
}

//跳跃，参数1：跳跃时的速度，参数2：加速度，参数3：是否忽略在空中
void Biology::jump(float vv, float va, bool isIgnore)
{
	if (m_IsFly && !isIgnore)
	{
		return;
	}

	m_IsFly = true;
	m_V_a = -va;	
	m_V_v1 = vv;
	updateStatus();
}

/***********************************5个碰撞检测的方法：公用、顶部、底部、左边、右边****************************************/
int Biology::_isCollision(CCPoint *p , int size)
{	
	CCTMXLayer *barrierLayer = getMap()->layerNamed("barrier");
	CCTMXLayer *objectsLayer = getMap()->layerNamed("monster_image");
	for (int i = 0; i < size; i++)
	{
		CCPoint pt = Common::pointToTile(getMap(), p[i]);
		if (barrierLayer->tileGIDAt(pt) ||
			(objectsLayer->tileGIDAt(pt) == 11 && objectsLayer->tileAt(pt)->isVisible()) ||
			objectsLayer->tileGIDAt(pt) == 17 ||
			objectsLayer->tileGIDAt(pt) == 25 ||
			objectsLayer->tileGIDAt(pt) == 26 ||
			objectsLayer->tileGIDAt(pt) == 27)
		{			
			return i;
		}
	}
	return size;
}

CCRect Biology::_isCollisionStair(CCPoint *p, int size)
{
	CCObject *obj = NULL;
	CCARRAY_FOREACH(m_Stairs, obj)
	{
		Biology *b = (Biology*)obj;
		for (int i = 0; i < size; i++)
		{
			if (b->boundingBox().containsPoint(p[i]))
			{
				return b->boundingBox();
			}
		}
	}
	return CCRectZero;
}

//顶部碰撞检测
bool Biology::_isCollisionUp()
{
	CCRect rect = boundingBox();
	//大于地图宽度的最高点,不做碰撞检测
	//CCLog("aaaaaaaaaaaaaaaaaaaaaaaaaaaa,%g,%g", this->getPositionY() + rect.size.height, getMap()->getContentSize().height - getMap()->getTileSize().height);
	if (rect.origin.y + rect.size.height > getMap()->getContentSize().height - getMap()->getTileSize().height)
		return false;

	CCPoint p[3] = { ccp(rect.getMidX(), rect.getMaxY()), ccp(rect.getMinX(), rect.getMaxY()), ccp(rect.getMaxX(), rect.getMaxY()) };

	CCTMXLayer *barrierLayer = getMap()->layerNamed("barrier");
	int v = 0;	
	if ((v = _isCollision(p, 3)) != 3)
	{
		CCPoint pt = Common::pointToTile(getMap(), p[v]);
		//微调，-1是因为不能更碰撞物刚好粘在一起，避免刚好与障碍物碰撞，左右移动不了情况
		float dy = rect.origin.y - (Common::tileToPoint(getMap(), pt).y - rect.size.height - 1);
		this->setPositionY(this->getPositionY() - dy);
		//CCLog("Mario %g,%g", Common::tileToPoint(getMap(), pt).x, rect.origin.x);
		return true;		
	}
	CCRect stairRect = _isCollisionStair(p, 3);
	if (stairRect.size.width != 0 && stairRect.size.height != 0)
	{
		//一定要算出差值来，因为有些精灵的锚点不是0，0
		float dy = rect.origin.y - stairRect.origin.y - 1;
		//this->setPositionY(stairRect.origin.y - rect.size.height - 1);
		this->setPositionY(this->getPositionY() + dy);
		return true;
	}
	return false;
}
//_isCollisionDown与_isCollisionUp有点不一样，为什么？主要原因：还是地图的砖块的位置都是int，而马里奥的位置都是float，涉及到了强转的问题，都是为了规避这些问题
//需要后续修改的问题：统一锚点在（0.5，0），马里奥的锚点在（0，0），其他怪物的锚点在中心，修改锚点好处在于，像马里奥的图片有很大的透明空间，设置位置的时候都要得出一个差值，在设置，很麻烦。
//底部碰撞检测
bool Biology::_isCollisionDown()
{
	CCRect rect = boundingBox();	
	//大于地图宽度的最高点,不做碰撞检测
	if (rect.origin.y + rect.size.height > getMap()->getContentSize().height - getMap()->getTileSize().height)
		return false;

	//这里-1表示提前碰到，是因为不减-1马里奥会一直在跳和不跳之间，
	//保证了每次都提前与地面碰撞，好处在马里奥不会一直跳，
	CCPoint p[3] = { ccp(rect.getMidX(), rect.getMinY() - 1), ccp(rect.getMinX(), rect.getMinY() - 1), ccp(rect.getMaxX(), rect.getMinY() - 1) };
	
	int v = _isCollision(p, 3);
	if (v != 3)
	{		
		CCPoint pt = Common::pointToTile(getMap(), p[v]);
		//微调,dy作用是因为图片有空白区域，得出一个差值
		float dy = Common::tileToPoint(getMap(), pt + ccp(0, -1)).y - rect.origin.y;			
		this->setPositionY(this->getPositionY() + dy);
		return true;					
	}
	CCRect stairRect = _isCollisionStair(p, 3);
	if (stairRect.size.width != 0 && stairRect.size.height != 0)
	{
		//CCLog("down %g", stairRect.origin.y + stairRect.size.height);
		float dy = stairRect.origin.y + stairRect.size.height + 1 - rect.origin.y;
		this->setPositionY(this->getPositionY() + dy);
		return true;
	}
	return false;
}

//左边碰撞检测
bool Biology::_isCollisionLeft()
{
	CCRect rect = boundingBox();
	//大于地图宽度的最高点,不做碰撞检测
	if (rect.origin.y + rect.size.height > getMap()->getContentSize().height - getMap()->getTileSize().height)
		return false;

	CCPoint p[3] = { ccp(rect.getMinX(), rect.getMidY()), ccp(rect.getMinX(), rect.getMinY()), ccp(rect.getMinX(), rect.getMaxY()) };

	int v = _isCollision(p, 3);
	if (v != 3)
	{
		CCPoint pt = Common::pointToTile(getMap(), p[v]);
		//微调
		float dx = Common::tileToPoint(getMap(), pt + ccp(1, 0)).x - rect.origin.x;
		this->setPositionX(this->getPositionX() + dx);
		return true;		
	}
	CCRect stairRect = _isCollisionStair(p, 3);
	if (stairRect.size.width != 0 && stairRect.size.height != 0)
	{		
		//CCLog("left %g", stairRect.size.width);
		float dx = stairRect.origin.x + stairRect.size.width - rect.origin.x;
		this->setPositionX(this->getPositionX() + dx);
		return true;
	}
	return false;
}

//右边碰撞检测
bool Biology::_isCollisionRight()
{
	CCRect rect = boundingBox();
	//大于地图宽度的最高点,不做碰撞检测
	if (rect.origin.y + rect.size.height > getMap()->getContentSize().height - getMap()->getTileSize().height)
		return false;

	CCPoint p[3] = { ccp(rect.getMaxX(), rect.getMidY()), ccp(rect.getMaxX(), rect.getMinY()), ccp(rect.getMaxX(), rect.getMaxY()) };

	int v = _isCollision(p, 3);
	if (v != 3)
	{
		CCPoint pt = Common::pointToTile(getMap(), p[v]);
		//微调,-1是为了不让它碰到瓷砖	
		float dx = rect.origin.x - (Common::tileToPoint(getMap(), pt).x - rect.size.width - 1);			
		this->setPositionX(this->getPositionX() - dx);			
		return true;		
	}
	CCRect stairRect = _isCollisionStair(p, 3);
	if (stairRect.size.width != 0 && stairRect.size.height != 0)
	{
		//CCLog("right %g,%g", stairRect.origin.x, rect.size.width);
		//float dx = rect.origin.x + rect.size.width - stairRect.origin.x -1;
		float dx = rect.origin.x - (stairRect.origin.x - rect.size.width - 1);
		this->setPositionX(this->getPositionX() - dx);
		return true;
	}
	return false;
}
/******************************************End*********************************************/
/* 水平move的方法移动是匀速移动，垂直移动的方法是匀加速移动*/
/***********************************移动方法，包括与地图的碰撞**********************************/
void Biology::calcX(float dt)
{
	this->setPositionX(this->getPositionX() + m_H_v1 * dt);
}
//水平方向的移动判断
void Biology::moveH(float dt)
{
	if (m_H_v1 < 0)
	{
		calcX(dt);
		if (_isCollisionLeft())
		{			
			m_H_v1 = m_H_Max_v1;//碰到墙后速度反过来
			updateStatus();//更新一下状态
		}
	}
	else if (m_H_v1 > 0)
	{
		calcX(dt);
		if (_isCollisionRight())
		{		
			m_H_v1 = -m_H_Max_v1;//碰到墙后速度反过来
			updateStatus();
		}
	}
	else//不会动
	{

	}
}
void Biology::calcY(float dt)
{
	float s = m_V_v1 * dt + m_V_v1 * dt * dt / 2;
	m_V_v1 = m_V_v1 + m_V_a * dt;
	this->setPositionY(this->getPositionY() + s);//先上升或下落，在进行碰撞检测微调，防止左右行走的时候误判断
	//CCLog("%g", m_V_v1);
	if (m_V_v1 <= -m_V_Max_v1)//限制垂直方向的速度，避免过大		
		m_V_v1 = -m_V_Max_v1;
}
//垂直方向的移动判断
void Biology::moveV(float dt)
{	
	if (m_V_v1 <= 0)//下落
	{
		if (m_IsFly)
			calcY(dt);//先下落，在碰撞检测，方便做微调，不然水平移动会出现问题		
		if (_isCollisionDown())
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
		if (_isCollisionUp())
		{
			m_V_v1 = 0;
			m_V_a = -BIOLOGY_V_A;
		}
	}	
}
