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

NormalEnemy::NormalEnemy(const EnemyBase::EnemyData& data, Player* player)
	:
	EnemyBase(data, player),
	state_(STATE::NONE),
	step_(0.0f)
{
}

NormalEnemy::~NormalEnemy(void)
{
}

void NormalEnemy::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();

	transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::RED_TOP));
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

	topsSpin_ = TOPS_DEFAULT_STAMINA;

	topsWeight_ = TOPS_WEIGHT;

	topsSpeed_ = topsMovement_ = 0.0f;

	topsRadius_ = COL_CAPSULE_RADIUS;

	topsVel_ = { 0.0f,0.0f,0.0f };

	prevPos_ = { 0.0f,0.0f,0.0f };
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
	/*switch (state_)
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
	}*/

	// 状態別更新
	stateUpdate_();

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

	//if (!InMovableRange())
	//{
	//	transform_.pos = prevPos_;

	//	// モデル制御更新
	//	transform_.Update();

	//	ChangeState(STATE::THINK);
	//}


}

void NormalEnemy::ChangeState(STATE state)
{
	state_ = state;

	// 各状態遷移の初期処理
	stateChanges_[static_cast<int>(state_)]();

	/*switch (state_)
	{
	case STATE::NONE:
		ChangeStateNone();
		break;
	case STATE::THINK:
		ChangeStateThink();
		break;
	case STATE::IDLE:
		ChangeStateIdle();
		break;
	case STATE::WANDER:
		ChangeStateWander();
		break;
	case STATE::END:
		ChangeStateEnd();
		break;
	}*/
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
	int rand = GetRand(100);

	if (rand < 30)
	{
		ChangeState(STATE::IDLE);
	}
	else
	{
		ChangeState(STATE::WANDER);
	}
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
	step_ = 2.0f + static_cast<float>(GetRand(5));

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

	movePow_ = VScale(moveDir_, moveSpeed_);

	step_--;
}

void NormalEnemy::UpdateEnd(void)
{
}
