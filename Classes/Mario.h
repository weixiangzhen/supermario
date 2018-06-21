#ifndef __MARIO_H__
#define __MARIO_H__

#include "Biology.h"

//#define MARIO_H_V1 50
#define MARIO_H_A					550
#define MARIO_H_MAX_V				330

//影响上升/下落的速度
#define MARIO_V_A					1300
//影响跳跃的高度
#define MARIO_V_MAX_V				600

#define STALLONE_H_A				550
#define STALLONE_H_MAX_V			330
#define STALLONE_V_A				1300
#define STALLONE_V_MAX_V			600

#define MARIO_SMALL_RECT_DAFAULT			CCRectMake(5*(512 / 9.0f), 0, (512 / 9.0f), 57)
#define MARIO_SMALL_RECT_WALKING1			CCRectMake((512 / 9.0f), 0, (512 / 9.0f), 57)
#define MARIO_SMALL_RECT_BRAKING			CCRectMake(3*(512 / 9.0f), 0, (512 / 9.0f), 57)
#define MARIO_SMALL_RECT_JUMP				CCRectMake(2*(512 / 9.0f), 0, (512 / 9.0f), 57)
#define MARIO_SMALL_RECT_STOPWALK1			CCRectMake(0, 0, (512 / 9.0f), 57)

#define MARIO_BIG1_RECT_DAFAULT				CCRectMake(5*(512 / 9.0f), 57, (512 / 9.0f), 57)
#define MARIO_BIG1_RECT_WALKING1			CCRectMake((512 / 9.0f), 57, (512 / 9.0f), 57)
#define MARIO_BIG1_RECT_BRAKING				CCRectMake(3*(512 / 9.0f), 57, (512 / 9.0f), 57)
#define MARIO_BIG1_RECT_JUMP				CCRectMake(2*(512 / 9.0f), 57, (512 / 9.0f), 57)
#define MARIO_BIG1_RECT_STOPWALK1			CCRectMake(0, 57, (512 / 9.0f), 57)

#define MARIO_BIG2_RECT_DAFAULT				CCRectMake(5*(512 / 9.0f), 114, (512 / 9.0f), 57)
#define MARIO_BIG2_RECT_WALKING1			CCRectMake((512 / 9.0f), 114, (512 / 9.0f), 57)
#define MARIO_BIG2_RECT_BRAKING				CCRectMake(3*(512 / 9.0f), 114, (512 / 9.0f), 57)
#define MARIO_BIG2_RECT_JUMP				CCRectMake(2*(512 / 9.0f), 114, (512 / 9.0f), 57)
#define MARIO_BIG2_RECT_STOPWALK1			CCRectMake(0, 114, (512 / 9.0f), 57)

#define MARIO_RECT_DEAD						CCRectMake(6*(512 / 9.0f), 0, (512 / 9.0f), 57)
#define MARIO_RECT_SHOOT					CCRectMake(6*(512 / 9.0f), 114, (512 / 9.0f), 57)

/* 马里奥和魂斗罗主角的父类*/
class Figure : public Biology
{
public:
	//移动方向
	enum MOVE_DIR{ MOVE_LEFT = -1, MOVE_NONE, MOVE_RIGHT };

	CREATE_FUNC(Figure);
	virtual bool init()
	{ 
		if (!Biology::init()){ return false; }	

		this->setZOrder(Common::Z_Figure);
		this->setAnchorPoint(ccp(0, 0));

		m_IsLeftFace = false;//默认的脸是朝右的
		m_DirRun = MOVE_NONE;
		//是否完全死亡
		m_IsDead = false;
		//自动运行
		m_IsAutoRun = false;
		//一直跳
		m_IsJump = false;
		//一直发射
		m_IsShoot = false;
		//自动运行的距离
		m_RunDistance = 0;
		return true; 
	}

	virtual void setGrowState(int, bool){}

	bool isDead(){ return m_IsDead; }

	void setAutoRun(bool b){ m_IsAutoRun = b; }
	bool isAutoRun(){ return m_IsAutoRun; }

	void setRunDir(MOVE_DIR dir){ if (m_Life <= 0)return; if (m_IsAutoRun)return; m_DirRun = dir; }
	MOVE_DIR getRunDir(){ return m_DirRun; }

	void setJump(bool b){ m_IsJump = b; }
	bool isJump(){ return m_IsJump; }

	virtual void shoot(){}
	virtual void setShoot(bool){}
	bool isShoot(){ return m_IsShoot; }

	virtual void dead(){}

	void setRunDistance(float d){ this->m_RunDistance = d; }
protected:
	//运行方向
	MOVE_DIR m_DirRun;
	//脸朝向
	bool m_IsLeftFace;

	bool m_IsShoot;

	//跳跃标记
	bool m_IsJump;
	//是否自动运行
	bool m_IsAutoRun;
	//死透了
	bool m_IsDead;
	//自动运行后需要运行的距离
	float m_RunDistance;
};

/* 马里奥 */
class Mario : public Figure
{
public:
	static void preload();
	//马里奥移动的方向
	enum MARIO_STATE{ M_LEFT = -2, M_LEFTBRAKING, M_NONE, M_RIGHT, M_RIGHTBRAKING };

	CREATE_FUNC(Mario);
	virtual bool init();

	virtual CCRect boundingBox();
	virtual void setGrowState(int state){ setGrowState(state, false); }
	virtual void setGrowState(int, bool);
protected:
	void godmodeing(float);
public:
	//跳跃	
	virtual void jump(float, bool);
	virtual void jump(){ jump(MARIO_V_MAX_V, false); }

	virtual void loseLife();


	//bool isDead(){ return m_IsDead; }

	//void setAutoRun(bool b){ m_IsAutoRun = b; }
	//bool isAutoRun(){ return m_IsAutoRun; }

	//virtual void setRunDir(MOVE_DIR dir){ if (m_Life <= 0)return; if (m_IsAutoRun)return; m_DirRun = dir; }
	//MOVE_DIR getRunDir(){ return m_DirRun; }	

	virtual void shoot();
	virtual void setShoot(bool);	
	//bool isShoot(){ return m_IsShoot; }	

	virtual void dead();
	void dieFinish(float);

	//void setRunDistance(float d){ this->m_RunDistance = d; }
protected:
	virtual void update(float);

	virtual void calcX(float);//计算并设置水平方向的距离
	virtual void moveH(float);//水平方向的移动
	virtual void calcY(float);//计算并设置垂直方向的距离
	virtual void moveV(float);//垂直方向的移动		
	virtual void move(float);//移动方法
	//行走和停止
	void walking();
	void stopWalk();
	//更新状态
	virtual void updateStatus();

	void restoreNomal(float){ updateStatus(); }	
private:	
	//马里奥现在在哪个方向运行
	MARIO_STATE m_State;

	//发射子弹标记
	//bool m_IsShoot;
	bool m_IsReadyShoot;
	float m_ShootCount;
}; 

/* 史泰龙 */
class Stallone : public Figure
{
public:
	static void preload();

	CREATE_FUNC(Stallone);
	virtual bool init();

	virtual CCRect boundingBox()
	{
		CCRect rect = CCSprite::boundingBox();
		//CCLog("%f,%f", rect.size.width, rect.size.height);
		if (m_IsFly)
			return CCRectMake(rect.origin.x, rect.origin.y, 30, 30);
		return CCRectMake(rect.origin.x, rect.origin.y, 30, 48);
	}

	virtual void setGrowState(int state){ setGrowState(state, false); }
	virtual void setGrowState(int, bool);
protected:
	void godmodeing(float);

public:
	//跳跃	
	virtual void jump(float, bool);
	virtual void jump(){ jump(m_V_Max_v1, false); }

	virtual void loseLife();

	virtual void shoot();
	void shootFinish();
	virtual void setShoot(bool);

	virtual void dead();
	void dieFinish(float);	
protected:
	virtual void update(float);

	virtual void calcX(float);//计算并设置水平方向的距离
	virtual void moveH(float);//水平方向的移动
	virtual void calcY(float);//计算并设置垂直方向的距离
	virtual void moveV(float);//垂直方向的移动	
	virtual void move(float dt);//移动方法
	//更新状态
	virtual void updateStatus();

private:
	MOVE_DIR m_State;

	bool m_IsFlyRunning;
	bool m_IsReadyShoot;
	float m_ShootCount;
};
#endif