#pragma once
#include "CharactorBase.h"
#include <vector>

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

	// コマのタイプ定義
	enum class TOP_TYPE
	{
		ATTACK,   // 攻撃型：激しく傾きながら暴れる
		DEFENSE,  // 防御型：どっしりしてブレにくい
		STAMINA,  // 持久型：軸がブレず超安定
		BALANCE,  // バランス型:すべての中間
	};

	TopBase(void);
	virtual ~TopBase(void);

	// 描画
	virtual void Draw(void)override;

	// 解放
	virtual void Release(void)override;

	float GetRadius(void);

	float GetSpin(void);
	void SpinScrape(float spin);

	float GetWeight(void);

	VECTOR GetVel(void);
	void SetVel(VECTOR vel);

	VECTOR GetPos(void);
	void SetPos(VECTOR pos);

	bool GetCollisionTarget_(void);

	// 衝突による傾き（外部から加算できるように）
	void AddCollisionTilt(VECTOR impulseDir, float impulseMag);

	// ステージ衝突による傾き適用
	void ApplyStageTilt(VECTOR hitNormal, float impactSpeed) override;

	// 目標座標を設定（GameScene から呼ぶ）
	void SetCollisionTarget(VECTOR targetPos);

	// リスポーン中（衝突無視中）かどうかを取得する
	bool IsRespawning(void) const { return isRespawning_; }

	bool IsGameEnd(void)const { return isEnd_ && !isDying_;}

	bool IsDying(void) const { return isDying_; }

	void DrawImage(void);

	float GetTopsShock(void)const { return topsShock_; }

	// 外部からタイプを設定・取得
	void SetTopType(TOP_TYPE type) { topType_ = type; }
	TOP_TYPE GetTopType(void) const { return topType_; }

	bool IsShielding(void) { return isShielding_; }

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

	static constexpr float TOPS_DEAD_POS_Y = -300.0f;

	// 衝突を無視する時間（秒数。1.0fで1秒間）
	static constexpr float RESPAWN_MUTE_TIME = 1.0f;

	// 調整用定数
	// 何秒ごとに点を打つか（小さいほど滑らか）
	static constexpr float TRAIL_INTERVAL = 0.02f;
	// 軌跡を残す最大の長さ（個数）
	static constexpr size_t TRAIL_MAX_POINTS = 50;
	// 軌跡（帯）の横幅
	static constexpr float TRAIL_WIDTH = 25.0f;

	static constexpr float SKILL_COOL_TIME_A = 2.0f; // クールタイムの長さ
	static constexpr float SKILL_COOL_TIME_D = 5.0f; // クールタイムの長さ
	static constexpr float SKILL_COOL_TIME_S = 10.0f; // クールタイムの長さ
	static constexpr float SKILL_COOL_TIME_B = 2.0f; // クールタイムの長さ

	// 操作
	virtual void ProcessMove(void);
	virtual void ProcessJump(void);
	virtual void ProcessTopMove(void);
	virtual void ProcessAnimPos(void);
	virtual	void ProcessAnimCapsule(void);

	// 衝突判定
	virtual void CollisionReserve(void) override;

	virtual void Respawn(void);

	// デバッグ描画
	virtual void DrawDebug(void);

	void TrailDraw(void);

	float VSizeSq(const VECTOR& v);

	// コマの傾き
	void ProcessTopTilt(void);

	// コマの軌跡
	void ProcessTopTrail(void);

	void TopSorting(void);

	void ProccesTypeAttack(void);
	void ProccesTypeDefense(void);
	void ProccesTypeStamina(void);
	void ProccesTypeBalance(void);

	// シーン管理
	SceneManager& sceMng_;

	// 衝突による目標座標
	VECTOR collisionTargetPos_;

	int imgChara_;

	// 衝突目標座標が有効かどうか
	bool hasCollisionTarget_;
	bool isHit_;

	float topsSpeed_;
	// コマのスタミナ
	float topsSpin_;

	// コマの公転半径
	float radiusFactor_;

	// スタミナの削れる速度
	float scrapSpeed_;

	// スタミナが15以下の時の削れる速度
	float dyingScrapSpeed_;

	// コマの重さ
	float topsWeight_;
	float topsRadius_;
	float topsMovement_;

	float tiltX_;
	float tiltZ_;
	// 傾きの軸角度（ラジアン）
	float tiltAngle_;      // 傾きの大きさ
	float tiltPhase_;      // 歳差運動の位相

	// 傾き目標値（衝突由来）
	float collisionTiltX_;
	float collisionTiltZ_;

	VECTOR respawnPos_;
	VECTOR respawnCenterPos_;

	VECTOR topsVel_;

	// コマの前座標
	VECTOR prevPos_;

	//	コマの回転の中心点座標
	VECTOR centerPos_;
	VECTOR centerRot_;

	VECTOR centerMovePow_;

	Quaternion centerQuaRot_;

	// コマごとの最大スピン
	float topsSpinMax_;

	// コマごとの衝突の強さ
	float topsShock_;

	bool isEnd_;
	bool isRespawning_;       // リスポーン直後フラグ
	float respawnTimer_;       // 無敵・衝突無視のタイマー数

	bool isDying_;       // 倒れ中フラグ
	float dyingTimer_;    // 倒れ始めてからの経過時間

	// 攻撃型スキル時
	bool isDashing_;
	// 防御型スキル時
	bool isShielding_;
	// スタミナ型スキル時
	bool isStabilitying_;
	// バランス型スキル時
	bool isBlancing_;

	// EXスキルの管理フラグ
	bool isExSkill_;
	// スキルの管理フラグ
	bool isSkill_;
	// スキルの管理タイマー
	float skillTimer_;
	// スキルのクールタイマー
	float skillCoolTimer_;     

	// スキル使用時のスピード
	float skillSpeed_;

	// 軌跡の1点ごとのデータ
	struct TrailPoint {
		VECTOR pos;       // 座標
		float alpha;      // 透明度
	};

	int dyeCount_;
	int trailColorF_;
	int trailColorE_;

	std::vector<TrailPoint> trailPoints_; // 軌跡データの配列
	float trailTimer_;             // 軌跡を追加する周期タイマー

	TOP_TYPE topType_;

	// タイプごとの挙動調整用パラメータ
	float stability_ ;    // 軸の安定度（高いほどブレがすぐ収まる）
	float defaultTilt_;  // 平常時の傾き（攻撃型などは最初から少し傾ける）
	float wobbleSpeed_;  // ヨロヨロと円を描く速度（歳差運動の速さ）
};