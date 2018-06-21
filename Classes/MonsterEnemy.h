#ifndef __MONSTERENEMY_H__
#define __MONSTERENEMY_H__

#include "Monster.h"

/************************************************************************/
/* 提示：所有继承MonsterEnemy的子类尽量不要重写logic(float, Biology*)， */
/* 应该重写logic(Biology*)这方法                                      */
/************************************************************************/
class MonsterEnemy : public Monster
{
public:
	static CCArray *m_MonsterEnemys;

	static MonsterEnemy* create(Biology*);;
	bool init(Biology*);
		
	virtual bool isCollision(Biology *obj)
	{		
		return Monster::isCollision(obj);
	}
	
	virtual void setLeftScope(int leftscope){ if (leftscope == 0)return; this->m_LeftScope = this->getPositionX() - leftscope; }
	virtual void setRightScope(int rightscope)
	{ 
		if (rightscope == 0)return; 
		this->m_RightScope = this->getPositionX() + rightscope + this->boundingBox().size.width / 2; 
	}
	virtual void setUpScope(int upscope){ if (upscope == 0)return; this->m_UpScope = this->getPositionY() + upscope; }
	virtual void setDownScope(int downscope){ if (downscope == 0)return; this->m_DownScope = this->getPositionY() - downscope; }

	//判断是否怪物的头部被别人踩了
	virtual bool isCollisionHeader(Biology*);	

	virtual void autoDead(Biology*, int num){}//被碰撞后要做的事情	
	virtual void logic(Biology*);//逻辑都在这	
	virtual void logic(float, Biology*);//逻辑都在这
	virtual void removeSelf();//移除自己

protected:
	virtual void moveH(float dt);
	virtual void moveV(float dt);
protected:
	bool m_IsRun;

	float m_LeftScope;
	float m_RightScope;

	float m_UpScope;
	float m_DownScope;
};

/* 蘑菇怪 */
class MonsterSmall : public MonsterEnemy
{
public:
	static MonsterSmall* create(Biology*, bool);
	bool init(Biology*, bool);
	
	virtual void updateStatus();
	virtual void autoDead(Biology*, int);
	//virtual void logic(Biology *obj);

private:
	void walk(float);
private:	
	bool m_IsDiscolor;
};

/* 乌龟 */
#define TORTOISE_H_V_FLY	95
#define TORTOISE_H_V		BIOLOGY_H_V / 2
#define TORTOISE_H_V_CRAZY	400

//v*v = 100a
#define TORTOISE_V_V		300
#define TORTOISE_V_A		900
class Tortoise : public MonsterEnemy
{
public:
	static Tortoise* create(Biology*, int);
	bool init(Biology*, int);

	virtual CCRect boundingBox()
	{
		CCRect rect = CCSprite::boundingBox();
		//CCLog("%g,%g", rect.size.width, rect.size.height);
		if (m_GrowState >= 2)
		{
			rect.origin.x += 17;
			rect.origin.y += 0;
			rect.size.width -= 34;
			rect.size.height -= 18; 
		}		

		return rect;
	}
	//virtual bool isCollision(Biology *obj);
	void revive(float);
	
	bool isCollisionAll(CCArray *arrays);

	virtual void moveH(float dt);
	virtual void moveV(float dt);
	virtual void updateStatus();
	virtual void autoDead(Biology*, int);
	virtual void logic(Biology *obj);

private:	
	bool m_IsReviving;
	int m_MoveState;
};

/* 带刺怪 */
class Backstab : public MonsterEnemy
{
public:
	static Backstab* create(Biology*);
	bool init(Biology*);

	virtual void updateStatus();
	virtual void autoDead(Biology*, int);
	virtual void logic(Biology *obj);
private:

};

/* 阶梯 */
class Stair : public MonsterEnemy
{
public:
	static Stair* create(Biology*, int dir);
	bool init(Biology*, int dir);

	virtual CCRect boundingBox()
	{
		CCRect rect = CCSprite::boundingBox();
		rect.size.height = rect.size.height * 2 / 3;
		return rect;
	}

	virtual void setLeftScope(int leftscope)
	{ 
		 if (leftscope == 0)
			 return; 
		 this->m_LeftScope = this->getPositionX() - leftscope; 
	}
	virtual void setRightScope(int rightscope)
	{
		if (rightscope == 0)
			return;
		this->m_RightScope = this->getPositionX() + rightscope;
	}
	virtual void setUpScope(int upscope)
	{ 
		if (upscope == 0)
			return; 
		this->m_UpScope = (getMap()->getMapSize().height + 1)*getMap()->getTileSize().height - upscope;
	}
	virtual void setDownScope(int downscope)
	{ 
		if (downscope == 0)
			return; 	
		this->m_DownScope = downscope;		
	}
	//只覆写了水平方向计算
	virtual void calcX(float dt);

	virtual void updateStatus();
	virtual void logic(Biology *obj);
	virtual void logic(float dt, Biology *obj);//方式不一样
protected:
	virtual void moveH(float dt);
	virtual void moveV(float dt);

private:
	int m_Dir;
	bool m_IsOnLadders;//判断是否在梯子上
};

/* 炮弹 */
class Cannon : public MonsterEnemy
{
public:
	static Cannon* create(Biology*);
	bool init(Biology*);

	
	void shoot(float);

	virtual void updateStatus();
	virtual void logic(Biology *obj);

private:
	bool m_IsDefaultFace;
};

/* 钢盔龟 */
class Helmet : public MonsterEnemy
{
public:
	static Helmet* create(Biology*);
	bool init(Biology*);	
	
	void revive(float);

	//bool isCollisionAll(CCArray *arrays);

	//virtual void moveH(float dt);
	//virtual void moveV(float dt){}
	virtual void updateStatus();
	virtual void autoDead(Biology*, int);
	virtual void logic(Biology *obj);

private:
	bool m_IsReviving;
};

/* boss */
class Boss : public MonsterEnemy
{
public:
	static Boss* create(Biology*);
	bool init(Biology*);

	virtual void updateStatus();
	void shoot();
	void shootFinish(float);

	virtual void autoDead(Biology*, int);
	virtual void logic(Biology *obj);
	void jump(float);

	void setDead(bool *b){ m_IsDead = b; }
	void deadfinish(float);
private:
	float m_JumpCount;
	float m_CurJumpCount;

	bool m_IsDefaultFace;

	bool *m_IsDead;
};

#endif