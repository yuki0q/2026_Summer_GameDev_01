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

	VECTOR contactPoint_;

	int shadowMapHandle_;
	int countTime_;
	bool isStart_;
	bool isEnd_;

	int image3;
	int image2;
	int image1;
	int imgWin_;

	// プレイヤーのポイント
	int playerScore_ = 0;   

	// エネミーのポイント
	int enemyScore_ = 0;    

	// このラウンドのポイント加算が済んだか
	bool isRoundProcessed_; 

	// ラウンドの管理フラグ
	bool isRoundEnd_;
	int lastRoundWinner_;

	int playerCount_;

	// 演出用変数
	float timeScale_;          // 時間の進む速さ（1.0 = 通常, 0.1 = 10%のスローなど）
	float slowMotionTimer_;    // スローモーションの残り時間

	// ステージ外を判定するための外周半径
	static constexpr float STAGE_OUT_RADIUS = 1600.0f;
};
