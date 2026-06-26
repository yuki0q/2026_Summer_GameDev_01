#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/EnemyManager.h"
#include "../Manager/TopDataManager.h"
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
	isEnd_(false),
	image3(0),
	image2(0),
	image1(0),
	playerScore_(0),
	enemyScore_(0),
	isRoundEnd_(false),
	isRoundProcessed_(false)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	/*TopDataManager* dataMng = new TopDataManager();
	dataMng_.LoadCsvData("TopData.csv");*/

	const TopBase::TopData* playerData = dataMng_.GetTopData
	(static_cast<TopBase::TOP_TYPE>(sceMng_.GetPlayerTopType()));

	// プレイヤー
	player_ = new Player(*playerData);
	player_->Init();

	// ステージ
	normalStage_ = new NormalStage();
	normalStage_->Init();

	// エネミー管理
	enemyManager_ = new EnemyManager(player_, dataMng_);
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
	isEnd_ = false;

	// スコアのリセット
	playerScore_ = 0;
	enemyScore_ = 0;
	isRoundEnd_ = false;
	isRoundProcessed_ = false;

	image3 = resMng_.Load(ResourceManager::SRC::IMAGE_3).handleId_;
	image2 = resMng_.Load(ResourceManager::SRC::IMAGE_2).handleId_;
	image1 = resMng_.Load(ResourceManager::SRC::IMAGE_1).handleId_;
}

void GameScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_ESCAPE) || 
		ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::PAUSE);
	}

	if (countTime_ > 0)
	{
		countTime_--;
	}
	else {
		isStart_ = true;
	}

	for (auto& enemy : enemyManager_->GetEemies())
	{
		if (player_->IsGameEnd() || enemy->IsGameEnd()) {
			isRoundEnd_ = true;

			// 敵を場外に飛ばしたかスピンが無くなったかを判定
			if (enemy->IsGameEnd() && !player_->IsGameEnd())
			{
				playerScore_ += 1;
			}
			// プレイヤーが負けた場合
			else if (player_->IsGameEnd() && !enemy->IsGameEnd())
			{
				enemyScore_ += 1;
			}
			// 3. 同時（引き分け）
			else {
				playerScore_ += 1;
				enemyScore_ += 1;
			}

			break;
		}
	}

	if (isRoundEnd_ && !isEnd_)
	{
		if (playerScore_ >= 3 || enemyScore_ >= 3)
		{
			isEnd_ = true;
		}
		else
		{
			// 4点未満なら、各オブジェクトに用意されている「Respawn」を呼び出して次ラウンドへ
			player_->Respawn();
			for (auto& enemy : enemyManager_->GetEemies()) {
				enemy->Respawn();
			}
			// カウントダウンを再設定して仕切り直し
			countTime_ = 180;
			isStart_ = false;
			isRoundEnd_ = false;
		}
	}

	normalStage_->Update();

	if (isEnd_) {
		//sceMng_.SetPlayerWin(player_->IsGameEnd()); // 勝利フラグをセット
		if (playerScore_ >= 3) { sceMng_.SetPlayerWin(true); }
		else if(enemyScore_ >= 3){ sceMng_.SetPlayerWin(false); }
		sceMng_.ChangeScene(SceneManager::SCENE_ID::RESULT);
	}

	if (isStart_ && !isRoundEnd_ && !isEnd_) {
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

	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, shadowMapHandle_);

	player_->Draw();
	enemyManager_->Draw();
	normalStage_->Draw();

	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);

	player_->DrawImage();

	for (auto& enemy : enemyManager_->GetEemies()) {
		enemy->DrawImage();
	}

	// 3Dの奥行き判定を一時的に無効化する
	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);

	if (!isStart_) {
		int i = countTime_ / 60;
		switch (i)
		{
		case 2:
			DrawBillboard3D(VGet(0.0f, 250.0f, 0.0f), 0.5f, 0.5f, 500.0f, 0.0f, image3, TRUE);
			//DrawRotaGraph(640, 260, 1.0f, 0.0f, image3, true);
			break;
		case 1:
			DrawBillboard3D(VGet(0.0f, 250.0f, 0.0f), 0.5f, 0.5f, 500.0f, 0.0f, image2, TRUE);
			break;
		case 0:
			DrawBillboard3D(VGet(0.0f, 250.0f, 0.0f), 0.5f, 0.5f, 500.0f, 0.0f, image1, TRUE);
			break;
		}
	}

	// 終わったら3D用に設定を戻す
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
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

		// どちらかがリスポーン直後なら、めり込み補正や反発速度計算を一切行わずにスルーする
		if (player_->IsRespawning() || enemy->IsRespawning())continue;

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

			playerNewVel = VScale(VSub(player_->GetVel(), 
				VScale(impulse, 1.0f / player_->GetWeight())), 
				enemy->GetTopsShock());
			enemyNewVel = VScale(VAdd(enemy->GetVel(), 
				VScale(impulse, 1.0f / enemy->GetWeight())), 
				player_->GetTopsShock());

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
			VECTOR i = player_->GetVel();
			playerNewVel = VScale(VSub(player_->GetVel(), 
				VScale(frictionImpulse, 1.0f / player_->GetWeight())), 
				enemy->GetTopsShock());
			enemyNewVel = VScale(VAdd(enemy->GetVel(), 
				VScale(frictionImpulse, 1.0f / enemy->GetWeight())), 
				player_->GetTopsShock());
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

		// スピンが50以下の時、押し出す力を強くする
		if (player_->GetSpin() <= 50.0f || enemy->GetSpin() <= 50.0f)
		{
			// 押し出しの基本強度
			float pushPower = 150.0f;

			if (player_->GetTopType() == TopBase::TOP_TYPE::DEFENSE && player_->IsShielding())
			{
				// プレイヤーが防御型でシールド中なら、自分が受けるノックバックをほぼゼロに
				pushPower = 0.1f;
				// さらに相手（エネミー）に追加のスピン減少ダメージを与える
				enemy->SpinScrape(10.0f);
			}

			if (enemy->GetTopType() == TopBase::TOP_TYPE::DEFENSE && player_->IsShielding())
			{
				// プレイヤーが防御型でシールド中なら、自分が受けるノックバックをほぼゼロに
				pushPower = 0.1f;
				// さらに相手（エネミー）に追加のスピン減少ダメージを与える
				player_->SpinScrape(5.0f);
			}

			// プレイヤーは法線と「逆」方向（引かれる方向）に強く押し出す
			playerTargetPos = VSub(playerTargetPos, VScale(normal, pushPower));

			// エネミーは法線方向（進む方向）に強く押し出す
			enemyTargetPos = VAdd(enemyTargetPos, VScale(normal, pushPower));

			// ついでにノックバック感を出すために、速度ベクトル（慣性）も少し外側に上書き付与
			player_->SetVel(VSub(player_->GetVel(), VScale(normal, 
				pushPower * enemy->GetTopsShock())));
			enemy->SetVel(VAdd(enemy->GetVel(), VScale(normal, 
				pushPower * player_->GetTopsShock())));
		}

		// コマが倒れかけているとき、押し出す力を弱くする
		if (player_->IsDying() || enemy->IsDying())
		{
			// 押し出しの基本強度
			float pushPower = 5.0f;

			// プレイヤーは法線と「逆」方向（引かれる方向）に強く押し出す
			playerTargetPos = VSub(playerTargetPos, VScale(normal, pushPower));

			// エネミーは法線方向（進む方向）に強く押し出す
			enemyTargetPos = VAdd(enemyTargetPos, VScale(normal, pushPower));
		}

		if(playerTargetPos.y > 200.0f)
		{
			playerTargetPos.y = 200.0f;
		}

		if(enemyTargetPos.y > 200.0f)
		{
			enemyTargetPos.y = 200.0f;
		}

		// 目標座標を TopBase に渡す（ProcessTopMove 内で補間移動）
		/*player_->SetCollisionTarget(playerTargetPos);
		enemy->SetCollisionTarget(enemyTargetPos);*/

		if (!player_->GetCollisionTarget_())
			player_->SetCollisionTarget(playerTargetPos);
		if (!enemy->GetCollisionTarget_())
			enemy->SetCollisionTarget(enemyTargetPos);

		player_->SpinScrape(VSize(VSub(playerTargetPos,
			player_->GetTransform().pos))*
			enemy->GetSpin() * 0.0001f);
		enemy->SpinScrape(VSize(VSub(enemyTargetPos,
			enemy->GetTransform().pos))*
			enemy->GetSpin() * 0.0001f);

	}
}
