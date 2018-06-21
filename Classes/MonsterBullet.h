#ifndef __MONSTERBULLET_H__
#define __MONSTERBULLET_H__

#include "Biology.h"

class MonsterBullet : public Biology
{
public:
	static CCArray *m_Bullets;

	//CREATE_FUNC(MonsterBullet);
	static MonsterBullet* create(Biology*);

	virtual bool init(Biology*);

	virtual void setRunDir(bool isLeftMove)
	{
		//m_IsMoveLeft = b;
		if (isLeftMove)
			m_H_v1 = -m_H_Max_v1;
		else
			m_H_v1 = m_H_Max_v1;

		updateStatus();
	}

	virtual void autoDead();

	virtual bool isCollision(Biology*);

	virtual void logic(float dt){ logic(dt, m_Biology); }
	virtual void logic(float, Biology*);
protected:
	virtual void moveH(float dt);
	virtual void moveV(float dt);	
	void removeSelf();

	virtual void updateStatus();

protected:
	float m_H_s;

	bool m_IsMove;

	Biology *m_Biology;
};

/* boss 的子弹 */
class BossBullet : public MonsterBullet
{
public:
	//CREATE_FUNC(BossBullet);
	static BossBullet* create(Biology*);
	virtual bool init(Biology*);
};

/* 炮弹的子弹 */
class CannonBullet : public MonsterBullet
{
public:
	//CREATE_FUNC(CannonBullet);
	static CannonBullet* create(Biology*);
	virtual bool init(Biology*);

	virtual CCRect boundingBox()
	{
		CCRect rect = CCSprite::boundingBox();
		rect.size.height -= 4;
		return rect;
	}
};

#endif


