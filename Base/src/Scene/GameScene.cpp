#include <DxLib.h>
#include <EffekseerForDXLib.h>
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
#include "../Application.h"
#include "../Effect/EffekseerEffect.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	normalStage_(nullptr),
	player_(nullptr),
	player2_(nullptr),
	enemyManager_(nullptr),
	shadowMapHandle_(0),
	SceneBase(),
	countTime_(0),
	isStart_(false),
	isEnd_(false),
	image3(0),
	image2(0),
	image1(0),
	imgWin_(0),
	playerScore_(0),
	enemyScore_(0),
	isRoundEnd_(false),
	isRoundProcessed_(false),
	lastRoundWinner_(0),
	playerCount_(0),
	GameBGM_(0),
	timeScale_(1.0f),
	slowMotionTimer_(0.0f),
	contactPoint_(0.0f,0.0f,0.0f)
{
}

GameScene::~GameScene(void)
{
}

void GameScene::Init(void)
{
	EffekseerEffect::GetInstance()->CreateInstance();
	EffekseerEffect::GetInstance()->Init();
	/*TopDataManager* dataMng = new TopDataManager();
	dataMng_.LoadCsvData("TopData.csv");*/

	playerCount_ = sceMng_.GetPlayerNo();

	const TopBase::TopData* playerData = dataMng_.GetTopData
	(static_cast<TopBase::TOP_TYPE>(sceMng_.GetPlayerTopType()));

	// プレイヤー
	player_ = new Player(*playerData, 1);
	player_->Init();

	// ステージ
	normalStage_ = new NormalStage();
	normalStage_->Init();
	const ColliderBase* stageCollider =
		normalStage_->GetOwnCollider(static_cast<int>(NormalStage::COLLIDER_TYPE::MODEL));

	// ステージモデルのコライダーをプレイヤーに登録
	player_->AddHitCollider(stageCollider);

	// 人数に応じた生成とコライダー登録の分岐
	if (playerCount_ == 2)
	{
		const TopBase::TopData* player2Data = dataMng_.GetTopData
		(static_cast<TopBase::TOP_TYPE>(sceMng_.GetPlayerTopType2P()));

		player2_ = new Player(*player2Data, 2);
		player2_->Init();

		// 2Pの初期位置をエネミー側（対面）に設定する
		// ※Playerクラスに初期位置を設定する関数、もしくはInitの引数で渡せるようにしておくと便利です
		// player2_->SetRespawnPos(VGet(0.0f, 75.0f, 500.0f)); 
		// player2_->Respawn();

		// 2Pにステージコライダーを登録
		player2_->AddHitCollider(stageCollider);

		// 1Pと2Pのコライダーを互いに登録
		player_->AddHitCollider(player2_->GetOwnCollider(static_cast<int>(CharactorBase::COLLIDER_TYPE::CAPSULE)));
		player2_->AddHitCollider(player_->GetOwnCollider(static_cast<int>(CharactorBase::COLLIDER_TYPE::CAPSULE)));
	}
	else 
	{
		// エネミー管理
		enemyManager_ = new EnemyManager(player_, dataMng_);
		enemyManager_->Init();

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
	}

	// 追従カメラモードに変更
	Camera* camera = sceMng_.GetCamera();
	//camera->SetFollow(&player_->GetTransform());
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
	imgWin_ = resMng_.Load(ResourceManager::SRC::IMAGE_WIN).handleId_;

	GameBGM_ = LoadSoundMem("Data/Music/GameScene.mp3");
	PlaySoundMem(GameBGM_, DX_PLAYTYPE_LOOP, true);
	ChangeVolumeSoundMem(150, GameBGM_);

	timeScale_ = 1.0f;
	slowMotionTimer_ = 0.0f;
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

	// スローモーションタイマーの更新
	if (slowMotionTimer_ > 0.0f)
	{
		// 1フレームあたりの秒数（1/60秒とする）に timeScale を考慮して減算
		slowMotionTimer_ -= (1.0f / 60.0f);
		timeScale_ = 0.3f; // 通常の15%の速度

		if (slowMotionTimer_ <= 0.0f)
		{
			slowMotionTimer_ = 0.0f;
			timeScale_ = 1.0f; // 元に戻す

			// カメラを元に戻す指示
			Camera* camera = sceMng_.GetCamera();
			camera->SetFollow(&normalStage_->GetTransform()); // 追従対象をステージ中心に戻す
		}
	}
	else
	{
		timeScale_ = 1.0f;
	}

	if (countTime_ > 0)
	{
		countTime_--;
	}
	else {
		isStart_ = true;
	}

	if (playerCount_ == 2)
	{
		// 2人プレイ時 1Pと2Pの決着判定
		if (player_->IsGameEnd() || player2_->IsGameEnd())
		{
			isRoundEnd_ = true;

			// 2Pの負け（1Pの勝ち）
			if (player2_->IsGameEnd() && !player_->IsGameEnd())
			{
				playerScore_ += 1;
				lastRoundWinner_ = 1;
			}
			// 1Pの負け（2Pの勝ち）
			else if (player_->IsGameEnd() && !player2_->IsGameEnd())
			{
				enemyScore_ += 1; // enemyScore_を2P用に使用
				lastRoundWinner_ = 2;
			}
			else
			{
				playerScore_ += 1; 
				enemyScore_ += 1;
				lastRoundWinner_ = 0;
			}
		}
	}
	else
	{
		for (auto& enemy : enemyManager_->GetEemies())
		{
			if (player_->IsGameEnd() || enemy->IsGameEnd()) {
				isRoundEnd_ = true;

				// 敵を場外に飛ばしたかスピンが無くなったかを判定
				if (enemy->IsGameEnd() && !player_->IsGameEnd())
				{
					playerScore_ += 1;
					lastRoundWinner_ = 1;
				}
				// プレイヤーが負けた場合
				else if (player_->IsGameEnd() && !enemy->IsGameEnd())
				{
					enemyScore_ += 1;
					lastRoundWinner_ = 2;
				}
				// 3. 同時（引き分け）
				else {
					playerScore_ += 1;
					enemyScore_ += 1;
					lastRoundWinner_ = 0;
				}
				break;
			}
		}
	}

	if (isRoundEnd_ && !isEnd_)
	{
		slowMotionTimer_ = 0.0f;
		timeScale_ = 1.0f;

		if (playerScore_ >= 3 || enemyScore_ >= 3)
		{
			isEnd_ = true;
		}
		else
		{
			// 4点未満なら、各オブジェクトに用意されている「Respawn」を呼び出して次ラウンドへ
			player_->Respawn();
			if (playerCount_ == 2) {
				player2_->Respawn();
			}
			else {
				for (auto& enemy : enemyManager_->GetEemies()) {
					enemy->Respawn();
				}
			}
			// カウントダウンを再設定して仕切り直し
			countTime_ = 180;
			isStart_ = false;
			isRoundEnd_ = false;
		}
	}

	static float updateAccumulator = 0.0f;
	updateAccumulator += timeScale_;

	if (updateAccumulator >= 1.0f)
	{
		updateAccumulator -= 1.0f;

		if (isStart_ && !isRoundEnd_ && !isEnd_) {
			player_->Update();
			if (playerCount_ == 2) {
				player2_->Update();
			}
			else {
				enemyManager_->Update();
			}
			Collision();
		}
	}
	else
	{
		// スロー中、スキップされたフレームでもコリジョン解決後の滑らかな「補間移動」などがある場合は
		// 最低限の更新を行う、もしくは完全に更新を止める（今回は完全停止でスローを表現）
	}
	normalStage_->Update();

	if (isEnd_) {
		//sceMng_.SetPlayerWin(player_->IsGameEnd()); // 勝利フラグをセット
		if (playerScore_ >= 3) { sceMng_.SetPlayerWin(true); }
		else if(enemyScore_ >= 3){ sceMng_.SetPlayerWin(false); }
		EffekseerEffect::GetInstance()->ClearAllTrails();
		sceMng_.ChangeScene(SceneManager::SCENE_ID::RESULT);
		StopSoundMem(GameBGM_);
	}

	
}

void GameScene::Draw(void)
{
	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(shadowMapHandle_);

	normalStage_->Draw();

	if (!isRoundEnd_ || lastRoundWinner_ == 1 || lastRoundWinner_ == 0) {
		player_->Draw();
	}
	if (playerCount_ == 2) {
		if (!isRoundEnd_ || lastRoundWinner_ == 2 || lastRoundWinner_ == 0) player2_->Draw();
	}
	else {
		if (!isRoundEnd_ || lastRoundWinner_ == 2 || lastRoundWinner_ == 0) enemyManager_->Draw();
	}
	

	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, shadowMapHandle_);

	normalStage_->Draw();

	if (!isRoundEnd_ || lastRoundWinner_ == 1 || lastRoundWinner_ == 0) {
		player_->Draw();
	}
	if (playerCount_ == 2) {
		if (!isRoundEnd_ || lastRoundWinner_ == 2 || lastRoundWinner_ == 0) player2_->Draw();
	}
	else {
		if (!isRoundEnd_ || lastRoundWinner_ == 2 || lastRoundWinner_ == 0) enemyManager_->Draw();
	}
	

	// 描画に使用するシャドウマップの設定を解除
	SetUseShadowMap(0, -1);

	if (!isRoundEnd_ || lastRoundWinner_ == 1 || lastRoundWinner_ == 0) {
		player_->DrawImage();
	}
	if (playerCount_ == 2)
	{
		if (!isRoundEnd_ || lastRoundWinner_ == 2 || lastRoundWinner_ == 0) player2_->DrawImage();
	}
	else {
		for (auto& enemy : enemyManager_->GetEemies()) {
			if (!isRoundEnd_ || lastRoundWinner_ == 2 || lastRoundWinner_ == 0) enemy->DrawImage();
		}
	}

	// 3Dの奥行き判定を一時的に無効化する
	SetUseZBuffer3D(FALSE);
	SetWriteZBuffer3D(FALSE);

	for (int i = 0; i < playerScore_; i++)
	{
		float winX = 120.0f + (i * 60.0f);
		float winY = Application::SCREEN_SIZE_Y - 150.0f;
		DrawRotaGraph(winX, winY, 0.15f, 0.0f, imgWin_, TRUE);
	}

	// 2P / エネミー側の勝星描画
	for (int i = 0; i < enemyScore_; i++)
	{
		float winX = (Application::SCREEN_SIZE_X - 180.0f) + (i * 60.0f);
		float winY = Application::SCREEN_SIZE_Y - 150.0f;
		DrawRotaGraph(winX, winY, 0.15f, 0.0f, imgWin_, TRUE);
	}

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

	if (player2_) {
		player2_->Release();
		delete player2_;
	}

	if (enemyManager_) {
		enemyManager_->Release();
		delete enemyManager_;
	}

	DeleteGraph(image3);
	DeleteGraph(image2);
	DeleteGraph(image1);
	DeleteGraph(imgWin_);
	DeleteSoundMem(GameBGM_);

	EffekseerEffect::GetInstance()->DeleteInstance();
}

void GameScene::Collision(void) 
{
	CollisionResolve();

}

// コマ同士の衝突
void GameScene::CollisionResolve(void)
{
	if (playerCount_ == 2)
	{
		// 2Pモード 1Pと2Pの衝突を計算
		ResolveTopToTop(player_, player2_);
	}
	else
	{
		// 1Pモード 1Pとすべてのエネミーの衝突を計算
		for (auto& enemy : enemyManager_->GetEemies())
		{
			ResolveTopToTop(player_, enemy);
		}
	}
}

void GameScene::ResolveTopToTop(TopBase* topA, TopBase* topB)
{
	VECTOR diff = VSub(topB->GetTransform().pos, topA->GetTransform().pos);
	float dist = VSize(diff);
	float hitDist = topA->GetRadius() + topB->GetRadius();

	// 衝突していない、または重なりすぎている場合は無視
	if (dist >= hitDist || dist <= 0.0001f) return;

	// どちらかがリスポーン直後なら、めり込み補正や反発速度計算を一切行わずにスルーする
	if (topA->IsRespawning() || topB->IsRespawning())return;

	VECTOR dirAtoB = { diff.x / dist, diff.y / dist, diff.z / dist };

	// Aの中心から、Aの半径分だけBの方向に進んだ位置を接触点とする
	contactPoint_ = VAdd(topA->GetPos(), VScale(dirAtoB, topA->GetRadius()));

	EffekseerEffect::GetInstance()->FireLightEffect(contactPoint_);

	// 衝突応答 (物理演算)
	//VECTOR normal = VDiv(diff, dist); // VNormの代わり（distが判明しているので効率的）
	VECTOR normal = { diff.x / dist,diff.y / dist,diff.z / dist };
	VECTOR rv = VSub(topB->GetVel(), topA->GetVel());   // 相対速度
	float normalSpeed = VDot(rv, normal);	// 法線成分

	VECTOR tangent = VSub(rv, VScale(normal, normalSpeed));

	float tangentSpeed = VSize(tangent);	// 接線成分

	// 衝突後の速度計算
	VECTOR playerNewVel, enemyNewVel;

	// 正面よりの時
	if (normalSpeed > tangentSpeed)
	{
		// 反発係数
		float e = 0.8f;

		// 衝撃量 j の計算
		float j = -(1.0f + e) * normalSpeed;
		j /= (1.0f / topA->GetWeight() + 1.0f / topB->GetWeight());

		// 衝撃ベクトルを適用
		VECTOR impulse = VScale(normal, j);

		// スピン量に応じて少しだけ反発力を強化
		float spinFactor = (topA->GetSpin() + topB->GetSpin()) * 0.001f;
		impulse = VScale(impulse, 1.0f + spinFactor);

		float impulseMag = VSize(impulse);
		// プレイヤーは法線の逆方向へ傾く
		VECTOR negNormal = VScale(normal, -1.0f);
		topA->AddCollisionTilt(negNormal, impulseMag);
		// エネミーは法線方向へ傾く
		topB->AddCollisionTilt(normal, impulseMag);

			/*player_->SetVel(VScale(VSub(player_->GetVel(), VScale(impulse, 1.0f / player_->GetWeight())), 0.05f));
			enemy->SetVel(VScale(VAdd(enemy->GetVel(), VScale(impulse, 1.0f / enemy->GetWeight())), 0.05f));*/

		playerNewVel = VScale(VSub(topA->GetVel(),
			VScale(impulse, 1.0f / topA->GetWeight())),
			topB->GetTopsShock());
		enemyNewVel = VScale(VAdd(topB->GetVel(),
			VScale(impulse, 1.0f / topB->GetWeight())),
			topA->GetTopsShock());

	}
	else {	// 擦りよりの時
		tangent = VNorm(tangent);	// 正規化
		float friction = 0.3f;	// 摩擦係数
		float vt = VDot(rv, tangent);	// 接線速度
		float jt = -vt / (1.0f / topA->GetWeight() + 1.0f / topB->GetWeight());	// 摩擦速度

		jt *= friction;

		VECTOR frictionImpulse = VScale(tangent, jt);

		// スピン量に応じて少しだけ反発力を強化
		float spinFactor = (topA->GetSpin() + topB->GetSpin()) * 0.001f;
		frictionImpulse = VScale(frictionImpulse, 1.0f + spinFactor);

		float impulseMag = VSize(frictionImpulse);
		topA->AddCollisionTilt(VScale(tangent, -1.0f), impulseMag * 0.5f);
		topB->AddCollisionTilt(tangent, impulseMag * 0.5f);

			/*player_->SetVel(VScale(VSub(player_->GetVel(),
				VScale(frictionImpulse, 1.0f / player_->GetWeight())), 0.05f));

			enemy->SetVel(VScale(VAdd(enemy->GetVel(),
				VScale(frictionImpulse, 1.0f / enemy->GetWeight())), 0.05f));*/
		//VECTOR i = topA->GetVel();
		playerNewVel = VScale(VSub(topA->GetVel(),
			VScale(frictionImpulse, 1.0f / topA->GetWeight())),
			topB->GetTopsShock());
		enemyNewVel = VScale(VAdd(topB->GetVel(),
			VScale(frictionImpulse, 1.0f / topB->GetWeight())),
			topA->GetTopsShock());
	}

	// めり込み補正 (Positional Correction)
	// これがないと、コマ同士が重なったまま震える現象が起きます
	float percent = 0.5f; // 補正の強さ
	float slop = 0.01f;   // 許容誤差

	VECTOR correction = VScale(normal, (fmaxf(hitDist - dist - slop, 0.0f) /
		(1.0f / topA->GetWeight() + 1.0f / topB->GetWeight())) * percent);

	// 速度を確定・目標座標を計算して渡す
	topA->SetVel(playerNewVel);
	topB->SetVel(enemyNewVel);

	VECTOR playerTargetPos = VSub(
		VAdd(topA->GetPos(), playerNewVel),
		VScale(correction, 1.0f / topA->GetWeight()));

	VECTOR enemyTargetPos = VAdd(
		VAdd(topB->GetPos(), enemyNewVel),
		VScale(correction, 1.0f / topB->GetWeight()));

	// スピンが50以下の時、押し出す力を強くする
	if (topA->GetSpin() <= 50.0f || topB->GetSpin() <= 50.0f)
	{
		// 押し出しの基本強度
		float pushPower = 150.0f;

		if (topA->GetTopType() == TopBase::TOP_TYPE::DEFENSE && topA->IsShielding())
		{
			// プレイヤーが防御型でシールド中なら、自分が受けるノックバックをほぼゼロに
			pushPower = 0.1f;
			// さらに相手に追加のダメージを与える
			topB->SpinScrape(8.0f);
		}

		if (topB->GetTopType() == TopBase::TOP_TYPE::DEFENSE && topA->IsShielding())
		{
			// プレイヤーが防御型でシールド中なら、自分が受けるノックバックをほぼゼロに
			pushPower = 0.1f;
			// さらに相手（エネミー）に追加のスピン減少ダメージを与える
			topA->SpinScrape(5.0f);
		}

		// プレイヤーは法線と逆方向に強く押し出す
		playerTargetPos = VSub(playerTargetPos, VScale(normal, pushPower));

		// エネミーは法線方向に強く押し出す
		enemyTargetPos = VAdd(enemyTargetPos, VScale(normal, pushPower));

		// ついでにノックバック感を出すために、速度ベクトル（慣性）も少し外側に上書き付与
		topA->SetVel(VSub(topA->GetVel(), VScale(normal,
			pushPower * topB->GetTopsShock())));
		topB->SetVel(VAdd(topB->GetVel(), VScale(normal,
			pushPower * topA->GetTopsShock())));
	}

	// コマが倒れかけているとき、押し出す力を弱くする
	if (topA->IsDying() || topB->IsDying())
	{
		// 押し出しの基本強度
		float pushPower = 5.0f;

		// プレイヤーは法線と逆方向に強く押し出す
		playerTargetPos = VSub(playerTargetPos, VScale(normal, pushPower));

		// エネミーは法線方向に強く押し出す
		enemyTargetPos = VAdd(enemyTargetPos, VScale(normal, pushPower));
	}

	if (playerTargetPos.y > 200.0f)
	{
		playerTargetPos.y = 200.0f;
	}

	if (enemyTargetPos.y > 200.0f)
	{
		enemyTargetPos.y = 200.0f;
	}

		// 目標座標を TopBase に渡す（ProcessTopMove 内で補間移動）
		/*player_->SetCollisionTarget(playerTargetPos);
		enemy->SetCollisionTarget(enemyTargetPos);*/

	if (!topA->GetCollisionTarget_())
		topA->SetCollisionTarget(playerTargetPos);
	if (!topB->GetCollisionTarget_())
		topB->SetCollisionTarget(enemyTargetPos);

	topA->SpinScrape(VSize(VSub(playerTargetPos,
		topA->GetTransform().pos)) *
		topB->GetSpin() * 0.0001f);
	topB->SpinScrape(VSize(VSub(enemyTargetPos,
		topB->GetTransform().pos)) *
		topA->GetSpin() * 0.0001f);

	float playerDistFromCenter = VSize({ playerTargetPos.x, 0.0f, playerTargetPos.z });
	float enemyDistFromCenter = VSize({ enemyTargetPos.x, 0.0f, enemyTargetPos.z });

	VECTOR target;

	if (playerDistFromCenter > STAGE_OUT_RADIUS) {
		target = { playerTargetPos.x,0.0f,playerTargetPos.z };
	}
	else if (enemyDistFromCenter > STAGE_OUT_RADIUS) {
		target = { enemyTargetPos.x,0.0f,enemyTargetPos.z };
	}

	if (playerDistFromCenter > STAGE_OUT_RADIUS || enemyDistFromCenter > STAGE_OUT_RADIUS)
	{
		// すでにスロー中でなければ、スローを開始する
		if (slowMotionTimer_ <= 0.0f)
		{
			slowMotionTimer_ = 0.5f; // 1.5秒間スローにする（実時間は 1.5 / 0.15 = 10秒 ではなく、スロータイマーの減算速度通りに終了します）

			// カメラに一時的な注視ポイントを認識させる
			Camera* camera = sceMng_.GetCamera();

			// 1. カメラを接触した瞬間（contactPoint_）へ寄せるための「ダミーのTransform」をシーン上で作成するか、
			// もしくはCamera自体に専用の関数「SetTemporaryFocus」を追加して直接位置を教える。
			// 今回は最も既存コードを壊さないように、Cameraに新しく「ズーム演出用の位置」を直接渡す設計が綺麗です。
			camera->TriggerZoomIn(contactPoint_, target, 1.5f);
		}
	}
}
