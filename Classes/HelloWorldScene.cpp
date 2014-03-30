#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;
USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();
    
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    
    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Point(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                 origin.y + closeItem->getContentSize().height/2));
    
    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Point::ZERO);
    this->addChild(menu, 1);
    
    /////////////////////////////
    // 3. add your codes below...
    
    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Point(origin.x + visibleSize.width/2,
                             origin.y + visibleSize.height - label->getContentSize().height));
    
    // add the label as a child to this layer
    this->addChild(label, 1);
    
    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("HelloWorld.png");
    
    // position the sprite on the center of the screen
    sprite->setPosition(Point(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    
    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    // マップチップ表示 　ここから
    TMXTiledMap* pTileMap = TMXTiledMap::create("desert.tmx");
    this->addChild(pTileMap);
    
    // player
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("chara1.plist");
    eSprite = Sprite::createWithSpriteFrameName("chara1_top_3.jpg");
    eSprite->setPosition(Point(500*0.5, 500*0.5));
    eSprite->setTag(99);
    this->addChild(eSprite);
    
    // enemy
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("enemy1.plist");
    enemySprite = Sprite::createWithSpriteFrameName("enemy1_bottom_1.jpg");
    enemySprite->setPosition(Point(300*0.5, 300*0.5));
    enemySprite->setTag(100);
    this->addChild(enemySprite);
    
    // タッチイベント有効
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    // マップチップ表示 　ここまで
    // BGM
    //SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(0.5);
    //SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("tokei.mp3");
    //SimpleAudioEngine::sharedEngine()->playBackgroundMusic("tokei.mp3");
    
    // shedule
    this->schedule(schedule_selector(HelloWorld::enemyLogic), 30.0);
    
    // shedule
    this->schedule(schedule_selector(HelloWorld::enemyLogicUpdate), 0.5);
    
    return true;
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

bool HelloWorld::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    //タッチが開始された時の処理
    //タッチした座標を取得
    Point location = pTouch->getLocation();
    
    //アニメーションを止める
    eSprite->stopActionByTag(1);
    eSprite->stopActionByTag(2);
    
    //移動距離のx, y座標を求める
    float moveX = eSprite->getPosition().x - location.x;
    float moveY = eSprite->getPosition().y - location.y;
    
    //移動距離の絶対値を求める
    float absMoveX = abs(moveX);
    float absMoveY = abs(moveY);
    
	//移動距離を求める
    float distance = sqrtf(absMoveX + absMoveY);
    
    //移動先の指定距離によって時間を変更する
    MoveTo* move = MoveTo::create(distance / 10, Point(location.x,location.y));
    move->setTag(1);
    
    //アニメーションの作成
    Animation* animation = Animation::create();
    
    if (absMoveX < absMoveY && moveY > 0) {
        // 正面向きの4コマアニメーション
        for (int i = 0; i < 3; i++)
        {
            auto spriteFrameName = StringUtils::format("chara1_bottom_%d.jpg", (i + 1));
            auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
            animation->addSpriteFrame(pFrame);
        }
    } else if (absMoveX < absMoveY && moveY < 0) {
        // 後ろ向きの4コマアニメーション
        for (int i = 0; i < 3; i++)
        {
            auto spriteFrameName = StringUtils::format("chara1_top_%d.jpg", (i + 1));
            auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
            animation->addSpriteFrame(pFrame);
        }
    } else if (absMoveX > absMoveY && moveX > 0) {
        // 左向きの4コマアニメーション
        for (int i = 0; i < 3; i++)
        {
            auto spriteFrameName = StringUtils::format("chara1_left_%d.jpg", (i + 1));
            auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
            animation->addSpriteFrame(pFrame);
        }
    } else {
        // 右向きの4コマアニメーション
        for (int i = 0; i < 3; i++)
        {
            auto spriteFrameName = StringUtils::format("chara1_right_%d.jpg", (i + 1));
            auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
            animation->addSpriteFrame(pFrame);
        }
    }
    
    //アニメーションの設定 1 : 1コマ0.1秒で切り替える。
    animation->setDelayPerUnit(0.1);
    
 	//距離によってアニメーションのループ回数を変更する
    animation->setLoops(distance / 10 * 2.5 + 1);
    
    //アニメーションの設定
    Animate* animate = Animate::create(animation);
    animate->setTag(2);
    
    //アニメーションの実行
    eSprite->runAction(animate);
    eSprite->runAction(move);
    return true;
}

void HelloWorld::onTouchEnded(Touch *pTouch, Event *pEvent)
{
    //タッチが終了した時の処理
}

void HelloWorld::onTouchMoved(Touch *pTouch, cocos2d::Event *pEvent)
{
    //タッチしながら移動した時の処理
}

void HelloWorld::onTouchCancelled(Touch *pTouch, Event *pEvent)
{
    //タッチがキャンセルされた時の処理
}

// enemylogic
void HelloWorld::enemyLogic(float times)
{
    Size enemyPoint = Director::getInstance()->getWinSize();
    int randWidth = rand() % (int)enemyPoint.height;
    int randHeight = rand() % (int)enemyPoint.width;
    
    // enemy
    enemySprite = Sprite::createWithSpriteFrameName("enemy1_bottom_1.jpg");
    
    enemySprite->setPosition(Point(randWidth, randHeight));
    enemySprite->setTag(randWidth);
    this->addChild(enemySprite);
}

// enemylogic
void HelloWorld::enemyLogicUpdate(float times)
{
    //アニメーションを止める
    enemySprite->stopActionByTag(99999);
    enemySprite->stopActionByTag(999991);
    
    // enemy move target player
    float moveX = enemySprite->getPosition().x - eSprite->getPosition().x;
    float moveY = enemySprite->getPosition().y - eSprite->getPosition().y;
    
    float absMoveX = abs(moveX);
    float absMoveY = abs(moveY);
    // 距離
    float distance = sqrtf(absMoveX + absMoveY);
    
    MoveTo* movea = MoveTo::create(distance / 10, Point(eSprite->getPosition().x,eSprite->getPosition().y));
    movea->setTag(99999);
    
    //アニメーションの作成
    Animation* aanimation = Animation::create();
    
    if (absMoveX < absMoveY && moveY > 0) {
        // 正面向きの4コマアニメーション
        for (int i = 0; i < 3; i++)
        {
            auto spriteFrameName = StringUtils::format("enemy1_bottom_%d.jpg", (i + 1));
            auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
            aanimation->addSpriteFrame(pFrame);
        }
    } else if (absMoveX < absMoveY && moveY < 0) {
        // 後ろ向きの4コマアニメーション
        for (int i = 0; i < 3; i++)
        {
            auto spriteFrameName = StringUtils::format("enemy1_top_%d.jpg", (i + 1));
            auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
            aanimation->addSpriteFrame(pFrame);
        }
    } else if (absMoveX > absMoveY && moveX > 0) {
        // 左向きの4コマアニメーション
        for (int i = 0; i < 3; i++)
        {
            auto spriteFrameName = StringUtils::format("enemy1_left_%d.jpg", (i + 1));
            auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
            aanimation->addSpriteFrame(pFrame);
        }
    } else {
        // 右向きの4コマアニメーション
        for (int i = 0; i < 3; i++)
        {
            auto spriteFrameName = StringUtils::format("enemy1_right_%d.jpg", (i + 1));
            auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
            aanimation->addSpriteFrame(pFrame);
        }
    }
    //アニメーションの設定 1 : 1コマ0.1秒で切り替える。
    aanimation->setDelayPerUnit(0.1);
    
 	//距離によってアニメーションのループ回数を変更する
    aanimation->setLoops(distance / 10 * 2.5 + 1);
    
    //アニメーションの設定
    Animate* aanimate = Animate::create(aanimation);
    aanimate->setTag(999991);
    
    //アニメーションの実行
    enemySprite->runAction(aanimate);
    enemySprite->runAction(movea);
}
