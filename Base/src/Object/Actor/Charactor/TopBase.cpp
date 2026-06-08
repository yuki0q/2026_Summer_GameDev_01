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
	topsSpinMax_(0.0f),
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
	isHit_(false),
	collisionTargetPos_(AsoUtility::VECTOR_ZERO),
	respawnPos_(AsoUtility::VECTOR_ZERO),
	respawnCenterPos_(AsoUtility::VECTOR_ZERO),
	topsShock_(0.0f),
	isEnd_(false),
	imgChara_(0),
	isRespawning_(false),
	respawnTimer_(0.0f),
	isDying_(false),
	dyingTimer_(0.0f)
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

	DeleteGraph(imgChara_);
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

bool TopBase::GetCollisionTarget_(void)
{
	return hasCollisionTarget_;
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
	float spinRatio = topsSpin_ / topsSpinMax_;
	float maxColTilt = AsoUtility::Deg2RadF(20.0f + 30.0f * (1.0f - spinRatio));

	collisionTiltX_ = std::clamp(collisionTiltX_, -maxColTilt, maxColTilt);
	collisionTiltZ_ = std::clamp(collisionTiltZ_, -maxColTilt, maxColTilt);
}

void TopBase::SetCollisionTarget(VECTOR targetPos)
{
	collisionTargetPos_ = targetPos;
	hasCollisionTarget_ = true;
	isHit_ = true;
}

void TopBase::DrawImage(void)
{
	DrawBillboard3D({ centerPos_.x,centerPos_.y + 200.0f,centerPos_.z },
		0.5f, 0.5f, 200.0f, 0.0f, imgChara_, true);

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
	isRespawning_ = false;       // リスポーン直後フラグ
	respawnTimer_ = 0.0f;       // 無敵・衝突無視のタイマー数
	isDying_ = false;       // 倒れ中フラグ
	dyingTimer_ = 0.0f;    // 倒れ始めてからの経過時間
}

void TopBase::UpdateProcess(void)
{
	// コマの回転・移動処理
	ProcessTopMove();

	float dt = scnMng_.GetDeltaTime();

	// スピンに応じた傾き角度
	// スピンが最大のときは傾きゼロ、ゼロに近づくほど大きく傾く
	float spinRatio = topsSpin_ / topsSpinMax_;          // 1.0→0.0
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
    /*tiltX_ = sinf(tiltPhase_) * gravTilt + inertiaTiltX + collisionTiltX_;
    tiltZ_ = cosf(tiltPhase_) * gravTilt + inertiaTiltZ + collisionTiltZ_;*/

	if (!isDying_)
	{
		// 通常時の傾き合成（既存の処理）
		tiltX_ = sinf(tiltPhase_) * gravTilt + inertiaTiltX + collisionTiltX_;
		tiltZ_ = cosf(tiltPhase_) * gravTilt + inertiaTiltZ + collisionTiltZ_;
	}
	else
	{
		// 倒れる演出中の処理
		dyingTimer_ += dt;

		// 完全に倒れるまでの時間
		float fallDuration = 2.5f;
		float progress = dyingTimer_ / fallDuration; // 0.0 ～ 1.0 に変化
		if (progress > 1.0f) progress = 1.0f;

		// 倒れる限界角度（約80度＝ほぼ真横にパタンと倒れる）
		float maxFallTilt = AsoUtility::Deg2RadF(60.0f);

		// 徐々に激しくブレながら倒れる演出
		// progressが進むほど、ベースの傾き(maxFallTilt * progress)が大きくなる
		float fallSpinSpeed = 30.0f * (1.0f - progress * 0.5f);
		float fallPhase = dyingTimer_ * fallSpinSpeed;

		// 現在どの方向に倒れ込んでいるかのベース
		float currentBaseTilt = maxFallTilt * progress;

		// コマがすり鉢状に回転しながら（ブレながら）倒れる様子を、円運動（sin, cos）で表現
		// progressが進むほど、回転の軸自体が大きく傾いていく
		float maxWobbleWidth = AsoUtility::Deg2RadF(5.0f);
		float wobbleWidth = maxWobbleWidth * (1.0f - progress);

		tiltX_ = sinf(fallPhase) * currentBaseTilt + cosf(fallPhase) * wobbleWidth;
		tiltZ_ = cosf(fallPhase) * currentBaseTilt + sinf(fallPhase) * wobbleWidth;

		topsSpin_ *= 0.95f; // 毎フレーム5%ずつ回転を遅くする

		// 指定時間経過したらリスポーン
		if (dyingTimer_ >= fallDuration)
		{
			//Respawn();
			isEnd_ = true;
			isDying_ = false;
		}
	}
	
}

void TopBase::UpdateProcessPost(void)
{
	if (!isDying_)
	{
		// 通常時の行列合成
		MATRIX rotX = MGetRotX(tiltX_);
		MATRIX rotZ = MGetRotZ(tiltZ_);
		MATRIX rotY = Quaternion::ToMatrix(transform_.quaRot);

		MATRIX mat = MMult(rotZ, MMult(rotX, rotY));
		MV1SetRotationMatrix(transform_.modelId, mat);

	}
	else
	{
		// 死亡演出時：
		// tiltX_ と tiltZ_ の中にすでに「回転しながら倒れる」という軌道（円運動）が計算されているため、
		// X軸とZ軸の回転を合成するだけで、コマが螺旋を描くように回りながら倒れます。
		MATRIX rotX = MGetRotX(tiltX_);
		MATRIX rotZ = MGetRotZ(tiltZ_);
		MATRIX rotY = Quaternion::ToMatrix(transform_.quaRot);

		MATRIX rotMat = MMult(rotZ, MMult(rotX, rotY));

		// 現在の座標（平行移動）の行列を作成
		MATRIX posMat = MGetTranslate(transform_.pos);

		// 回転してから現在の位置へ平行移動するように合成
		MATRIX mat = MMult(rotMat, posMat);

		MV1SetMatrix(transform_.modelId, mat);
	}
}

void TopBase::ProcessMove(void)
{
}

void TopBase::ProcessJump(void)
{
}

void TopBase::ProcessTopMove(void)
{
	if (transform_.pos.y < TOPS_DEAD_POS_Y) {
		Respawn(); // リスポーン判定
	}

	// リスポーンタイマーの更新
	if (isRespawning_)
	{
		respawnTimer_ += scnMng_.GetDeltaTime();
		if (respawnTimer_ >= RESPAWN_MUTE_TIME)
		{
			isRespawning_ = false; // 指定時間経過したら通常状態に戻す
			respawnTimer_ = 0.0f;
		}
	}

	if (isDying_)
	{
		hasCollisionTarget_ = false; // 座標ロックを解除
		prevPos_ = transform_.pos;
		// 座標の更新などは行わず、UpdatePostでの行列合成に任せる
		transform_.Update();
		return;
	}

	// コマ自身の回転
	transform_.quaRot = Quaternion::Mult(transform_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(topsSpinMax_ *
			topsSpin_ / topsSpinMax_), AsoUtility::AXIS_Y));

	// centerPos周りの回転
	centerQuaRot_ = Quaternion::Mult(centerQuaRot_,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(topsSpeed_ *
			topsSpin_ / topsSpinMax_), AsoUtility::AXIS_Y));

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
	VECTOR localRotPos_ = VTransform(
		{ transform_.localPos.x * topsSpin_ / topsSpinMax_, 0.0f,
		transform_.localPos.z * topsSpin_ / topsSpinMax_ }, parentMat);
	/*VECTOR localRotPos_ = VTransform(VScale(transform_.localPos,
		topsSpin_ / TOPS_SPIN_MAX), parentMat);*/

	// ワールド座標
	//transform_.pos = VAdd(localRotPos_, { centerPos_.x,transform_.pos.y,centerPos_.z });
	transform_.pos.x = localRotPos_.x + centerPos_.x;
	transform_.pos.z = localRotPos_.z + centerPos_.z;

	centerPos_.y = transform_.pos.y;

 	/*if (topsSpin_ > 13.5f) {
		topsSpin_ -= (topsSpin_ * 0.0002f + topsMovement_ * 0.0001f) / 2.0f;
	}
	else {
		Respawn();
	}*/

	if (!isDying_)
	{
		if (topsSpin_ > 0.0f) 
		{
			if (topsSpin_ < 15.0f)
			{
				topsSpin_ -= 0.10f;
			}
			else
			{
				// 通常時（10.0f以上）の緩やかな削れかた
				topsSpin_ -= (topsSpin_ * 0.0002f + topsMovement_ * 0.0001f) / 2.0f;
			}
		}
		else {
			// スピンが切れたら死亡演出（倒れるモード）へ移行
			isDying_ = true;
			dyingTimer_ = 0.0f;
			topsSpin_ = 0.0f; // スピンを完全に止める

			isEnd_ = false;
		}
	}

	//0.998f

	/*if (20.0f > topsSpin_)
	{
		topsSpeed_ = 3.0f;
	}*/

	if (topsSpin_ < 0.0f)
	{
		topsSpin_ = 0.0f;
	}

	if (transform_.pos.y > 300.0f) transform_.pos.y = 300.0f;

	float dt = scnMng_.GetDeltaTime();

	if (VSizeSq(VSub(transform_.pos, prevPos_)) > 500.0f * 500.0f) 
	{
		prevPos_ = transform_.pos;
	}

	if (dt > 0.0f)
	{
		// 位置差分
		VECTOR move = VSub(transform_.pos, prevPos_);

		/*if (move.x == 0.0f && move.z == 0.0f) {
			move.x = move.z = 1.0f;
		}*/

		// 速度ベクトル
		topsVel_ = VScale(move, 1.0f / dt);

		// 速度の大きさ
		topsMovement_ = VSize(topsVel_);
	}

	// 摩擦
	topsVel_ = VScale(topsVel_, 0.97f);

	/*if (!hasCollisionTarget_)
	{
		centerPos_.x += topsVel_.x * scnMng_.GetDeltaTime();
		centerPos_.z += topsVel_.z * scnMng_.GetDeltaTime();
	}*/

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
			float lerpSpeed = 0.1f;
			centerPos_ = VAdd(centerPos_, VScale(toTarget, lerpSpeed));
			//hasCollisionTarget_ = false;
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

void TopBase::Respawn(void)
{
	isEnd_ = true;


	//transform_.pos = respawnPos_;
	//centerPos_ = respawnCenterPos_;
	topsSpin_ = topsSpinMax_;

	// 速度をゼロに
	topsVel_ = { 0.0f, 0.0f, 0.0f };
	prevPos_ = respawnPos_;

	isRespawning_ = true;
	respawnTimer_ = 0.0f;



	isDying_ = false;
	dyingTimer_ = 0.0f;
	tiltX_ = 0.0f;
	tiltZ_ = 0.0f;
}

void TopBase::DrawDebug(void)
{
}

float TopBase::VSizeSq(const VECTOR& v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}
