#pragma once
#include "SceneBase.h"
#include "../Common/Vector2.h"

class Camera;
class Stage;

class GameScene	: public SceneBase
{
public:

	// コンストラクタ
	GameScene(void);
	// デストラクタ
	~GameScene(void)override;

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:

	// BGM 
	int bgmBattle_;

	// 画像
	int imgBattleBack_;
};

