#ifndef __COMMON_H__
#define __COMMON_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace CocosDenshion;
//using namespace std;

#define winSize CCDirector::sharedDirector()->getWinSize()

#define GAME_FONT					"font/mario.fnt"
#define zh_CN_XML					"zh_CN.xml"
#define zh_STARTGAME                "startgame"
#define zh_ENTERGAME                "entergame"
#define zh_RESUMEGAME				"resumegame"
#define zh_SETTINGS					"settings"
#define zh_HELP						"help"
#define zh_ABOUT					"about"
#define zh_BACKGAME					"backgame"
#define zh_BUYMUSHROOM				"buymushroom"
#define zh_BUYLIFE					"buylife"
#define zh_SHOPHINT					"shophint"
#define zh_QUITGAMETEXT				"quitgametext"
#define zh_CONFIRM					"confirm"
#define zh_CANCEL					"cancel"
#define zh_LIFETEXT					"lifetext"
#define zh_COINTEXT					"cointext" 
#define zh_GROWTEXT					"growtext"
#define zh_SCORETEXT				"scoretext"
#define zh_TIMETEXT					"timetext"
#define zh_PASSHINT					"passhint"
#define zh_GAMEOVER1TEXT			"gameover1text"
#define zh_GAMEOVER2TEXT			"gameover2text"
#define zh_SHOPBUYLIFE				"shopbuylife"
#define zh_MAINMENU					"mainmenu"
#define zh_HELPPAUSE				"helppause"
#define zh_HELPLEFT					"helpleft"
#define zh_HELPRIGHT				"helpright"
#define zh_HELPGROW					"helpgrow"
#define zh_HELPSHOOT				"helpshoot"
#define zh_HELPJUMP					"helpjump"
#define zh_HELPSHOP					"helpshop"
#define zh_ABOUTAUTHOR				"aboutauthor"
#define zh_ABOUTQQ					"aboutqq"
#define zh_ABOUTEMAIL				"aboutemail"
#define zh_ACHIEVEMENT				"achievement"
#define zh_ACHIEVEMENTCOIN			"achievementcoin"
#define zh_ACHIEVEMENTMONSTER		"achievementmonster"

static const char *musicNames[] = { "music/back.ogg", "music/start.ogg", "music/m_back.ogg" };
static const char *effectNames[] = {
	"music/bubbling.ogg",//0
	"music/coin.ogg",//1
	"music/makeProp.ogg",//2
	"music/eatMushroom.ogg",//3	
	"music/fire.ogg",	//4
	"music/ice.ogg",//5
	"music/jump.ogg",	//6
	"music/m_hitbrick.ogg",//7
	"music/m_hitMonster.ogg",//8
	"music/m_hitTortoise.ogg",//9
	"music/m_dead.ogg",//10
	"music/time1.ogg",//11
	"music/gun_h.wav",
	"music/jump.wav"/*,

	"music/menu.ogg",
	"music/haliluya.ogg",
	"music/hitCoin.ogg",
	"music/hitCoin2.ogg",
	"music/hitMonster.ogg",
	"music/hitProp.ogg",
	"music/hitStone.ogg",
	"music/m_bullethitMonster.ogg",
	"music/m_deadbefore.ogg",
	"music/m_eatMushroom.ogg",
	"music/m_fire1.ogg",
	"music/m_fire2.ogg",
	"music/explode.ogg",
	"music/m_hitcoin.ogg",
	"music/m_jump1.ogg",
	"music/m_jump2.ogg",
	"music/m_mushroom.ogg",
	"music/m_pass.ogg",
	"music/thunder.ogg",
	"music/time.ogg",	
	"music/trampleMonster.ogg",
	"music/trampleTortoise.ogg",
	"music/upCollide.ogg",
	"music/waterfall.ogg",
	"music/wind.ogg"*/ };

class Common
{
public:
	enum ZORDER	{ Z_BACKGROUND = 50, Z_PILLAR, Z_CLOUD, Z_HILL, Z_GROUND, Z_BARRIER, Z_FOOD, Z_MONSTER, Z_BULLETFRIEND, Z_Figure };

	//static int playerID;
	static bool isPlayEffect;
	static bool isPlayMusic;

	//场景
	static CCScene* scene(CCLayer *layer)
	{
		CCScene *pScene = CCScene::create();
		pScene->addChild(layer);
		return pScene;
	}
	//正在加载动画
	static void loadingAnimation(CCLayer *layer)
	{
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage("loading.png");
		CCArray* arr = CCArray::create();
		for (int i = 0; i <= 11; ++i)
		{
			CCSpriteFrame* frame;
			if (i >= 10)
			{
				frame = CCSpriteFrame::createWithTexture(texture, CCRectMake((i - 10) * 100, 200, 100, 100));
			}
			else if (i >= 5)
			{
				frame = CCSpriteFrame::createWithTexture(texture, CCRectMake((i - 5) * 100, 100, 100, 100));
			}
			else
			{
				frame = CCSpriteFrame::createWithTexture(texture, CCRectMake(i * 100, 0, 100, 100));
			}
			arr->addObject(frame);
		}

		CCSprite *loadingSprite = CCSprite::createWithSpriteFrame((CCSpriteFrame*)arr->objectAtIndex(0));
		CCAnimation *pAnimation = CCAnimation::createWithSpriteFrames(arr, 0.1f);
		CCAnimate *animate = CCAnimate::create(pAnimation);
		loadingSprite->runAction(CCRepeatForever::create(animate));
		loadingSprite->setPosition(ccp(winSize.width / 2, winSize.height / 2));
		layer->addChild(loadingSprite);
	}

	//设置游戏文字
	static CCLabelBMFont* fontLabel(CCDictionary *dict, const char *text, CCPoint p, float scale)
	{		
		CCString *str = (CCString*)dict->objectForKey(text);
		CCLabelBMFont *labelFont = CCLabelBMFont::create(str->getCString(), GAME_FONT);
		labelFont->setScale(scale);
		labelFont->setPosition(p);
		return labelFont;
	}
	static CCLabelBMFont* fontLabel(const char *text, CCPoint p, float scale)
	{		
		CCLabelBMFont *labelFont = CCLabelBMFont::create(text, GAME_FONT);
		labelFont->setScale(scale);
		labelFont->setPosition(p);
		return labelFont;
	}

	//设置游戏文字
	static CCLabelBMFont* fontLabel(CCDictionary *dict, const char *text, CCPoint p)
	{
		return fontLabel(dict, text, p, 0.75f);
	}

	static CCLabelBMFont* fontLabel(const char *text, CCPoint p)
	{
		return fontLabel(text, p, 0.75f);
	}

	static CCMenuItemLabel* itemLabel(CCDictionary *dict, const char *text, CCPoint p)
	{
		CCString *str = (CCString*)dict->objectForKey(text);
		return itemLabel(str->getCString(), p);
	}

	static CCMenuItemLabel* itemLabel(const char *text, CCPoint p)
	{		
		CCMenuItemLabel *item = CCMenuItemLabel::create(CCLabelBMFont::create(text, GAME_FONT));
		//item->setScale(0.85f);
		item->setPosition(p);
		return item;
	}

	static const char* format(int i, char *preix = "", char* suffix = "")
	{
		static char buf[512];
		sprintf(buf, "%s%d%s", preix, i, suffix);
		return buf;
	}

	static CCPoint tileToPoint(CCTMXTiledMap *map, CCPoint pt)
	{	
		/*float fw = map->getTileSize().width;
		float fh = map->getTileSize().height;
		return ccp(pt.x*fw, (map->getMapSize().height - pt.y - 1)*fh);*/
		pt.y = map->getMapSize().height - 1 - pt.y;

		return ccp(pt.x * map->getTileSize().width, pt.y * map->getTileSize().height);
			
	}

	static CCPoint pointToTile(CCTMXTiledMap *map, CCPoint pl)
	{
		float dx = map->getTileSize().width;
		float dy = map->getTileSize().height;

		int x = pl.x / dx;
		//pl.y/dy : 一定要强转成int，否则pl.y/dy的结果是浮点数，整个计算以浮点运算，最后结果以浮点数转换成int，导致不正确
		int y = map->getMapSize().height - 1 - (int)(pl.y / dy);		
		return ccp(x, y);		
	}

	static void bgMove(CCTMXTiledMap *map, float s)
	{
		CCTMXLayer *layer0 = map->layerNamed("background");
		if (!layer0)
		{		
			CCTMXLayer *layer1 = map->layerNamed("hill");
			layer1->setPositionX(layer1->getPositionX() + (s / 2));
			CCTMXLayer *layer2 = map->layerNamed("small_cloud");
			layer2->setPositionX(layer2->getPositionX() + (s / 2));
			CCTMXLayer *layer3 = map->layerNamed("big_cloud");
			layer3->setPositionX(layer3->getPositionX() + (s / 2));
			CCTMXLayer *layer4 = map->layerNamed("pillar");
			layer4->setPositionX(layer4->getPositionX() + (s / 2));
		}
	}

	//判断是否在屏幕内
	static bool isOnScreen(CCTMXTiledMap *map , CCSprite *spr)
	{
		CCPoint pl = map->convertToWorldSpace(spr->getPosition());
		if (pl.x < winSize.width + spr->getContentSize().width / 2 && 
						pl.y >= -spr->getContentSize().height && pl.y <= winSize.height + spr->getContentSize().height)
		{
			return true;
		}
		return false;
	}

	//判断是否在屏幕外
	static bool isOutScreen(CCTMXTiledMap *map, CCSprite *spr, float currentX)
	{
		float ox = currentX;
		float tx = spr->getPositionX();
		if (ox > tx + winSize.width * 3 / 2 || spr->getPositionX() < 0 || spr->getPositionY() <= map->getTileSize().height)
		{
			return true;
		}
		return false;
	}

	//创建动画
	static CCAnimation* createAnimation(const char* filename, int start, int end, int width, int height, float delay)
	{
		CCTexture2D* texture = CCTextureCache::sharedTextureCache()->addImage(filename);
		CCArray* arr = CCArray::create();
		for (int i = start; i <= end; ++i)
		{
			CCSpriteFrame* frame = CCSpriteFrame::createWithTexture(texture, CCRectMake(i*width, 0, width, height));
			arr->addObject(frame);
		}

		return CCAnimation::createWithSpriteFrames(arr, delay);
	}
};

#endif