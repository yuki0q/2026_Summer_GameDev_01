#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"
class NormalStage;
class Player;
class EnemyManager;
class TopBase;

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

	void ResolveTopToTop(TopBase* topA, TopBase* topB);

private:

	NormalStage* normalStage_;
	// 1P
	Player* player_;
	// 2P
	Player* player2_;        
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

	// 1 または 2 (SceneManagerなどから取得するか、デバッグ用に設定)
	int playerCount_;   
};
