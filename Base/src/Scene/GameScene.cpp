#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/EnemyManager.h"
#include "../Manager/Resource.h"
#include "../Object/Actor/NormalStage.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Collider/ColliderBase.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	normalStage_(nullptr),
	player_(nullptr),
	enemyManager_(nullptr),
	shadowMapHandle_(0),
	SceneBase(),
	countTime_(0),
	isStart_(false),
	image3(0),
	image2(0),
	image1(0)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	// プレイヤー
	player_ = new Player();
	player_->Init();

	// ステージ
	normalStage_ = new NormalStage();
	normalStage_->Init();

	// エネミー管理
	enemyManager_ = new EnemyManager(player_);
	enemyManager_->Init();

	const ColliderBase* stageCollider =
		normalStage_->GetOwnCollider(static_cast<int>(NormalStage::COLLIDER_TYPE::MODEL));

	// ステージモデルのコライダーをプレイヤーに登録
	player_->AddHitCollider(stageCollider);

	for (auto& enemy : enemyManager_->GetEemies())
	{
		// エネミーのコライダーをプレイヤーに登録
		player_->
			AddHitCollider(enemy->GetOwnCollider(static_cast<int>
				(CharactorBase::COLLIDER_TYPE::CAPSULE)));
	}

	// ステージモデルのコライダーをエネミーに登録
	enemyManager_->AddHitCollider(stageCollider);

	// プレイヤーのコライダーをエネミーに登録
	enemyManager_->
		AddHitCollider(player_->GetOwnCollider(static_cast<int>
			(CharactorBase::COLLIDER_TYPE::CAPSULE)));

	// 追従カメラモードに変更
	Camera* camera = sceMng_.GetCamera();
	camera->SetFollow(&normalStage_->GetTransform());
	camera->SetFollow(&normalStage_->GetTransform());
	camera->ChangeMode(Camera::MODE::FOLLOW);
	camera->AddHitCollider(stageCollider);

	//// シャドウマップハンドルの作成
	shadowMapHandle_ = MakeShadowMap(2048, 2048);

	// シャドウマップが想定するライトの方向もセット
	SetShadowMapLightDirection(shadowMapHandle_, VGet(0.5f, -0.5f, 0.5f));

	// シャドウマップに描画する範囲を設定
	SetShadowMapDrawArea(shadowMapHandle_, VGet(-5000.0f, -70.0f, -6000.0f), 
		VGet(6000.0f, 1500.0f, 8000.0f));

	countTime_ = 180;
	isStart_ = false;

	image3 = resMng_.Load(ResourceManager::SRC::IMAGE_3).handleId_;
	image2 = resMng_.Load(ResourceManager::SRC::IMAGE_2).handleId_;
	image1 = resMng_.Load(ResourceManager::SRC::IMAGE_1).handleId_;
}

void GameScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::RESULT);
	}

	normalStage_->Update();

	if (countTime_ > 0)
	{
		countTime_--;
	}
	else{
		isStart_ = true;
	}

	if (isStart_) {
		player_->Update();
		enemyManager_->Update();
		Collision();
	}
}

void GameScene::Draw(void)
{
	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(shadowMapHandle_);

	player_->Draw();
	enemyManager_->Draw();
	normalStage_->Draw();

	if (!isStart_) {
		int i = countTime_ / 60;
		switch (i)
		{
		case 2:
			DrawBillboard3D(VGet(0.0f, 250.0f, 0.0f), 0.5f, 0.5f, 20.0f, 0.0f, image3, TRUE);
			//DrawRotaGraph(640, 260, 1.0f, 0.0f, image3, true);
			break;
		case 1:
			DrawBillboard3D(VGet(0.0f, 250.0f, 0.0f), 0.5f, 0.5f, 20.0f, 0.0f, image2, TRUE);
			break;
		case 0:
			DrawBillboard3D(VGet(0.0f, 250.0f, 0.0f), 0.5f, 0.5f, 20.0f, 0.0f, image1, TRUE);
			break;
		}
	}

	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, shadowMapHandle_);

	player_->Draw();
	enemyManager_->Draw();
	normalStage_->Draw();
	
	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);
}

void GameScene::Release(void)
{
	DeleteShadowMap(shadowMapHandle_);

	normalStage_->Release();
	delete normalStage_;

	player_->Release();
	delete player_;

	enemyManager_->Release();
	delete enemyManager_;

	DeleteGraph(image3);
	DeleteGraph(image2);
	DeleteGraph(image1);
}

void GameScene::Collision(void) 
{
	CollisionResolve();

}

// コマ同士の衝突
void GameScene::CollisionResolve(void)
{
	for (auto& enemy : enemyManager_->GetEemies())
	{

		VECTOR diff = VSub(enemy->GetTransform().pos, player_->GetTransform().pos);
		float dist = VSize(diff);
		float hitDist = player_->GetRadius() + enemy->GetRadius();

		// 衝突していない、または重なりすぎている場合は無視
		if (dist >= hitDist || dist <= 0.0001f) continue;

		// 衝突応答 (物理演算)
		//VECTOR normal = VDiv(diff, dist); // VNormの代わり（distが判明しているので効率的）
		VECTOR normal = { diff.x / dist,diff.y / dist,diff.z / dist };
		VECTOR rv = VSub(enemy->GetVel(), player_->GetVel());   // 相対速度
		float normalSpeed = VDot(rv, normal);	// 法線成分

		VECTOR tangent = VSub(rv, VScale(normal, normalSpeed));

		float tangentSpeed = VSize(tangent);	// 接線成分

		// 衝突後の速度計算
		VECTOR playerNewVel, enemyNewVel;

		// 正面よりの時
		if (normalSpeed > tangentSpeed)
		{
			// 反発係数 (1.0で完全弾性衝突)
			float e = 0.8f;

			// 衝撃量 j の計算
			float j = -(1.0f + e) * normalSpeed;
			j /= (1.0f / player_->GetWeight() + 1.0f / enemy->GetWeight());

			// 衝撃ベクトルを適用
			VECTOR impulse = VScale(normal, j);

			// スピン量に応じて少しだけ反発力を強化
			float spinFactor = (player_->GetSpin() + enemy->GetSpin()) * 0.001f;
			impulse = VScale(impulse, 1.0f + spinFactor);

			float impulseMag = VSize(impulse);
			// プレイヤーは法線の逆方向へ傾く
			VECTOR negNormal = VScale(normal, -1.0f);
			player_->AddCollisionTilt(negNormal, impulseMag);
			// エネミーは法線方向へ傾く
			enemy->AddCollisionTilt(normal, impulseMag);

			/*player_->SetVel(VScale(VSub(player_->GetVel(), VScale(impulse, 1.0f / player_->GetWeight())), 0.05f));
			enemy->SetVel(VScale(VAdd(enemy->GetVel(), VScale(impulse, 1.0f / enemy->GetWeight())), 0.05f));*/

			playerNewVel = VScale(VSub(player_->GetVel(), VScale(impulse, 1.0f / player_->GetWeight())), 0.05f);
			enemyNewVel = VScale(VAdd(enemy->GetVel(), VScale(impulse, 1.0f / enemy->GetWeight())), 0.05f);

		}
		else {	// 擦りよりの時
			tangent = VNorm(tangent);	// 正規化
			float friction = 0.3f;	// 摩擦係数
			float vt = VDot(rv, tangent);	// 接線速度
			float jt = -vt / (1.0f / player_->GetWeight() + 1.0f / enemy->GetWeight());	// 摩擦速度

			jt *= friction;

			VECTOR frictionImpulse = VScale(tangent, jt);

			// スピン量に応じて少しだけ反発力を強化
			float spinFactor = (player_->GetSpin() + enemy->GetSpin()) * 0.001f;
			frictionImpulse = VScale(frictionImpulse, 1.0f + spinFactor);

			float impulseMag = VSize(frictionImpulse);
			player_->AddCollisionTilt(VScale(tangent, -1.0f), impulseMag * 0.5f);
			enemy->AddCollisionTilt(tangent, impulseMag * 0.5f);

			/*player_->SetVel(VScale(VSub(player_->GetVel(),
				VScale(frictionImpulse, 1.0f / player_->GetWeight())), 0.05f));

			enemy->SetVel(VScale(VAdd(enemy->GetVel(),
				VScale(frictionImpulse, 1.0f / enemy->GetWeight())), 0.05f));*/

			playerNewVel = VScale(VSub(player_->GetVel(), VScale(frictionImpulse, 1.0f / player_->GetWeight())), 0.05f);
			enemyNewVel = VScale(VAdd(enemy->GetVel(), VScale(frictionImpulse, 1.0f / enemy->GetWeight())), 0.05f);
		}

		// めり込み補正 (Positional Correction)
		// これがないと、コマ同士が重なったまま震える現象が起きます
		float percent = 0.5f; // 補正の強さ
		float slop = 0.01f;   // 許容誤差

		VECTOR correction = VScale(normal, (fmaxf(hitDist - dist - slop, 0.0f) /
			(1.0f / player_->GetWeight() + 1.0f / enemy->GetWeight())) * percent);

		// 速度を確定・目標座標を計算して渡す
		player_->SetVel(playerNewVel);
		enemy->SetVel(enemyNewVel);

		VECTOR playerTargetPos = VSub(
			VAdd(player_->GetPos(), playerNewVel),
			VScale(correction, 1.0f / player_->GetWeight()));

		VECTOR enemyTargetPos = VAdd(
			VAdd(enemy->GetPos(), enemyNewVel),
			VScale(correction, 1.0f / enemy->GetWeight()));

		if(playerTargetPos.y > 200.0f)
		{
			playerTargetPos.y = 200.0f;
		}

		if(enemyTargetPos.y > 200.0f)
		{
			enemyTargetPos.y = 200.0f;
		}

		// 目標座標を TopBase に渡す（ProcessTopMove 内で補間移動）
		player_->SetCollisionTarget(playerTargetPos);
		enemy->SetCollisionTarget(enemyTargetPos);

		player_->SpinScrape(enemy->GetSpin() * 0.01f);
		enemy->SpinScrape(player_->GetSpin() * 0.01f);

	}
}