#ifndef __BIOLOGY_H__
#define __BIOLOGY_H__

#include "Common.h"

#define BIOLOGY_H_V			90

#define BIOLOGY_V_MAX_V		500
#define BIOLOGY_V_V			300
#define BIOLOGY_V_A			900

/* 所有对象的父类 */
class Biology : public CCSprite
{
public:	
	static CCArray *m_Stairs;
	
	CREATE_FUNC(Biology);
	virtual bool init();

	virtual void onEnter();
	virtual void onExit();
	//获取地图对象
	virtual CCTMXTiledMap* getMap(){ return (CCTMXTiledMap*)this->getParent(); }
	//成长状态
	virtual void setGrowState(int state){ this->m_GrowState = state; }
	int getGrowState(){ return m_GrowState; }
	//生命值设置
	virtual void loseLife();
	void setLife(int life){ m_Life = life; }
	int getLife(){ return m_Life; }
	//设置获取无敌模式
	bool isGodMode(){ return m_IsGodMode; }
	void startGodMode();
	void stopGodMode();
protected:
	virtual void godmodeing(float);
public:
	//是否在空中
	bool isFly(){ return m_IsFly; }
	//设置获取水平速度
	float getHV1(){ return m_H_v1; }
	void setHV1(float v1){ m_H_v1 = v1; }
	//设置获取垂直速度
	float getVV1(){ return m_V_v1; }	
	float setVV1(float v1){ m_V_v1 = v1; }

	virtual void jump(float, float, bool);
	virtual void jump(float vv, bool isIgnore){ jump(vv, BIOLOGY_V_A, isIgnore); }//跳跃	
	virtual void jump(){ jump(BIOLOGY_V_V, false); }


	virtual void mapMoveH(Biology *b, float s)
	{		
		CCPoint pt = getMap()->convertToWorldSpace(b->getPosition());
		if ((this->getHV1() > 0 && winSize.width / 2 < pt.x) || (this->getHV1() <= 0 && winSize.width / 2 >= pt.x && getMap()->getPositionX() < 0))
		{
			getMap()->setPositionX(getMap()->getPositionX() - s);

			if (getMap()->getPositionX() > 0)
			{
				getMap()->setPositionX(0);
			}
			Common::bgMove(getMap(), s / 2);
		}
	}

	virtual void mapMoveV(Biology *b, float s)
	{	
		CCPoint pt = getMap()->convertToWorldSpace(b->getPosition());
	
		if ((this->getVV1() > 0 && winSize.height * 2 / 3 < pt.y &&
			getMap()->getPositionY() > -(getMap()->getContentSize().height - winSize.height - getMap()->getTileSize().height)) ||
			(this->getVV1() <= 0 && winSize.height / 3 >= pt.y && getMap()->getPositionY() < 0))			
		{
			getMap()->setPositionY(getMap()->getPositionY() - s);
			if (getMap()->getPositionY() > 0)
			{
				getMap()->setPositionY(0);
			}
		}
	}

protected:	
	virtual int _isCollision(CCPoint *p, int size);
	virtual CCRect _isCollisionStair(CCPoint *p, int size);
	virtual bool _isCollisionUp();
	virtual bool _isCollisionDown();
	virtual bool _isCollisionLeft();
	virtual bool _isCollisionRight();
	virtual void calcX(float dt);
	virtual void moveH(float dt);
	virtual void calcY(float dt);
	virtual void moveV(float dt);
	virtual void move(float dt){ moveV(dt); moveH(dt); }
	virtual void updateStatus(){}	
protected:
	//当前成长情况
	int m_GrowState;
	//当前生命值
	int m_Life;
	//判断是否是无敌模式
	bool m_IsGodMode;
	unsigned int m_GodModeRepeat;
	//水平方向的速度和加速度
	float m_H_v1;
	float m_H_Max_v1;
	float m_H_a;
	//垂直方向的速度和加速度
	float m_V_v1;
	float m_V_Max_v1;
	float m_V_a;
	//判断是否在空中
	bool m_IsFly;
	
};
#endif
