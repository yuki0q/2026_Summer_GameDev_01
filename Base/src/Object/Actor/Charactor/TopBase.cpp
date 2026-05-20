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
#include "../../../Common/Quaternion.h"
#include "TopBase.h"

TopBase::TopBase(void)
	:
	CharactorBase(),
	centerMovePow_(AsoUtility::VECTOR_ZERO),
	centerPos_(AsoUtility::VECTOR_ZERO),
	centerRot_(AsoUtility::VECTOR_ZERO),
	centerQuaRot_(Quaternion::Identity()),
	topsSpeed_(0.0f),
	topsSpin_(0.0f),
	topsWeight_(0.0f),
	topsRadius_(0.0f),
	topsMovement_(0.0f),
	tiltX_(0.0f),
	tiltZ_(0.0f),
	tiltAngle_(0.0f),
	tiltPhase_(0.0f),
	collisionTiltX_(0.0f),
	collisionTiltZ_(0.0f),
	topsVel_(AsoUtility::VECTOR_ZERO),
	prevPos_(AsoUtility::VECTOR_ZERO),
	hasCollisionTarget_(false),
	collisionTargetPos_(AsoUtility::VECTOR_ZERO),
	respawnPos_(AsoUtility::VECTOR_ZERO),
	respawnCenterPos_(AsoUtility::VECTOR_ZERO)
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

float TopBase::GetSpin(void)
{
	return topsSpin_;
}

void TopBase::SpinScrape(float spin)
{
	topsSpin_ -= spin;
}

float TopBase::GetWeight(void)
{
	return topsWeight_;
}

VECTOR TopBase::GetVel(void)
{
	return topsVel_;
}

void TopBase::SetVel(VECTOR vel)
{
	topsVel_ = vel;
}

VECTOR TopBase::GetPos(void)
{
	return centerPos_;
}

void TopBase::SetPos(VECTOR pos)
{
	//transform_.pos = pos;
	centerPos_ = pos;
}

void TopBase::AddCollisionTilt(VECTOR impulseDir, float impulseMag)
{
	// 衝突方向と強さから傾きを計算
	// impulseDir は正規化された衝突法線ベクトルを想定
	float tiltScale = impulseMag * 0.0002f;

	// 衝突方向と逆向きに傾く（弾かれる方向へ）
	collisionTiltX_ += -impulseDir.z * tiltScale;
	collisionTiltZ_ += impulseDir.x * tiltScale;

	// 傾きを歳差位相にも反映させる
	tiltPhase_ = atan2f(collisionTiltX_, collisionTiltZ_);
}

void TopBase::ApplyStageTilt(VECTOR hitNormal, float impactSpeed)
{
	// 法線のY成分で地面か壁かを判定
	float normalY = hitNormal.y;

	if (normalY > 0.7f)
	{
		// 地面・ほぼ水平面への着地
		// 着地の衝撃で速度に応じて小さく揺れる
		float shakeMag = impactSpeed * 0.00015f;

		// 現在の歳差位相と逆方向に揺り戻す
		collisionTiltX_ += cosf(tiltPhase_) * shakeMag;
		collisionTiltZ_ += -sinf(tiltPhase_) * shakeMag;
	}
	else if (normalY < 0.3f)
	{
		// 壁・急斜面への衝突
		// 壁の法線の水平成分方向と逆に傾く
		float tiltScale = impactSpeed * 0.0003f; // 感度調整

		collisionTiltX_ += -hitNormal.z * tiltScale;
		collisionTiltZ_ += hitNormal.x * tiltScale;

		// 歳差の位相も衝突方向に向ける
		tiltPhase_ = atan2f(collisionTiltX_, collisionTiltZ_);
	}
	else
	{
		// 中間の斜面
		// 地面寄りと壁寄りの中間の挙動
		float tiltScale = impactSpeed * 0.0002f * (1.0f - normalY);

		collisionTiltX_ += -hitNormal.z * tiltScale;
		collisionTiltZ_ += hitNormal.x * tiltScale;
	}

	// 傾きの上限（コマが倒れすぎないようにクランプ）
	float spinRatio = topsSpin_ / TOPS_SPIN_MAX;
	float maxColTilt = AsoUtility::Deg2RadF(20.0f + 30.0f * (1.0f - spinRatio));

	collisionTiltX_ = std::clamp(collisionTiltX_, -maxColTilt, maxColTilt);
	collisionTiltZ_ = std::clamp(collisionTiltZ_, -maxColTilt, maxColTilt);
}

void TopBase::SetCollisionTarget(VECTOR targetPos)
{
	collisionTargetPos_ = targetPos;
	hasCollisionTarget_ = true;
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
	// コマの回転・移動処理
	ProcessTopMove();

	float dt = scnMng_.GetDeltaTime();

	// スピンに応じた傾き角度
	// スピンが最大のときは傾きゼロ、ゼロに近づくほど大きく傾く
	float spinRatio = topsSpin_ / TOPS_SPIN_MAX;          // 1.0→0.0
	float maxTilt = AsoUtility::Deg2RadF(35.0f);        // 最大傾き角度(調整可)
	float gravTilt = maxTilt * (1.0f - spinRatio);       // 重力由来の傾き

	// 歳差運動の位相を進める
	// スピンが遅いほど歳差もゆっくり
	float precessionSpeed = AsoUtility::Deg2RadF(180.0f * spinRatio); // 調整可
	tiltPhase_ += precessionSpeed * dt;

	// 移動慣性による傾き
	float inertiaTiltX = topsVel_.z * 0.0003f;
	float inertiaTiltZ = -topsVel_.x * 0.0003f;

	// 衝突傾きを徐々に減衰させる
	collisionTiltX_ *= 0.92f;
	collisionTiltZ_ *= 0.92f;

	// 傾きを合成（歳差 + 慣性 + 衝突）
    tiltX_ = sinf(tiltPhase_) * gravTilt + inertiaTiltX + collisionTiltX_;
    tiltZ_ = cosf(tiltPhase_) * gravTilt + inertiaTiltZ + collisionTiltZ_;
	
}

void TopBase::UpdateProcessPost(void)
{
	// 行列合成
	MATRIX rotX = MGetRotX(tiltX_);
	MATRIX rotZ = MGetRotZ(tiltZ_);
	MATRIX rotY = Quaternion::ToMatrix(transform_.quaRot);

	MATRIX mat = MMult(rotZ, MMult(rotX, rotY));
	MV1SetRotationMatrix(transform_.modelId, mat);
}

void TopBase::ProcessMove(void)
{
}

void TopBase::ProcessJump(void)
{
}

void TopBase::ProcessTopMove(void)
{
	// コマ自身の回転
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(TOPS_SPIN_MAX *
			topsSpin_ / TOPS_SPIN_MAX), AsoUtility::AXIS_Y));

	// centerPos周りの回転
	centerQuaRot_ = Quaternion::Mult(centerQuaRot_,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(topsSpeed_ *
			topsSpin_ / TOPS_SPIN_MAX), AsoUtility::AXIS_Y));

	// 行列の合成
	MATRIX selfMat = Quaternion::ToMatrix(Quaternion::Mult(transform_.quaRotLocal, transform_.quaRot));

	// 親（centerPosの回転）
	/*MATRIX parentMat = MatrixUtility::GetMatrixRotateXYZ(
		Quaternion::ToEuler(Quaternion::Mult(centerQuaRot_,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(10.0f), AsoUtility::AXIS_Y))));*/

	MATRIX parentMat = Quaternion::ToMatrix(
		Quaternion::Mult(centerQuaRot_,
			Quaternion::AngleAxis(AsoUtility::Deg2RadF(10.0f), AsoUtility::AXIS_Y)));

	// 行列の合成(子 : コマ、 親 : centerPos
	MATRIX mat = MatrixUtility::Multiplication(selfMat, parentMat);

	// 行列を使用してモデルの角度を設定
	MV1SetRotationMatrix(transform_.modelId, mat);

	//ローカル座標を親の回転行列で回転
	VECTOR localRotPos_ = VTransform(transform_.localPos, parentMat);

	// ワールド座標
	//transform_.pos = VAdd(localRotPos_, { centerPos_.x,transform_.pos.y,centerPos_.z });
	transform_.pos.x = localRotPos_.x + centerPos_.x;
	transform_.pos.z = localRotPos_.z + centerPos_.z;

	centerPos_.y = transform_.pos.y;
	topsSpin_ -= (topsSpin_ * 0.002f + topsMovement_ * 0.0001f) / 2.0f;
	//0.998f

	if (topsSpin_ < 0.0f)
	{
		topsSpin_ = 0.0f;
	}

	if (transform_.pos.y < TOPS_DEAD_POS_Y || topsSpin_ <= 0.0f)
	{
		transform_.pos = respawnPos_;
		centerPos_ = respawnCenterPos_;
		topsSpin_ = TOPS_SPIN_MAX;
	}

	if (transform_.pos.y > 300.0f) transform_.pos.y = 300.0f;

	float dt = scnMng_.GetDeltaTime();

	if (dt > 0.0f)
	{
		// 位置差分
		VECTOR move = VSub(transform_.pos, prevPos_);

		if (move.x == 0.0f && move.z == 0.0f) {
			move.x = move.z = 1.0f;
		}

		// 速度ベクトル
		topsVel_ = VScale(move, 1.0f / dt);

		// 速度の大きさ
		topsMovement_ = VSize(topsVel_);
	}

	// 摩擦
	topsVel_ = VScale(topsVel_, 0.97f);

	// 次フレーム用保存
	prevPos_ = transform_.pos;

	// 衝突後の目標座標へ補間移動
	if (hasCollisionTarget_)
	{
		// XZ成分のみで距離を計算
		float dx = collisionTargetPos_.x - centerPos_.x;
		float dz = collisionTargetPos_.z - centerPos_.z;
		VECTOR toTarget = VSub(collisionTargetPos_, centerPos_);
		float distXZ = sqrtf(dx * dx + dz * dz);

		// 到達判定（誤差以内なら完了）
		if (distXZ < 1.0f)
		{
			// XZのみ目標座標に合わせる（Yは触らない）
			centerPos_.x = collisionTargetPos_.x;
			centerPos_.z = collisionTargetPos_.z;
			hasCollisionTarget_ = false;
		}
		else
		{
			// 補間速度（大きいほど素早く移動）
			float lerpSpeed = 0.3f;
			centerPos_ = VAdd(centerPos_, VScale(toTarget, lerpSpeed));
		}
	}

	transform_.Update();
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
