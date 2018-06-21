#ifndef __LAYERBASE_H__
#define __LAYERBASE_H__

#include "Common.h"

/* 只有商店按钮 */
class LayerBase1 : public CCLayer
{
public:
	CREATE_FUNC(LayerBase1);
	virtual bool init();

	virtual void onEnter();
	virtual void onExit();

	virtual void keyBackClicked();
	virtual void keyMenuClicked();

	virtual void overDialog(bool b);
protected:
	virtual void shopCallBack(CCObject*);

public:
	virtual void addPopup(){ m_PopupCount++; }
	virtual void subPopup(){ m_PopupCount--; }
protected:
	int m_PopupCount;
};

/* 有商店按钮和菜单按钮 */
class LayerBase2 : public LayerBase1
{
public:
	CREATE_FUNC(LayerBase2);
	virtual bool init();

	virtual void onEnter();
	virtual void onExit();	
protected:
	virtual void settingsCallBack(CCObject*);
	virtual void growCallBack(CCObject*);
};
#endif // __LAYERBASE_H__

