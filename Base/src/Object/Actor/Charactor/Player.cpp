#include "../../../Utility/AsoUtility.h"
#include "../../../Utility/MatrixUtility.h"
#include "../../../Manager/ResourceManager.h"
#include "../../../Manager/Resource.h"
#include "../../../Manager/SceneManager.h"
#include "../../../Manager/InputManager.h"
#include "../../../Manager/Camera.h"
#include "../../../Application.h"
#include "../../Collider/ColliderLine.h"
#include "../../Collider/ColliderCapsule.h"
#include "Player.h"

Player::Player(void)
	:
	TopBase(),
	centorMovePow_(AsoUtility::VECTOR_ZERO),
	centorPos_(AsoUtility::VECTOR_ZERO),
	centorRot_(AsoUtility::VECTOR_ZERO),
	centorQuaRot_(Quaternion::Identity()),
	topsSpeed_(0.0f),
	topsStamina_(0.0f)
{
}

Player::~Player(void)
{
}

void Player::Draw(void)
{
	TopBase::Draw();
	DrawDebug();
}

void Player::Release(void)
{
	TopBase::Release();
}

void Player::InitLoad(void)
{
	// 基底クラスのリソースロード
	TopBase::InitLoad();

	transform_.SetModel(resMng_.Load(ResourceManager::SRC::TOP).handleId_);
}

void Player::InitTransform(void)
{
	transform_.scl = PLAYER_DEFAULT_SCALE;

	transform_.quaRot = Quaternion::Identity();

	transform_.quaRotLocal = Quaternion::Identity();
	transform_.quaRotLocal = Quaternion::Euler(PLAYER_DEFAULT_ROT_LOCAL);

	transform_.pos = PLAYER_DEFAULT_POS;
	transform_.Update();
}

void Player::InitCollider(void)
{
	// 主に地面との衝突で使用する線分コライダ
	ColliderLine* colLine = new ColliderLine(
		ColliderBase::TAG::TOP, &transform_,
		COL_LINE_START_LOCAL_POS, COL_LINE_END_LOCAL_POS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::LINE), colLine);

	// 主に壁や木などの衝突で仕様するカプセルコライダ
	ColliderCapsule* colCapsule = new ColliderCapsule(
		ColliderBase::TAG::TOP, &transform_,
		COL_CAPSULE_TOP_LOCAL_POS, COL_CAPSULE_DOWN_LOCAL_POS,
		COL_CAPSULE_RADIUS);
	ownColliders_.emplace(static_cast<int>(COLLIDER_TYPE::CAPSULE), colCapsule);

}

void Player::InitAnimation(void)
{
	animController_ = new AnimationController(transform_.modelId);

	animController_->Add(static_cast<int>(ANIM_TYPE::IDLE), 30.0f,
		Application::PATH_MODEL + "Player/Idle.mv1");

	animController_->Add(static_cast<int>(ANIM_TYPE::RUN), 30.0f,
		Application::PATH_MODEL + "Player/Run.mv1");

	animController_->Add(static_cast<int>(ANIM_TYPE::FAST_RUN), 30.0f,
		Application::PATH_MODEL + "Player/FastRun.mv1");

	animController_->Add(static_cast<int>(ANIM_TYPE::JUMP), 60.0f,
		Application::PATH_MODEL + "Player/JumpRising.mv1");


	animController_->Play(static_cast<int>(ANIM_TYPE::IDLE));
}

void Player::InitPost(void)
{
	isAnim_ = false;

	centorPos_ = PLAYER_ROT_CENTER_POS;

	transform_.localPos = TOPS_DEFAULT_LOCAL_POS;

	topsSpeed_ = SPEED_MOVE;

	topsStamina_ = TOPS_DEFAULT_STAMINA;
}

void Player::UpdateProcess(void)
{
	TopBase::UpdateProcess();

	// 移動操作
	ProcessMove();

	// ジャンプ処理
	ProcessJump();

	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(10.0f), AsoUtility::AXIS_Y));

	centorQuaRot_ = Quaternion::Mult(centorQuaRot_,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(topsSpeed_ *
			topsStamina_ / TOPS_DEFAULT_STAMINA), AsoUtility::AXIS_Y));

	// 行列の合成
	MATRIX selfMat = Quaternion::ToMatrix(Quaternion::Mult(transform_.quaRotLocal, transform_.quaRot));

	// 親（プレイヤーの回転）
	MATRIX parentMat = MatrixUtility::GetMatrixRotateXYZ(
		Quaternion::ToEuler(Quaternion::Mult(centorQuaRot_,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(10.0f), AsoUtility::AXIS_Y))));

	// 行列の合成(子 : 、 親 : プレイヤー)
	MATRIX mat = MatrixUtility::Multiplication(selfMat, parentMat);

	// 行列を使用してモデルの角度を設定
	MV1SetRotationMatrix(transform_.modelId, mat);

	//ローカル座標を親の回転行列で回転
	VECTOR localRotPos_ = VTransform(transform_.localPos, parentMat);

	// ワールド座標
	transform_.pos = VAdd(localRotPos_, { centorPos_.x,transform_.pos.y,centorPos_.z });

	//centorPos_.y = transform_.pos.y;
	topsStamina_ -= scnMng_.GetDeltaTime() * (topsSpeed_ / 15.0f * 10.0f); // スタミナの減少量を調整
	if (topsStamina_ < 0.0f)
	{
		topsStamina_ = 0.0f;
	}

	if (transform_.pos.y < TOPS_DEAD_POS_Y || topsStamina_ <= 0.0f)
	{
		transform_.pos = PLAYER_DEFAULT_POS;
		centorPos_ = PLAYER_ROT_CENTER_POS;
		topsStamina_ = TOPS_DEFAULT_STAMINA;
	}

	transform_.Update();
}

void Player::UpdateProcessPost(void)
{
	TopBase::UpdateProcessPost();
}

void Player::ProcessMove(void)
{
	auto& ins = InputManager::GetInstance();

	VECTOR dir = AsoUtility::VECTOR_ZERO;

	bool isDash = false;

	moveSpeed_ = 0.0f;
	topsSpeed_ = SPEED_MOVE;

	//movePow_ = AsoUtility::VECTOR_ZERO;
	centorMovePow_ = AsoUtility::VECTOR_ZERO;

	// ゲームパッドが接続されている数で処理を分ける
	if (GetJoypadNum() == 0)
	{
		// WASDで移動する
		if (ins.IsNew(KEY_INPUT_W)) { dir = AsoUtility::DIR_F; }
		if (ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_L; }
		if (ins.IsNew(KEY_INPUT_S)) { dir = AsoUtility::DIR_B; }
		if (ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_R; }

		if (ins.IsNew(KEY_INPUT_W) && ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_FL; }
		if (ins.IsNew(KEY_INPUT_W) && ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_FR; }
		if (ins.IsNew(KEY_INPUT_S) && ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_BL; }
		if (ins.IsNew(KEY_INPUT_S) && ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_BR; }

		if (ins.IsNew(KEY_INPUT_RSHIFT)) { isDash = true; }

	}
	else
	{
		// 接続されているゲームパッド１の情報を取得
		InputManager::JOYPAD_IN_STATE padState =
			ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

		// アナログキーの入力値から方向を取得
		dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

		if (ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::R_TRIGGER))
		{
			isDash = true;
		}

	}

	if (isDash)
	{
		moveSpeed_ = SPEED_DASH;
		topsSpeed_ = SPEED_DASH;
	}

	if (!AsoUtility::EqualsVZero(dir))
	{
		// 移動スピード
		moveSpeed_ = SPEED_MOVE;

		// ジャンプ中はアニメーションを変えない
		if (!isJump_)
		{
			// アニメーション
			if (isDash)
			{
				animController_->Play(
					static_cast<int>(ANIM_TYPE::FAST_RUN), true);
			}
			else
			{
				animController_->Play(
					static_cast<int>(ANIM_TYPE::RUN), true);
			}
		}

		// Y軸のみのカメラ角度を取得
		Quaternion cameraRot = scnMng_.GetCamera()->GetQuaRotY();

		// 移動方向をカメラに合わせる
		moveDir_ = Quaternion::PosAxis(cameraRot, dir);

		// 移動量を計算
		//movePow_ = VScale(moveDir_, moveSpeed_);
		centorMovePow_ = VScale(moveDir_, moveSpeed_ * (topsStamina_ / TOPS_DEFAULT_STAMINA));

		// 移動処理
		centorPos_ = VAdd(centorPos_, centorMovePow_);
		//transform_.pos = VAdd(transform_.pos, movePow_);
	}
	else
	{
		// ジャンプ中はアニメーションを変えない
		if (!isJump_)
		{
			// IDLE状態に戻す
			animController_->Play(
				static_cast<int>(ANIM_TYPE::IDLE), true);

		}
	}
}

void Player::ProcessJump(void)
{
	auto& ins = InputManager::GetInstance();
	// 持続ジャンプ処理
	bool isHitKeyNew = ins.IsNew(KEY_INPUT_BACKSLASH)
		|| ins.IsPadBtnNew(
			InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
	if (isHitKeyNew)
	{
		// ジャンプの入力受付時間を減少
		stepJump_ += scnMng_.GetDeltaTime();
		if (stepJump_ < TIME_JUMP_INPUT)
		{
			// ジャンプ量の計算
			float jumpSpeed = POW_JUMP_KEEP * scnMng_.GetDeltaTime();
			jumpPow_ = VAdd(jumpPow_, VScale(AsoUtility::DIR_U, jumpSpeed));
		}
	}
	else
	{
		// ボタンを離したらジャンプ力に加算しない
		return;
	}

	// 初期ジャンプ処理
	bool isHitKey = ins.IsTrgDown(KEY_INPUT_BACKSLASH)
		|| ins.IsPadBtnTrgDown(
			InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);

	// ジャンプ
	if (isHitKey && !isJump_)
	{
		// ジャンプ量の計算
		float jumpSpeed = POW_JUMP_INIT * scnMng_.GetDeltaTime();
		jumpPow_ = VScale(AsoUtility::DIR_U, jumpSpeed);
		isJump_ = true;

		// アニメーション再生
		/*animController_->Play(
			static_cast<int>(ANIM_TYPE::JUMP), false);*/
	}
}

void Player::ProcessAnimPos(void)
{
	TopBase::ProcessAnimPos();
}

void Player::ProcessAnimCapsule(void)
{
	TopBase::ProcessAnimCapsule();
}

void Player::CollisionReserve(void)

{
	TopBase::CollisionReserve();
}

void Player::DrawDebug(void)
{
	DrawFormatString(15, 20, 0x000000, "コマの位置:,%f,%f,%f", transform_.pos.x, transform_.pos.y, transform_.pos.z);
	DrawFormatString(15, 40, 0x000000, "回転の中心:,%f,%f,%f", centorPos_.x, centorPos_.y, centorPos_.z);
	DrawFormatString(15, 60, 0x000000, "スタミナ:,%f", topsStamina_);
	DrawSphere3D(centorPos_, 30.0f, 16, 0xFF0000, 0xFF0000, true);
}
