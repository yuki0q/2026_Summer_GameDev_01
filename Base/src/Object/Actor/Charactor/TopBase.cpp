#include <EffekseerForDXLib.h>
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
#include "../../../Effect/EffekseerEffect.h"
#include "TopBase.h"

TopBase::TopBase(const TopBase::TopData& data)
	:
	CharactorBase(),
	centerMovePow_(AsoUtility::VECTOR_ZERO),
	centerPos_(AsoUtility::VECTOR_ZERO),
	centerRot_(AsoUtility::VECTOR_ZERO),
	centerQuaRot_(Quaternion::Identity()),
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
	isEnd_(false),
	imgChara_(0),
	isRespawning_(false),
	respawnTimer_(0.0f),
	isDying_(false),
	dyingTimer_(0.0f),
	trailColorF_(0),
	trailColorE_(0),
	trailTimer_(0.0f),
	isDash_(false),
	isDashing_(false),
	isShielding_(false),
	isStabilitying_(false),
	isBlancing_(false),
	skillTimer_(0.0f),
	isExSkill_(false),
	isSkill_(false),
	prevSkill_(false),
	skillCoolTimer_(0.0f),
	dyeCount_(0),
	topType_(data.type),
	topsSpin_(data.topsSpin),
	topsSpinMax_(data.topsSpin),
	stability_(data.stability),
	defaultTilt_(data.defaultTilt),
	wobbleSpeed_(data.wobbleSpeed),
	topsWeight_(data.topsWeight),
	topsShock_(data.topsShock),
	topsSpeed_(data.speed),
	dashSpeed_(data.dashSpeed),
	skillSpeed_(data.skillSpeed),
	scrapSpeed_(data.scrapSpeed),
	dyingScrapSpeed_(data.dyingScrapSpeed),
	radiusFactor_(data.radiusFactor),
	currentEffHandle_(0),
	prevDashing_(false),
	gaugeFrame(0),
	spinGauge(0),
	//topType_(TOP_TYPE::BALANCE),// デフォルトはバランス]
	sceMng_(SceneManager::GetInstance())
{
}

TopBase::~TopBase(void)
{
}

void TopBase::InitLoad(void)
{
	// 基底クラスのリソースロード
	CharactorBase::InitLoad();

	gaugeFrame = resMng_.Load(ResourceManager::SRC::IMAME_SPINGAUGE_FRAME).handleId_;
	spinGauge = resMng_.Load(ResourceManager::SRC::IMAME_SPINGAUGE).handleId_;

	switch (topType_)
	{
	case TOP_TYPE::ATTACK:
		// コマのカラー
		//transform_.SetModel(resMng_.Load(ResourceManager::SRC::BLUE_TOP).handleId_);
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::BLUE_TOP));

		break;

	case TOP_TYPE::DEFENSE:
		//transform_.SetModel(resMng_.Load(ResourceManager::SRC::GREEN_TOP).handleId_);
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::GREEN_TOP));

		break;

	case TOP_TYPE::STAMINA:
		//transform_.SetModel(resMng_.Load(ResourceManager::SRC::YELLOW_TOP).handleId_);
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::YELLOW_TOP));

		break;
	case TOP_TYPE::BALANCE:
		//transform_.SetModel(resMng_.Load(ResourceManager::SRC::RED_TOP).handleId_);
		transform_.SetModel(resMng_.LoadModelDuplicate(ResourceManager::SRC::RED_TOP));

		break;
	}
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
	ResetTrailEffect();

	dyeCount_ = 0;
	// 攻撃型
	isDashing_ = false;
	// 防御型
	isShielding_ = false;
	// スタミナ型
	isStabilitying_ = false;
	// バランス型
	isBlancing_ = false;

	skillTimer_ = 0.0f;
	skillCoolTimer_ = 0.0f;
	isSkill_ = false;
	isExSkill_ = false;

	trailTimer_ = 0.0f;             // 軌跡を追加する周期タイマー
	isRespawning_ = false;       // リスポーン直後フラグ
	respawnTimer_ = 0.0f;       // 無敵・衝突無視のタイマー数
	isDying_ = false;       // 倒れ中フラグ
	dyingTimer_ = 0.0f;    // 倒れ始めてからの経過時間

	// タイプに応じたパラメータの味付け
	switch (topType_)
	{
	case TOP_TYPE::ATTACK:
		//topsSpin_ = topsSpinMax_ = 90.0f;
		//stability_ = 0.3f;     // 復元力が弱いので、ずっとヨロヨロする
		//defaultTilt_ = 0.26f;   // 最初から約8度くらい傾いて回る
		//wobbleSpeed_ = 2.5f;    // 激しくグルグル円を描く
		//topsWeight_ = 30.0f;
		//topsShock_ = 0.5f;
		//scrapSpeed_ = 0.0005f;
		//dyingScrapSpeed_ = 0.10f;
		//skillSpeed_ = 100.0f;
		//radiusFactor_ = 1.2f;

		// 軌跡のカラー
		trailColorF_ = 0x6464FF;
		trailColorE_ = 0x3232FF;
		break;

	case TOP_TYPE::DEFENSE:
		//topsSpin_ = topsSpinMax_ = 100.0f;
		//stability_ = 5.0f;     // すぐ直立に戻る
		//defaultTilt_ = 0.0f;
		//wobbleSpeed_ = 0.8f;
		//topsWeight_ = 28.5f;
		//topsShock_ = 0.3f;
		//scrapSpeed_ = 0.0002;
		//skillSpeed_ = 1.0f;
		//dyingScrapSpeed_ = 0.05f;
		//radiusFactor_ = 0.3f;

		trailColorF_ = 0x64ff64;
		trailColorE_ = 0x32ff32;
		break;

	case TOP_TYPE::STAMINA:
		//topsSpin_ = topsSpinMax_ = 120.0f;
		//stability_ = 3.0f;     // 標準的
		//defaultTilt_ = 0.0f;    // 全くブレずに静かに回る
		//wobbleSpeed_ = 1.0f;
		//topsWeight_ = 25.0f;
		//topsShock_ = 0.2f;
		//scrapSpeed_ = 0.0002;
		//skillSpeed_ = SPEED_MOVE;
		//dyingScrapSpeed_ = 0.03f;
		//radiusFactor_ = 0.75;

		trailColorF_ = 0xC8FF64;
		trailColorE_ = 0xE1FF32;
		break;
	case TOP_TYPE::BALANCE:
		//topsSpin_ = topsSpinMax_ = 110.0f;
		//stability_ = 1.5f;     // 標準的
		//defaultTilt_ = 0.15f;   
		//wobbleSpeed_ = 1.75f;
		//topsWeight_ = 27.5f;
		//topsShock_ = 0.4f;
		//scrapSpeed_ = 0.0002f;
		//skillSpeed_ = 50.0f;
		//dyingScrapSpeed_ = 0.075f;
		//radiusFactor_ = 1.0f;

		trailColorF_ = 0xff6464;
		trailColorE_ = 0xff3232;
		break;
	}
}

void TopBase::Draw(void)
{
	CharactorBase::Draw();
	DrawDebug();

	// 軌跡の描画処理
	TrailDraw();
}

void TopBase::Release(void)
{
	CharactorBase::Release();

	ResetTrailEffect();
	EffekseerEffect::GetInstance()->SkillStop(this);
	EffekseerEffect::GetInstance()->DeleteInstance();

	DeleteGraph(imgChara_);
	DeleteGraph(gaugeFrame);
	DeleteGraph(spinGauge);
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

	if (topsSpin_ < 0.0f)
	{
		topsSpin_ = 0.0f;
	}
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
	if (isRespawning_) return;
		DrawBillboard3D({ centerPos_.x,centerPos_.y + 200.0f,centerPos_.z },
			0.5f, 0.5f, 200.0f, 0.0f, imgChara_, true);

}

void TopBase::UpdateProcess(void)
{
	// コマの回転・移動処理
	ProcessTopMove();

	// 移動操作
	ProcessMove();

	// コマの傾き
	ProcessTopTilt();

	// コマの軌跡
	ProcessTopTrail();
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
		//Respawn(); // リスポーン判定
		isEnd_ = true;
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
		{ transform_.localPos.x * (topsSpin_ / topsSpinMax_) * radiusFactor_, 0.0f,
		transform_.localPos.z * (topsSpin_ / topsSpinMax_) * radiusFactor_ }, parentMat);
	/*VECTOR localRotPos_ = VTransform(VScale(transform_.localPos,
		topsSpin_ / TOPS_SPIN_MAX), parentMat);*/

	if (isShielding_) {
		localRotPos_.x = localRotPos_.z = 1.0f;
	}

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
		if (topsSpin_ >= 0.0f) 
		{
			if (!isStabilitying_) {
				if (topsSpin_ < 20.0f)
				{
					topsSpin_ -= dyingScrapSpeed_;//0.10f;
				}
				else
				{
					// 通常時（10.0f以上）の緩やかな削れかた
					/*topsSpin_ -= (topsSpin_ * scrapSpeed_ + 
						topsMovement_ * scrapSpeed_/2.0f) / 2.0f;*/
					topsSpin_ -= (topsSpin_ * 0.0002f + topsMovement_ * 0.0001f) / 2.0f;
				}
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

	//if (topsSpin_ < 0.0f)
	//{
	//	topsSpin_ = 0.0f;
	//}

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


void TopBase::ProcessTopTilt(void)
{
	float dt = scnMng_.GetDeltaTime();

	// スピンに応じた傾き角度
	// スピンが最大のときは傾きゼロ、ゼロに近づくほど大きく傾く
	float spinRatio = topsSpin_ / topsSpinMax_;          // 1.0→0.0
	float maxTilt = AsoUtility::Deg2RadF(35.0f);        // 最大傾き角度(調整可)
	float gravTilt = maxTilt * (1.0f - spinRatio);       // 重力由来の傾き

	// 歳差運動の位相を進める
	// スピンが遅いほど歳差もゆっくり
	float precessionSpeed = AsoUtility::Deg2RadF(180.0f * spinRatio); // 調整可
	//	tiltPhase_ += precessionSpeed * dt;

		// 移動慣性による傾き
	float inertiaTiltX = topsVel_.z * 0.0003f;
	float inertiaTiltZ = -topsVel_.x * 0.0003f;

	// 衝突傾きを徐々に減衰させる
	/*collisionTiltX_ *= 0.92f;
	collisionTiltZ_ *= 0.92f;*/

	if (!isDying_)
	{
		// 歳差運動（ヨロヨロ）の時間を進める（攻撃型は早く回る）
		tiltPhase_ += wobbleSpeed_ * dt;

		// 外部からの衝撃によるブレ（collisionTilt）を stability_ に応じて減衰させる
		// 防御型（stability_がデカい）なら一瞬で 0 に戻る
		collisionTiltX_ *= (1.0f - (5.0f * stability_ * dt));
		collisionTiltZ_ *= (1.0f - (5.0f * stability_ * dt));

		tiltX_ = sinf(tiltPhase_) * defaultTilt_ + collisionTiltX_;
		tiltZ_ = cosf(tiltPhase_) * defaultTilt_ + collisionTiltZ_;
		TopSorting();
	}
	else
	{
		//topsSpin_ = 0.0f;

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
			/*Respawn();*/
			isEnd_ = true;
			isDying_ = false;
		}
	}
}

void TopBase::ProcessTopTrail(void)
{
	bool dashEff = isDash_ || isDashing_;
	// 通常からダッシュに切り替わった瞬間
	if (dashEff && !prevDashing_)
	{
		// ダッシュ用エフェクトに切り替える
		EffekseerEffect::GetInstance()->TrailDashStart(this, static_cast<int>(topType_));
	}
	// ダッシュから通常に戻った瞬間
	else if (!dashEff && prevDashing_)
	{
		// 通常エフェクトに戻す
		EffekseerEffect::GetInstance()->TrailIdleStart(this, static_cast<int>(topType_));
	}

	// 前フレームの状態を記憶（次フレームの切り替え検知用）
	prevDashing_ = dashEff;

	EffekseerEffect::GetInstance()->TrailUpdate(this, transform_.pos);
}

void TopBase::TopSorting(void)
{
	if (isSkill_ && !prevSkill_)
	{
		EffekseerEffect::GetInstance()->SkillStop(this);

		EffekseerEffect::GetInstance()->SkillStart(this, static_cast<int>(topType_));
	}

	switch (topType_) {
	case TOP_TYPE::ATTACK:
		ProccesTypeAttack();
		break;
	case TOP_TYPE::DEFENSE:
		ProccesTypeDefense();
		break;
	case TOP_TYPE::STAMINA:
		ProccesTypeStamina();
		break;
	case TOP_TYPE::BALANCE:
		ProccesTypeBalance();
		break;
	}

	if (skillCoolTimer_ > 0.0f)
	{
		skillCoolTimer_ -= scnMng_.GetDeltaTime();
		if (skillCoolTimer_ <= 0.0f) 
			skillCoolTimer_ = 0.0f; // 0以下にならないようクランプ
	}

	if (isDashing_)
	{
		skillTimer_ -= scnMng_.GetDeltaTime();
		if (skillTimer_ <= 0.0f || !isSkill_)
		{
			isDashing_ = false;
			topsShock_ = 0.5f; // 元の衝撃力に戻す
			radiusFactor_ = 1.2f;
			trailColorF_ = GetColor(100, 100, 255);
			trailColorE_ = GetColor(50, 50, 255);
			EffekseerEffect::GetInstance()->SkillStop(this);
		}
	}

	if (isShielding_)
	{
		skillTimer_ -= scnMng_.GetDeltaTime();
		if (skillTimer_ <= 0.0f || !isSkill_)
		{
			isShielding_ = false;
			trailColorF_ = GetColor(100, 255, 100);
			trailColorE_ = GetColor(50, 255, 50);
			EffekseerEffect::GetInstance()->SkillStop(this);
		}
	}

	if (isStabilitying_)
	{
		skillTimer_ -= scnMng_.GetDeltaTime();
		if (skillTimer_ <= 0.0f || !isSkill_)
		{
			isStabilitying_ = false;
			trailColorF_ = GetColor(255, 255, 100);
			trailColorE_ = GetColor(255, 255, 50);
			stability_ = 3.0f;
			EffekseerEffect::GetInstance()->SkillStop(this);
		}
	}

	if (isBlancing_)
	{
		skillTimer_ -= scnMng_.GetDeltaTime();
		if (skillTimer_ <= 0.0f || !isSkill_)
		{
			isBlancing_ = false;
			trailColorF_ = GetColor(255, 100, 100);
			trailColorE_ = GetColor(255, 50, 50);
			stability_ = 1.5f;
			topsShock_ = 0.4f;
			EffekseerEffect::GetInstance()->SkillStop(this);
		}
	}
	//isSkill_がtrueの間だけ SkillUpdate を実行
	if (isSkill_)
	{
		EffekseerEffect::GetInstance()->SkillUpdate(this, transform_.pos);
	}

	// 現在のフレームの状態を保存（次フレーム判定用）
	prevSkill_ = isSkill_;
}

void TopBase::ProccesTypeAttack(void)
{
	// 現在の移動システム（centerMovePow_）に合わせて、毎フレーム足し込むベクトルを作る
	float attackMoveForce = 5.0f; // centerMovePow_に直接足すため、数値は小さめ（3.0f〜8.0f程度）から調整してください
	VECTOR slideAccel = { tiltX_ * attackMoveForce, 0.0f, tiltZ_ * attackMoveForce };

	// 攻撃型の推進力を、実際に移動に使われている centerMovePow_ に直接加算する
	centerMovePow_ = VAdd(centerMovePow_, slideAccel);

	if (!isDashing_ && skillCoolTimer_ <= 0.0f && isSkill_)
	{
		isDashing_ = true;
		skillTimer_ = 1.0f;
		skillCoolTimer_ = SKILL_COOL_TIME_A;
	//	radiusFactor_ = 3.0f;
		// 突進方向に強い慣性を乗せる（現在の移動速度や向きに合わせて）
		VECTOR dashDir = VNorm({tiltX_, 0.0f, tiltZ_});
		if (VSizeSq(dashDir) < 0.01f) {
			dashDir = { 1.0f, 0.0f, 0.0f }; // 傾きがない場合の安全策
		}
		// 瞬間的に慣性速度を大きくする
		centerMovePow_ = VScale(dashDir, 800.0f);

		// 攻撃型専用の衝撃力バフ（通常0.5f → ダッシュ時 2.0f）
		topsShock_ = 2.0f;
	}
}

void TopBase::ProccesTypeDefense(void)
{
	centerMovePow_ = VScale(centerMovePow_, 0.97f);

	if (!isShielding_ && skillCoolTimer_ <= 0.0f && isSkill_)
	{
		isShielding_ = true;
		skillTimer_ = 3.0f;

		skillCoolTimer_ = SKILL_COOL_TIME_D;
	}

	
}

void TopBase::ProccesTypeStamina(void)
{
	centerMovePow_ = VScale(centerMovePow_, 0.85f);

	if(!isStabilitying_ && skillCoolTimer_ <= 0.0f && isSkill_)
	{
		isStabilitying_ = true;
		skillTimer_ = 5.0f; 

		skillCoolTimer_ = SKILL_COOL_TIME_S;

		collisionTiltX_ = 0.0f;
		collisionTiltZ_ = 0.0f;

		stability_ = 15.0f;
	}
	;
}

void TopBase::ProccesTypeBalance(void)
{
	float balanceMoveForce = 1.5f;
	VECTOR slideAccel = { tiltX_ * balanceMoveForce, 0.0f, tiltZ_ * balanceMoveForce };
	centerMovePow_ = VAdd(centerMovePow_, slideAccel);

	if (!isBlancing_ && skillCoolTimer_ <= 0.0f && isSkill_)
	{
		isBlancing_ = true;
		
		VECTOR inputDir = { tiltX_, 0.0f, tiltZ_ };
		if (topsSpin_ > topsSpinMax_ / 2.0f) 
		{
			// 攻撃型
			centerMovePow_ = VScale(VNorm(inputDir), 1200.0f);
			topsShock_ = 1.5f;
			skillTimer_ = 1.2f;
			skillSpeed_ = 80.0f;
		}
		else // 防御型
		{
			isShielding_ = true; // 既存のフラグを流用
			stability_ = 10.0f;
			skillTimer_ = 1.0f;
			skillSpeed_ = 1.0f;
		}

		skillCoolTimer_ = SKILL_COOL_TIME_B;
	}
	;
}

void TopBase::CollisionReserve(void)

{
	// アニメーションごとの位置調整
	ProcessAnimPos();

	ProcessAnimCapsule();
}

void TopBase::Respawn(void)
{
	EffekseerEffect::GetInstance()->SkillStop(this);
	ResetTrailEffect();

	topsVel_ = { 0.0f, 0.0f, 0.0f };
	transform_.pos = respawnPos_;
	centerPos_ = respawnCenterPos_;
	prevPos_ = respawnPos_;
	topsSpin_ = topsSpinMax_;

	// === ここから追加・修正 ===
	// 衝突による強制移動やめり込み補正の目標座標をクリアする
	collisionTargetPos_ = { 0.0f, 0.0f, 0.0f };
	hasCollisionTarget_ = false;

	// 衝突によって発生したコマの傾き（衝撃）をリセット
	collisionTiltX_ = 0.0f;
	collisionTiltZ_ = 0.0f;
	centerMovePow_ = { 0.0f, 0.0f, 0.0f }; // 加速度・移動パワーもゼロに
	// === ここまで追加・修正 ===

	isRespawning_ = true;
	respawnTimer_ = 0.0f;

	isDying_ = false;
	dyingTimer_ = 0.0f;
	tiltX_ = 0.0f;
	tiltZ_ = 0.0f;

	prevSkill_ = false;
	isEnd_ = false;

	trailPoints_.clear();
	trailTimer_ = 0.0f;	
}

void TopBase::ResetTrailEffect(void)
{
	EffekseerEffect::GetInstance()->TrailStop(this);
	EffekseerEffect::GetInstance()->TrailIdleStart(this, static_cast<int>(topType_));
	EffekseerEffect::GetInstance()->TrailUpdate(this, transform_.pos);
}

void TopBase::DrawDebug(void)
{
}

void TopBase::TrailDraw(void)
{
	// 軌跡の描画処理
	if (trailPoints_.size() < 2) return;

	// 透過処理を有効にする
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);

	// 2点ずつ結んで四角い板ポリゴンを描画していく
	for (size_t i = 0; i < trailPoints_.size() - 1; ++i)
	{
		const auto& p1 = trailPoints_[i];
		const auto& p2 = trailPoints_[i + 1];

		// 進行方向ベクトル
		VECTOR dir = VSub(p2.pos, p1.pos);
		if (VSizeSq(dir) < 0.001f) continue; // 動いていない時はスキップ

		// 進行方向と上方向（Y軸）の外積から、真横を向くベクトル（幅の方向）を計算
		VECTOR up = { 0.0f, 1.0f, 0.0f };
		VECTOR side = VCross(dir, up);
		side = VNorm(side); // 正規化

		// 軌跡の幅を適用
		VECTOR halfSide = VScale(side, TRAIL_WIDTH * 0.5f);

		// 4つの頂点座標を計算（p1の左右、p2の左右）
		VECTOR v1_L = VSub(p1.pos, halfSide);
		VECTOR v1_R = VAdd(p1.pos, halfSide);
		VECTOR v2_L = VSub(p2.pos, halfSide);
		VECTOR v2_R = VAdd(p2.pos, halfSide);

		// アルファ値をDxLibのカラーに反映（255段階）
		int colorVal1 = static_cast<int>(p1.alpha * 255);
		int colorVal2 = static_cast<int>(p2.alpha * 255);

		// 軌跡の色を指定
		unsigned int color1 = trailColorF_;
		unsigned int color2 = trailColorE_;

		// それぞれの頂点の不透明度を設定して三角形2枚で四角形を描画
		// （テクスチャ画像を使いたい場合は DrawPolygon3D を使用しますが、
		//   単色ラインなら DrawTriangle3D で十分綺麗に光のラインっぽくなります）
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, colorVal1);
		DrawTriangle3D(v1_L, v1_R, v2_L, color1, TRUE);
		DrawTriangle3D(v2_L, v1_R, v2_R, color2, TRUE);
	}

	// 描画ブレンドモードを元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

float TopBase::VSizeSq(const VECTOR& v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}
