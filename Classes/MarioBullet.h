#ifndef __MARIOBULLET_H__
#define __MARIOBULLET_H__

#include "Biology.h"

//水平匀速移动
#define BULLET_H_V1		600
//#define BULLET_H_A		600

//v1^2 = 80a 
#define BULLET_MAX_V	1100
#define BULLET_V_V1		400
#define BULLET_V_A		2000

/* 普通子弹 */
class Bullet : public Biology
{
public:
	static void preload();
	static CCArray *m_Bullets;

	CREATE_FUNC(Bullet);

	virtual bool init();

	virtual void setRunDir(bool isLeftMove)
	{ 
		//m_IsMoveLeft = b;
		if (isLeftMove)		
			m_H_v1 = -BULLET_H_V1;			
		else		
			m_H_v1 = BULLET_H_V1;		
		
		updateStatus();
	}

	virtual void autoDead();

	virtual bool isCollision(Biology*);
	
	virtual void logic(float);
protected:
	virtual void moveH(float dt);
	virtual void moveV(float dt);
	//virtual void move(float dt){ jump(m_V_v1, BULLET_V_A, false); moveV(dt); moveH(dt); }
	virtual void removeSelf(float);

	virtual void updateStatus();

protected:
	float m_H_s;

	bool m_IsMove;
};

/* 加强子弹 */
class IceBullet : public Bullet
{
public:
	CREATE_FUNC(IceBullet);
	virtual bool init();

	virtual CCRect boundingBox()
	{
		CCRect rect = CCSprite::boundingBox();
		if (!m_IsMove)
		{
			rect.origin.x += 32;
			rect.origin.y += 32;
			rect.size.width -= 64;
			rect.size.height -= 64;
		}		
		return rect;
	}

	virtual void autoDead();

protected:
	void removeSelf();
	void updateStatus();

};

class FireBullet : public Bullet
{
public:
	CREATE_FUNC(FireBullet);
	virtual bool init();

	/*virtual CCRect boundingBox()
	{
		CCRect rect = CCSprite::boundingBox();
		if (!m_IsMove)
		{
			rect.origin.x += 32;
			rect.origin.y += 32;
			rect.size.width -= 64;
			rect.size.height -= 64;
		}
		return rect;
	}*/

	virtual void autoDead();

protected:
	void removeSelf();
	void updateStatus();

};

#endif
