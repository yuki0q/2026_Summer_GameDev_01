#include <DxLib.h>
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
//#include "../Manager/EnemyManager.h"
#include "../Manager/Resource.h"
#include "../Object/Actor/NormalStage.h"
#include "../Object/Actor/Charactor/Player.h"
#include "../Object/Collider/ColliderBase.h"
#include "GameScene.h"

GameScene::GameScene(void)
	:
	normalStage_(nullptr),
	player_(nullptr),
	//enemyManager_(nullptr),
	shadowMapHandle_(0),
	SceneBase()
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
	/*enemyManager_ = new EnemyManager(player_);
	enemyManager_->Init();*/

	const ColliderBase* stageCollider =
		normalStage_->GetOwnCollider(static_cast<int>(NormalStage::COLLIDER_TYPE::MODEL));

	// ステージモデルのコライダーをプレイヤーに登録
	player_->AddHitCollider(stageCollider);

	// ステージモデルのコライダーをエネミーに登録
	//enemyManager_->AddHitCollider(stageCollider);

	//// プレイヤーのコライダーをエネミーに登録
	//enemyManager_->AddHitCollider(player_->GetOwnCollider(static_cast<int>(CharactorBase::COLLIDER_TYPE::CAPSULE)));

	// 追従カメラモードに変更
	Camera* camera = sceMng_.GetCamera();
	camera->SetFollow(&normalStage_->GetTransform());
//	camera->SetFollow(&normalStage_->GetTransform());
	camera->ChangeMode(Camera::MODE::FOLLOW);
	camera->AddHitCollider(stageCollider);

	//// シャドウマップハンドルの作成
	shadowMapHandle_ = MakeShadowMap(2048, 2048);

	// シャドウマップが想定するライトの方向もセット
	SetShadowMapLightDirection(shadowMapHandle_, VGet(0.5f, -0.5f, 0.5f));

	// シャドウマップに描画する範囲を設定
	SetShadowMapDrawArea(shadowMapHandle_, VGet(-5000.0f, -70.0f, -6000.0f), VGet(6000.0f, 1500.0f, 8000.0f));
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
	player_->Update();
	//enemyManager_->Update();
}

void GameScene::Draw(void)
{
	// シャドウマップへの描画の準備
	ShadowMap_DrawSetup(shadowMapHandle_);

	player_->Draw();
	//enemyManager_->Draw();
	normalStage_->Draw();

	// シャドウマップへの描画を終了
	ShadowMap_DrawEnd();

	// 描画に使用するシャドウマップを設定
	SetUseShadowMap(0, shadowMapHandle_);

	player_->Draw();
	//enemyManager_->Draw();
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

	/*enemyManager_->Release();
	delete enemyManager_;*/
}
