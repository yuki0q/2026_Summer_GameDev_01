#include "../../../../Utility/AsoUtility.h"
#include "../../../../Manager/ResourceManager.h"
#include "../../../../Manager/Resource.h"
#include "../../../../Manager/SceneManager.h"
#include "../../../../Manager/InputManager.h"
#include "../../../../Manager/Camera.h"
#include "../../../../Application.h"
#include "../../../Collider/ColliderLine.h"
#include "../../../Collider/ColliderCapsule.h"
#include "../Player.h"
#include "NormalEnemy.h"

NormalEnemy::NormalEnemy(const TopBase::TopData& data, Player* player)
	:
	EnemyBase(data, player),
	state_(STATE::NONE),
	step_(0.0f)
{
	topType_ = data.type;
}

NormalEnemy::~NormalEnemy(void)
{
}

void NormalEnemy::InitLoad(void)
{
	//topType_ = TOP_TYPE::STAMINA;
	/*topType_ = static_cast<TOP_TYPE>(GetRand(3));*/

	// 基底クラスのリソースロード
	TopBase::InitLoad();

	imgChara_ = resMng_.Load(ResourceManager::SRC::IMAGE_CPU).handleId_;
}

void NormalEnemy::InitTransform(void)
{
	respawnCenterPos_ = ENEMY_CENTER_DEFAULT_POS;

	respawnPos_ = ENEMY_DEFAULT_POS;

	transform_.scl = VScale(AsoUtility::VECTOR_ONE, SCALE);
	transform_.quaRot = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Euler(ROT);
	transform_.pos = respawnPos_;
	transform_.localPos = TOPS_DEFAULT_LOCAL_POS;
	transform_.Update();

	centerPos_ = respawnCenterPos_;

	topsMovement_ = topsSpeed_ = 0.0f;

	topsSpeed_ = topsMovement_ = 0.0f;

	topsRadius_ = COL_CAPSULE_RADIUS;

	topsVel_ = { 0.0f,0.0f,0.0f };

	prevPos_ = { 0.0f,0.0f,0.0f };

	/*topsSpin_ = topsSpinMax_ = TOPS_SPIN_MAX;
	topsWeight_ = TOPS_WEIGHT;
	topsShock_ = TOPS_SHOCK;*/
}

void NormalEnemy::InitCollider(void)
{
	// 主に地面との衝突で使用する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::ENEMY, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);
}

void NormalEnemy::InitAnimation(void)
{
	animController_ = new AnimationController(transform_.modelId);

	int idleType = -1;
	int walkType = -1;

	idleType = static_cast<int>(ANIM_TYPE::IDLE);
	walkType = static_cast<int>(ANIM_TYPE::WALK);

	animController_->AddInFbx(idleType, 20.0f, idleType);
	animController_->AddInFbx(walkType, 20.0f, walkType);

	animController_->Play(idleType, true);
}

void NormalEnemy::InitPost(void)
{
	TopBase::InitPost();

	// 状態遷移初期処理登録
	stateChanges_.emplace(static_cast<int>(STATE::NONE),
		std::bind(&NormalEnemy::ChangeStateNone, this));
	stateChanges_.emplace(static_cast<int>(STATE::THINK),
		std::bind(&NormalEnemy::ChangeStateThink, this));
	stateChanges_.emplace(static_cast<int>(STATE::IDLE),
		std::bind(&NormalEnemy::ChangeStateIdle, this));
	stateChanges_.emplace(static_cast<int>(STATE::WANDER),
		std::bind(&NormalEnemy::ChangeStateWander, this));
	stateChanges_.emplace(static_cast<int>(STATE::END),
		std::bind(&NormalEnemy::ChangeStateEnd, this));

	ChangeState(STATE::THINK);
}

void NormalEnemy::UpdateProcess(void)
{
	TopBase::UpdateProcess();

	if (isDying_)
	{
		return;
	}

	switch (state_)
	{
	case STATE::NONE:
		UpdateNone();
		break;
	case STATE::THINK:
		UpdateThink();
		break;
	case STATE::IDLE:
		UpdateIdle();
		break;
	case STATE::WANDER:
		UpdateWander();
		break;
	case STATE::END:
		UpdateEnd();
		break;
	}

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	bool isDash = false;
	float accele = 0.3f;
	moveSpeed_ = 0.0f;
	topsSpeed_ = SPEED_MOVE;
	centerMovePow_ = AsoUtility::VECTOR_ZERO;
}

void NormalEnemy::UpdateProcessPost(void)
{
	TopBase::UpdateProcessPost();

	if (!InMovableRange())
	{
	//	ChangeState(STATE::THINK);
	}


}

void NormalEnemy::ProcessMove(void)
{
	// 状態別更新
	stateUpdate_();
}

void NormalEnemy::Draw(void)
{
	EnemyBase::Draw();
	// ゲージの基本サイズ
	const int GAUGE_WIDTH = 360;
	const int GAUGE_HEIGHT = 100;

	// 現在のスタミナの割合 (0.0f ～ 1.0f)
	float spinRatio = topsSpin_ / topsSpinMax_;
	if (spinRatio < 0.0f) spinRatio = 0.0f;
	if (spinRatio > 1.0f) spinRatio = 1.0f;

	// 割合に応じた描画幅
	int barDrawWidth = static_cast<int>(GAUGE_WIDTH * spinRatio);

	// ゲージ描画位置
	int bgX = Application::SCREEN_SIZE_X - GAUGE_WIDTH - 30;
	int bgY = Application::SCREEN_SIZE_Y - 150;

	// ゲージの枠を描画
	DrawGraph(bgX, bgY, gaugeFrame, TRUE);

	// ゲージの中身を描画
	DrawRectGraph(bgX, bgY,
		0, 0, barDrawWidth, GAUGE_HEIGHT,
		spinGauge, TRUE);

	if (skillCoolTimer_ <= 0.0f && !isSkill_) {
		DrawFormatString(980, 670, 0xffffff, "Skill Ready!!");
	}
}

void NormalEnemy::Respawn(void)
{
	TopBase::Respawn();

	transform_.pos = respawnPos_;
	centerPos_ = respawnCenterPos_;

	ChangeState(STATE::THINK);
	step_ = 0.0f;
}

void NormalEnemy::ChangeState(STATE state)
{
	state_ = state;

	// マップに登録した初期化関数を呼ぶだけで十分（switch文は削除する）
	if (stateChanges_.count(static_cast<int>(state_)))
	{
		stateChanges_[static_cast<int>(state_)]();
	}
}

void NormalEnemy::ChangeStateNone(void)
{
	stateUpdate_ = std::bind(&NormalEnemy::UpdateNone, this);
}

void NormalEnemy::ChangeStateThink(void)
{
	stateUpdate_ = std::bind(&NormalEnemy::UpdateThink, this);

	// 思考
	// ランダムに次の行動を決定
	// 30%で待機、70%で徘徊
	/*int rand = GetRand(100);

	if (rand < 30)
	{
		isSkill_ = true;
	}
	else
	{
		ChangeState(STATE::WANDER);
	}*/
}

void NormalEnemy::ChangeStateIdle(void)
{
	stateUpdate_ = std::bind(&NormalEnemy::UpdateIdle, this);

	// ランダムな待機時間
	step_ = 3.0f + static_cast<float>(GetRand(3));

	// 移動量ゼロ
	movePow_ = AsoUtility::VECTOR_ZERO;

	// 待機アニメーション再生
	animController_->Play(
		static_cast<int>(ANIM_TYPE::IDLE), true);
}

void NormalEnemy::ChangeStateWander(void)
{
	stateUpdate_ = std::bind(&NormalEnemy::UpdateWander, this);

	// ランダムな角度
	float angle = static_cast<float>(GetRand(360)) * DX_PI_F / 180.0f;

	// 移動方向
	moveDir_ = VGet(cosf(angle), 0.0f, sinf(angle));

	// ランダムな移動時間
	//step_ = 100.0f + static_cast<float>(GetRand(5));
	step_ = (3.0f + GetRand(3));

	// 移動スピード
	//topsSpeed_ = 3.0f;


	// 歩きアニメーション再生
	animController_->Play(
		static_cast<int>(ANIM_TYPE::WALK), true);
}

void NormalEnemy::ChangeStateEnd(void)
{
	stateUpdate_ = std::bind(&NormalEnemy::UpdateEnd, this);
}

void NormalEnemy::UpdateNone(void)
{
}

void NormalEnemy::UpdateThink(void)
{
	if (GetCollisionTarget_()) { 
		ChangeState(STATE::WANDER); }

	// Think時は中心にとどまる
		// XZ成分のみで距離を計算
		float dx = 0.0f - centerPos_.x;
		float dz = 0.0f - centerPos_.z;
		/*VECTOR toTarget = VSub(AsoUtility::VECTOR_ZERO, centerPos_);
		float distXZ = sqrtf(dx * dx + dz * dz);*/
		VECTOR toTarget = VGet(0.0f - centerPos_.x, 0.0f, 0.0f - centerPos_.z);
		float distXZ = VSize(toTarget);

		if (topType_ == TOP_TYPE::DEFENSE || topType_ == TOP_TYPE::STAMINA) {
			// 到達判定（誤差以内なら完了）
			if (distXZ < 5.0f)
			{
				// XZのみ目標座標に合わせる（Yは触らない）
				centerPos_.x = 0.0f;
				centerPos_.z = 0.0f;

				int rand = GetRand(100);

				if (rand < 30)
				{
					topsSpeed_ = SPEED_MOVE;
					isSkill_ = true;
				}
				else
				{
					isSkill_ = false;
				}

				//isSkill_ = true;
			}
			else
			{
				//// 補間速度（大きいほど素早く移動）
				//float lerpSpeed = 0.3f;
				//centerPos_ = VAdd(centerPos_, VScale(toTarget, lerpSpeed));

				// 中心への方向ベクトルを正規化
				VECTOR moveDir = VNorm(toTarget);

				// スタミナ（スピン量）の割合に応じた移動量を計算
				float spinRatio = topsSpin_ / TOPS_SPIN_MAX;
				//float spinRatio = 2.0f;
				centerMovePow_ = VScale(moveDir, topsSpeed_ * spinRatio);

				// centerPos_ を物理移動
				centerPos_ = VAdd(centerPos_, centerMovePow_);
			}
		}
		else {
			VECTOR toTarget = VGet(player_->GetTransform().pos.x - centerPos_.x, 
				0.0f, 
				player_->GetTransform().pos.z - centerPos_.z);

			if (distXZ < 2.0f)
			{
				// XZのみ目標座標に合わせる（Yは触らない）
				centerPos_.x = 0.0f;
				centerPos_.z = 0.0f;
			}
			else
			{

				//// 補間速度（大きいほど素早く移動）
				//float lerpSpeed = 0.3f;
				//centerPos_ = VAdd(centerPos_, VScale(toTarget, lerpSpeed));

				// 中心への方向ベクトルを正規化
				VECTOR moveDir = VNorm(toTarget);

				// スタミナ（スピン量）の割合に応じた移動量を計算
				float spinRatio = topsSpin_ / TOPS_SPIN_MAX;
				//float spinRatio = 2.0f;
				centerMovePow_ = VScale(moveDir, topsSpeed_ * spinRatio);

				// centerPos_ を物理移動
				centerPos_ = VAdd(centerPos_, centerMovePow_);

				int rand = GetRand(100);

				if (rand < 30)
				{
					topsSpeed_ = SPEED_DASH;
					isSkill_ = true;
				}
				else
				{
					isSkill_ = false;
				}
			}
		}
}

void NormalEnemy::UpdateIdle(void)
{
	if (step_ < 0.0f) {
		ChangeState(STATE::THINK);
	}

	step_--;
}

void NormalEnemy::UpdateWander(void)
{
	if (step_ < 0.0f) {
		ChangeState(STATE::THINK);
	}

	if (GetCollisionTarget_()) {
		step_ += 1.0f;
	}
	//movePow_ = VScale(moveDir_, moveSpeed_);

	step_--;
}

void NormalEnemy::UpdateEnd(void)
{
}
