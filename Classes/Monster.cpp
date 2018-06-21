#include "Monster.h"

CCArray *Monster::m_MAwardDatas = NULL;

//图片预加载
void Monster::preload()
{
	CCTexture2D *pObjectTexture = CCTextureCache::sharedTextureCache()->addImage("tiledmap/object.png");
	
	{//金币砖块
		CCArray *arr = CCArray::create();

		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 32, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(32, 32, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 32, 32, 32));
		arr->addObject(frame);

		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.5f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "BrickCoin");
	}

	{//岩石砖块
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 96, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "BrickRock");
	}

	{//金币特效
		CCArray *arr = CCArray::create();

		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(96, 32, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 32, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(160, 32, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 32, 32, 32));
		arr->addObject(frame);

		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.02f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "BrickMakeCoin1");
	}

	{//金币动画特效2
		CCArray *arr = CCArray::create();

		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(96, 128, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 128, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(160, 128, 32, 32));
		arr->addObject(frame);

		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.02f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "BrickMakeCoin2");
	}

	{//纯金币动画
		CCArray *arr = CCArray::create();

		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 128, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(32, 128, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 128, 32, 32));
		arr->addObject(frame);

		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.5f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "PropCoin");
	}

	{//食物
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(224, 0, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "FoodMushroom");

		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(96, 96, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "FoodFlower");

		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(224, 32, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "DarkFoodMushroom");
	}

	{//普通的怪物
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 160, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "MonsterSmall");
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(32, 160, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "MonsterSmallDead");

		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 160, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "MonsterSmallBlack");
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(96, 160, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "MonsterSmallBlackDead");
	}

	{//乌龟边飞边走动画
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 320, 64, 64));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 320, 64, 64));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "TortoiseFlyWalk");
		//乌龟行走动画
		arr = CCArray::create();
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 320, 64, 64));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 320, 64, 64));
		arr->addObject(frame);
		animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "TortoiseWalk");

		//龟壳
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 160, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "TortoiseHull");

		//乌龟疯狂时候的动画
		arr = CCArray::create();
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(160, 160, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 160, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(224, 160, 32, 32));
		arr->addObject(frame);
		animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "TortoiseCrazy");
	}

	{//钢盔龟行走动画
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 224, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(32, 224, 32, 32));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "HelmetWalk");		
		//钢盔龟壳
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 224, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "HelmetHull");
		//钢盔乌龟疯狂时候的动画
		arr = CCArray::create();
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(96, 224, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 224, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(160, 224, 32, 32));
		arr->addObject(frame);
		animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "HelmetCrazy");
	}

	{//带刺怪行走动画
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(32, 192, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 192, 32, 32));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "Backstab");
	}

	{//阶梯
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 608, 64, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "Stair");
	}

	{//水虎鱼动画
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 384, 64, 64));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 384, 64, 64));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "Piranha");
	}

	{//食人花动画
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 192, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(160, 192, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 192, 32, 32));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "SFlower");
	}

	{//火弹动画
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(160, 256, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 256, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(224, 256, 32, 32));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "AnBiao");
	}

	{//boss行走+发射动画
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 512, 64, 64));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 512, 64, 64));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 512, 64, 64));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 1.0f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "BossWalk");

		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 512, 64, 64));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "BossShoot");
	}

	{//boss子弹的动画		
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(0, 608, 64, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 608, 64, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 608, 64, 32));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "BossBullet");
	}

	{//圆盘
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 224, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "Disk");
	}

	{//炮弹的子弹
		CCArray *arr = CCArray::create();
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(64, 256, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(96, 256, 32, 32));
		arr->addObject(frame);
		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 256, 32, 32));
		arr->addObject(frame);
		CCAnimation *animation = CCAnimation::createWithSpriteFrames(arr, 0.2f);
		CCAnimationCache::sharedAnimationCache()->addAnimation(animation, "CannonBullet");
	}

	{//马里奥的普通子弹1
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(192, 128, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "MarioBULLET");

		frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(224, 128, 32, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "MarioBULLETBLAST1");	
	}

	{//结束旗帜
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(pObjectTexture, CCRectMake(128, 480, 64, 32));
		CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFrame(frame, "End");	
	}
}

//创建对象
Monster* Monster::create(Biology *obj)
{
	Monster *pRet = new Monster;
	if (pRet && pRet->init(obj))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = NULL;
	}
	return pRet;
}

//初始化
bool Monster::init()
{
	if (!Biology::init()){ return false; }	
	//设置怪物的zorder
	this->setZOrder(Common::Z_FOOD);	

	m_Biology = NULL;//	

	m_IsMove = false;//默认不能移动
	return true;
}

bool Monster::init(Biology *obj)
{
	if (!Monster::init()){ return false; }
	
	m_Biology = obj;
	return true;
}

void Monster::addAward(CCPoint p, bool isMakeCoin, bool isMonster, int score)
{
	MAwardData *data = MAwardData::create();
	data->setPosition(p);
	data->setMakeCoin(isMakeCoin);
	data->setMonster(isMonster);
	data->setScore(score);
	m_MAwardDatas->addObject(data);
}

//判断是否碰撞
bool Monster::isCollision(Biology *obj)
{	
	if (obj->getLife() <= 0)return false;//生命小于等于0后，不在碰撞	
	return (this->boundingBox().intersectsRect(obj->boundingBox()));
}

//是否被砖块碰到
bool Monster::isCollisionBrick(int *pos)
{
	CCTMXLayer *layer = getMap()->layerNamed("monster_image");

	CCRect rect = this->boundingBox();
	//-10是为了等下转换是否是砖块，正常应该是减砖块高度的一半
	CCPoint p[3] = { ccp(rect.getMidX(), rect.getMinY() - 10), ccp(rect.getMinX(), rect.getMinY() - 10), ccp(rect.getMaxX(), rect.getMinY() - 10) };

	for (int i = 0; i < 3; i++)
	{
		CCPoint pt1 = Common::pointToTile(getMap(), p[i]);
		CCSprite *spr = layer->tileAt(pt1);
		if (!spr)
			continue;

		//方块正在抖动
		CCPoint pt2 = Common::tileToPoint(getMap(), pt1);
		if (pt2.y != spr->getPositionY())
		{
			if (pos)			
				*pos = i;
			return true;
		}
	}
	return false;
}

//碰撞检测
/*void Monster::logic(Biology *obj)
{
	if (m_Life && isCollision(obj))
	{
		//m_Life = 0;	
		autoDead(obj);
	}

	//每次都判断是否要移除自己
	removeSelf();
	
}*/
//碰撞检测+移动
/*void Monster::logic(float dt, Biology *obj)
{
	if (m_Life && m_IsMove)
	{
		move(dt);
	}
	logic(obj);		
}*/

