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
	topsSpin_(0.0f),
	topsWeight_(0.0f),
	topsRadius_(0.0f),
	topsVel_(AsoUtility::VECTOR_ZERO),
	prevPos_(AsoUtility::VECTOR_ZERO)
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

float TopBase::GetRadius(void)
{
	return topsRadius_;
}

float TopBase::GetSpeed(void)
{
	return topsSpeed_;
}

float TopBase::GetWeight(void)
{
	return topsWeight_;
}

VECTOR TopBase::GetVel(void)
{
	return topsVel_;
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
