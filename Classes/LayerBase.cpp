#include "LayerBase.h"
#include "LayerPopup.h"

/************************************************************************/
/* 基础层1：只有商店                                                      */
/************************************************************************/
bool LayerBase1::init()
{
	if (!CCLayer::init()){ return false; }	
	
	this->setKeypadEnabled(true);
	m_PopupCount = 0;

	return true;
}

void LayerBase1::onEnter()
{
	CCLayer::onEnter();
	//添加商城
	CCTexture2D *shopTexture = CCTextureCache::sharedTextureCache()->addImage("menu.png");
	CCSprite *shopSprite = CCSprite::createWithTexture(shopTexture, CCRectMake(320, 256, 64, 64));
	shopSprite->setScale(0.9f);
	CCMenuItemSprite *shopItem = CCMenuItemSprite::create(shopSprite, shopSprite, this, menu_selector(LayerBase1::shopCallBack));
	shopItem->setPosition(ccp(winSize.width - shopItem->getContentSize().width / 2, winSize.height - shopItem->getContentSize().height / 2));

	CCMenu *shopMenu = CCMenu::create(shopItem, NULL);
	shopMenu->setPosition(CCPointZero);
	addChild(shopMenu);	

	//防止被后面的界面被遮住
	shopMenu->setZOrder(100);
}

void LayerBase1::onExit()
{
	CCLayer::onExit();
}

void LayerBase1::keyBackClicked()
{
	//CCLog("BACK clicked!");
	overDialog(true);
}

void LayerBase1::keyMenuClicked()
{
	CCLog("MENU clicked!");
}


void LayerBase1::shopCallBack(CCObject*)
{
	addPopup();
	this->addChild(LayerShop::create());

	//播放音效
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}
}

void LayerBase1::overDialog(bool b)
{	
	if (this->getChildByTag(3000))
		return;

	addPopup();
	LayerOver *layer = LayerOver::create(b);
	layer->setTag(3000);
	layer->runAction(CCMoveTo::create(0.5f, ccp(0, 0)));
	this->addChild(layer);
}

/************************************************************************/
/* 基础层2：菜单和商店                                                    */
/************************************************************************/
bool LayerBase2::init()
{
	if (!LayerBase1::init()){ return false; }

	return true;
}

void LayerBase2::onEnter()
{
	LayerBase1::onEnter();

	//添加设置按钮
	CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage("menu.png");
	CCSprite *settingsSprite = CCSprite::createWithTexture(pTexture, CCRectMake(256, 256, 64, 64));
	settingsSprite->setScale(0.9f);
	CCMenuItemSprite *settingsItem = CCMenuItemSprite::create(settingsSprite, settingsSprite, this, menu_selector(LayerBase2::settingsCallBack));
	settingsItem->setPosition(ccp(settingsItem->getContentSize().width / 2 + 20, winSize.height - settingsItem->getContentSize().height / 2));

	CCSprite *growSprite = CCSprite::createWithTexture(pTexture, CCRectMake(384, 256, 64, 64));
	growSprite->setScale(0.9f);
	CCMenuItemSprite *growItem = CCMenuItemSprite::create(growSprite, growSprite, this, menu_selector(LayerBase2::growCallBack));
	growItem->setPosition(ccp(winSize.width / 2 - 10, winSize.height - growSprite->getContentSize().height / 2));

	CCMenu *menu = CCMenu::create(settingsItem, growItem, NULL);
	menu->setPosition(CCPointZero);
	addChild(menu);

	//防止被后面的界面被遮住
	menu->setZOrder(100);
}

void LayerBase2::onExit()
{
	LayerBase1::onExit();
}

void LayerBase2::settingsCallBack(CCObject*)
{
	addPopup();
	this->addChild(LayerSettings::create());

	//播放音效
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}
}

void LayerBase2::growCallBack(CCObject*)
{
	//播放音效
	if (Common::isPlayEffect)
	{
		SimpleAudioEngine::sharedEngine()->playEffect(effectNames[0], false);
	}
}
