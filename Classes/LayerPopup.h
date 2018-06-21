#ifndef __LayerPOPUP_H__
#define __LayerPOPUP_H__

#include "Common.h"

class LayerPopup : public CCLayer
{
public:
	CREATE_FUNC(LayerPopup);
	virtual bool init();

private:
	//注册触摸事件，实现ccTouchBegan()方法
	void registerWithTouchDispatcher();
	bool ccTouchBegan(CCTouch * touch, CCEvent * pevent);

protected:
	int m_TouchStretch;

	CCDictionary *m_Dict;
};

/* 返回游戏对话框 */
class LayerBack :public LayerPopup
{
public:
	CREATE_FUNC(LayerBack);
	virtual bool init();

	//在弹出的对话框上加俩个按钮，以下的函数是对应的按钮的处理事件
	virtual void backCallBack(CCObject*);
protected:
	CCSprite *bgSprite;
};

/* 设置对话框 */
class LayerSettings : public LayerBack
{
public:
	CREATE_FUNC(LayerSettings);
	virtual bool init();

protected:
	void backCallBack(CCObject*);
	void backMenuCallBack(CCObject*);
	void musicCallBack(CCObject*);
	void soundCallBack(CCObject*);
};

/* 商店对话框 */
class LayerShop : public LayerBack
{
public:
	CREATE_FUNC(LayerShop);
	virtual bool init();

protected:
	void addMushroom(CCObject*);
	void addLife(CCObject*);

	virtual void update(float);
	virtual void backCallBack(CCObject*);
private:
	int m_GrowCount;
	int m_Life;
	int m_Coin;

	int m_TempCoin;

	CCLabelBMFont *m_LifeLabel;
	CCLabelBMFont *m_GrowLabel;
	CCLabelBMFont *m_CoinLabel;
};


/* 游戏结束和退出游戏对话框 */
class LayerOver : public LayerPopup
{
public:
	//CREATE_FUNC(LayerOver);
	static LayerOver* create(bool isEsc);
	virtual bool init(bool);

	void button1CallBack(CCObject*);
	void button2CallBack(CCObject*);

private:
	bool m_IsEsc;
};

#endif

