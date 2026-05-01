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
#include "TopBase.h"

TopBase::TopBase(void)
	:
	CharactorBase(),
	centorMovePow_(AsoUtility::VECTOR_ZERO),
	centorPos_(AsoUtility::VECTOR_ZERO),
	centorRot_(AsoUtility::VECTOR_ZERO),
	centorQuaRot_(Quaternion::Identity()),
	topsSpeed_(0.0f),
	topsStamina_(0.0f)
{
}

TopBase::~TopBase(void)
{
}

void TopBase::Draw(void)
{
	CharactorBase::Draw();
	DrawDebug();
}

void TopBase::Release(void)
{
	CharactorBase::Release();
}

void TopBase::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();
}

void TopBase::InitTransform(void)
{
}

void TopBase::InitCollider(void)
{
}

void TopBase::InitAnimation(void)
{
}

void TopBase::InitPost(void)
{
}

void TopBase::UpdateProcess(void)
{
}

void TopBase::UpdateProcessPost(void)
{
}

void TopBase::ProcessMove(void)
{
	//auto& ins = InputManager::GetInstance();

	//VECTOR dir = AsoUtility::VECTOR_ZERO;

	//bool isDash = false;

	//moveSpeed_ = 0.0f;
	//topsSpeed_ = SPEED_MOVE;

	////movePow_ = AsoUtility::VECTOR_ZERO;
	//centorMovePow_ = AsoUtility::VECTOR_ZERO;

	//// ゲームパッドが接続されている数で処理を分ける
	//if (GetJoypadNum() == 0)
	//{
	//	// WASDで移動する
	//	if (ins.IsNew(KEY_INPUT_W)) { dir = AsoUtility::DIR_F; }
	//	if (ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_L; }
	//	if (ins.IsNew(KEY_INPUT_S)) { dir = AsoUtility::DIR_B; }
	//	if (ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_R; }

	//	if (ins.IsNew(KEY_INPUT_W) && ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_FL; }
	//	if (ins.IsNew(KEY_INPUT_W) && ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_FR; }
	//	if (ins.IsNew(KEY_INPUT_S) && ins.IsNew(KEY_INPUT_A)) { dir = AsoUtility::DIR_BL; }
	//	if (ins.IsNew(KEY_INPUT_S) && ins.IsNew(KEY_INPUT_D)) { dir = AsoUtility::DIR_BR; }

	//	if (ins.IsNew(KEY_INPUT_RSHIFT)) { isDash = true; }

	//}
	//else
	//{
	//	// 接続されているゲームパッド１の情報を取得
	//	InputManager::JOYPAD_IN_STATE padState =
	//		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	//	// アナログキーの入力値から方向を取得
	//	dir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	//	if (ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
	//		InputManager::JOYPAD_BTN::R_TRIGGER))
	//	{
	//		isDash = true;
	//	}

	//}

	//if (isDash)
	//{
	//	moveSpeed_ = SPEED_DASH;
	//	topsSpeed_ = SPEED_DASH;
	//}

	//if (!AsoUtility::EqualsVZero(dir))
	//{
	//	// 移動スピード
	//	moveSpeed_ = SPEED_MOVE;

	//	// ジャンプ中はアニメーションを変えない
	//	if (!isJump_)
	//	{
	//		// アニメーション
	//		if (isDash)
	//		{
	//			animController_->Play(
	//				static_cast<int>(ANIM_TYPE::FAST_RUN), true);
	//		}
	//		else
	//		{
	//			animController_->Play(
	//				static_cast<int>(ANIM_TYPE::RUN), true);
	//		}
	//	}

	//	// Y軸のみのカメラ角度を取得
	//	Quaternion cameraRot = scnMng_.GetCamera()->GetQuaRotY();

	//	// 移動方向をカメラに合わせる
	//	moveDir_ = Quaternion::PosAxis(cameraRot, dir);

	//	// 移動量を計算
	//	//movePow_ = VScale(moveDir_, moveSpeed_);
	//	centorMovePow_ = VScale(moveDir_, moveSpeed_ * (topsStamina_ / TOPS_DEFAULT_STAMINA));

	//	// 移動処理
	//	centorPos_ = VAdd(centorPos_, centorMovePow_);
	//	//transform_.pos = VAdd(transform_.pos, movePow_);
	//}
	//else
	//{
	//	// ジャンプ中はアニメーションを変えない
	//	if (!isJump_)
	//	{
	//		// IDLE状態に戻す
	//		animController_->Play(
	//			static_cast<int>(ANIM_TYPE::IDLE), true);

	//	}
	//}
}

void TopBase::ProcessJump(void)
{
}

void TopBase::ProcessAnimPos(void)
{
	// アニメーションごとの線分調整
	if (animController_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP))
	{
		// ジャンプ中は線分を伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_JUMP_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_JUMP_END_LOCAL_POS);
		}
	}
	else
	{
		// 通常時の線分に戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::LINE)) != 0)
		{
			ColliderLine* colLine = dynamic_cast<ColliderLine*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::LINE)));
			colLine->SetLocalPosStart(COL_LINE_START_LOCAL_POS);
			colLine->SetLocalPosEnd(COL_LINE_END_LOCAL_POS);
		}
	}
}

void TopBase::ProcessAnimCapsule(void)
{
	// アニメーションごとの線分調整
	if (animController_->GetPlayType() == static_cast<int>(ANIM_TYPE::JUMP))
	{
		// ジャンプ中は線分を伸ばす
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			/*ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_JUMP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_JUMP_LOCAL_POS);*/
		}
	}
	else
	{
		// 通常時の線分に戻す
		if (ownColliders_.count(static_cast<int>(COLLIDER_TYPE::CAPSULE)) != 0)
		{
			ColliderCapsule* colCapsule = dynamic_cast<ColliderCapsule*>(
				ownColliders_.at(static_cast<int>(COLLIDER_TYPE::CAPSULE)));
			colCapsule->SetLocalPosTop(COL_CAPSULE_TOP_LOCAL_POS);
			colCapsule->SetLocalPosDown(COL_CAPSULE_DOWN_LOCAL_POS);
		}
	}
}

void TopBase::CollisionReserve(void)

{
	// アニメーションごとの位置調整
	ProcessAnimPos();

	ProcessAnimCapsule();
}

void TopBase::DrawDebug(void)
{
}
