#pragma once
#include <DxLib.h>
#include "EnemyBase.h"
class Player;

class NormalEnemy : public EnemyBase
{
public:

	// 状態
	enum class STATE
	{
		NONE,
		THINK,
		IDLE,
		WANDER,
		END
	};

	// アニメーション種別
	enum class ANIM_TYPE
	{
		IDLE = 9,
		WALK = 13,
	};

	// コンストラクタ
	NormalEnemy(const EnemyBase::EnemyData& data, Player* player);

	// デストラクタ
	~NormalEnemy(void) override;

protected:

	// リソースロード
	void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	void InitTransform(void) override;

	// 衝突判定の初期化
	void InitCollider(void) override;

	// アニメーションの初期化
	void InitAnimation(void) override;

	// 初期化後の個別処理
	void InitPost(void) override;

	// 更新系
	void UpdateProcess(void) override;
	void UpdateProcessPost(void) override;

private:

	// モデルの大きさ
	static constexpr float SCALE = 1.0f;

	// モデルのローカル回転
	static constexpr VECTOR ROT = { 0.0f, 180.0f * DX_PI_F / 180.0f, 0.0f };

	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 110.0f, 0.0f };

	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };

	static constexpr VECTOR ENEMY_DEFAULT_POS = { 0.0f, 75.0f, 500.0f };

	// プレイヤーの回転の中心点の初期座標
	static constexpr VECTOR ENEMY_CENTER_DEFAULT_POS = { 0.0f, 75.0f, 500.0f };

	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 80.0f;
	//static constexpr float COL_CAPSULE_RADIUS = 20.0f;

	// コマのスタミナ
	static constexpr float TOPS_DEFAULT_STAMINA = 100.0f;

	// コマの重さ
	static constexpr float TOPS_WEIGHT = 30.0f;

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 15.0f;

	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 30.0f;

	// 相対座標の初期値
	static constexpr VECTOR TOPS_DEFAULT_LOCAL_POS = { 100.0f,0.0f,100.0f };

	// 状態
	STATE state_;

	// 更新ステップ
	float step_;

	void Respawn(void) override;

	// 状態遷移
	void ChangeState(STATE state);
	void ChangeStateNone(void);
	void ChangeStateThink(void);
	void ChangeStateIdle(void);
	void ChangeStateWander(void);
	void ChangeStateEnd(void);

	// 更新系
	void UpdateNone(void);
	void UpdateThink(void);
	void UpdateIdle(void);
	void UpdateWander(void);
	void UpdateEnd(void);

};