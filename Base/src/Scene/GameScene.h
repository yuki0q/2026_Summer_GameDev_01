#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
class NormalStage;
class Player;
class EnemyManager;

class GameScene : public SceneBase
{

public:

	// コンストラクタ
	GameScene(void);

	// デストラクタ
	~GameScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

	void Collision(void);

	void CollisionResolve(void);

private:

	NormalStage* normalStage_;
	Player* player_;
	EnemyManager* enemyManager_;

	int shadowMapHandle_;
	int countTime_;
	bool isStart_;
	bool isEnd_;

	int image3;
	int image2;
	int image1;

	// プレイヤーのポイント
	int playerScore_ = 0;   

	// エネミーのポイント
	int enemyScore_ = 0;    

	// このラウンドのポイント加算が済んだか
	bool isRoundProcessed_; 

	// ラウンドの管理フラグ
	bool isRoundEnd_;
};
