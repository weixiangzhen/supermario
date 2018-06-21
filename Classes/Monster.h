#ifndef __MONSTER_H__
#define __MONSTER_H__

#include "Biology.h"

class MAwardData : public CCObject
{
public:
	CREATE_FUNC(MAwardData);

	bool init()
	{ 
		m_pt = CCPointZero;
		m_score = 0;
		m_IsMakeCoin = false;
		m_IsMonster = false;
		return true; 
	}

	void setPosition(CCPoint p){ m_pt = p; }
	CCPoint getPosition(){ return m_pt; }

	void setScore(int score){ m_score = score; }
	int getScore(){ return m_score; }

	bool isMonster(){ return m_IsMonster; }
	void setMonster(bool isMonster){ this->m_IsMonster = isMonster; }

	void setMakeCoin(bool b){ m_IsMakeCoin = b; }
	bool isMakeCoin(){ return m_IsMakeCoin; }

private:
	int m_score;
	CCPoint m_pt;
	bool m_IsMakeCoin;
	bool m_IsMonster;
};

//所有怪物的父类，包括友方和敌方的
class Monster : public Biology
{
public:	
	static CCArray *m_MAwardDatas;

	static void preload();
	//创建对象
	CREATE_FUNC(Monster);
	static Monster* create(Biology*);
	//初始化
	virtual bool init();
	virtual bool init(Biology*);

	//重写boundingBox，为了有了怪物刚好是一个格子大小，在移动时不会往下掉空间只有一个格子的地方
	virtual CCRect boundingBox()
	{
		CCRect rect = CCSprite::boundingBox();
		rect.origin.x += 1;
		rect.size.width -= 2;
		return rect;
	}
	//添加到奖励数据中去
	void addAward(CCPoint p, bool isMakeCoin, bool isMonster, int score);

	bool isMove(){ return m_IsMove; }

	//维护了一个马里奥进来
	void setBiology(Biology *b){ this->m_Biology = b; }
	Biology* getBiology(){ return m_Biology; }	

	//判断怪物是否被砖块碰到
	bool isCollisionBrick(int * = NULL);

	virtual bool isCollision(Biology *obj);
	bool isCollision(){ return isCollision(m_Biology); }
	//
	virtual void autoDead(Biology*, int num){}//被碰撞后要做的事情
	virtual void autoDead(Biology *obj){ autoDead(obj, 0); }//被碰撞后要做的事情

	virtual void removeSelf(){}
	virtual void logic(Biology *obj){}
	virtual void logic(float dt, Biology *obj){}
	//void logic(){ logic(m_Biology); }	
	void logic(float dt){ logic(dt, m_Biology); }	
	
protected:
	Biology *m_Biology;	
	
	bool m_IsMove;//能否移动
};

#endif//__MONSTER_H__


