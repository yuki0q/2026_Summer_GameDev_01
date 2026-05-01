#pragma once
#include "CharactorBase.h"

class TopBase : public CharactorBase
{

public:

	enum class ANIM_TYPE
	{
		IDLE,
		RUN,
		FAST_RUN,
		JUMP,
		//MAX,
	};

	TopBase(void);
	virtual ~TopBase(void);

	// 描画
	virtual void Draw(void)override;

	// 解放
	virtual void Release(void)override;

protected:

	// リソースロード
	virtual void InitLoad(void) override;

	// 大きさ、回転、座標の初期化
	virtual void InitTransform(void) override;

	// 衝突判定の初期化
	virtual void InitCollider(void) override;

	// アニメーションの初期化
	virtual void InitAnimation(void) override;

	// 初期化後の個別処理
	virtual void InitPost(void) override;

	// 更新系
	virtual void UpdateProcess(void) override;

	virtual void UpdateProcessPost(void) override;

	static constexpr VECTOR PLAYER_DEFAULT_POS = { 0.0f, 75.0f, 0.0f };

	static constexpr VECTOR PLAYER_DEFAULT_SCALE = { 1.0f,1.0f,1.0f };

	static constexpr VECTOR PLAYER_DEFAULT_ROT_LOCAL = { 0.0f,180.0f * DX_PI_F / 180.0f,0.0f };

	// 移動速度(通常)
	static constexpr float SPEED_MOVE = 15.0f;

	// 移動速度(ダッシュ)
	static constexpr float SPEED_DASH = 30.0f;

	// 衝突判定用線分開始
	static constexpr VECTOR COL_LINE_START_LOCAL_POS = { 0.0f, 80.0f, 0.0f };

	// 衝突判定用線分終了
	static constexpr VECTOR COL_LINE_END_LOCAL_POS = { 0.0f, -10.0f, 0.0f };

	// 衝突判定用線分開始(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_START_LOCAL_POS = { 0.0f, 130.0f, 0.0f };

	// 衝突判定用線分終了(ジャンプ時)
	static constexpr VECTOR COL_LINE_JUMP_END_LOCAL_POS = { 0.0f, 50.0f, 0.0f };

	// ジャンプ力
	static constexpr float POW_JUMP_INIT = 3500.0f;

	// 持続ジャンプ力
	static constexpr float POW_JUMP_KEEP = 400.0f;

	// ジャンプ受付時間
	static constexpr float TIME_JUMP_INPUT = 0.5f;

	// 衝突判定用カプセル上部球体
	static constexpr VECTOR COL_CAPSULE_TOP_LOCAL_POS = { 0.0f, 100.0f, 0.0f };

	// 衝突判定用カプセル下部球体
	static constexpr VECTOR COL_CAPSULE_DOWN_LOCAL_POS = { 0.0f, 30.0f, 0.0f };

	// 衝突判定用カプセル球体半径
	static constexpr float COL_CAPSULE_RADIUS = 20.0f;

	// プレイヤーの回転の中心点の初期座標
	static constexpr VECTOR PLAYER_ROT_CENTER_POS = { 0.0f, 75.0f, 0.0f };

	static constexpr VECTOR TOPS_DEFAULT_LOCAL_POS = { 100.0f,0.0f,100.0f };

	static constexpr float TOPS_DEFAULT_STAMINA = 100.0f;

	static constexpr float TOPS_DEAD_POS_Y = -300.0f;

	// 操作
	virtual void ProcessMove(void);
	virtual void ProcessJump(void);
	virtual void ProcessAnimPos(void);
	virtual	void ProcessAnimCapsule(void);

	// 衝突判定
	virtual void CollisionReserve(void) override;

	// デバッグ描画
	virtual void DrawDebug(void);

	float topsSpeed_;
	float topsStamina_;

	//	コマの回転の中心点座標
	VECTOR centorPos_;
	VECTOR centorRot_;

	VECTOR centorMovePow_;

	Quaternion centorQuaRot_;

private:

	
};