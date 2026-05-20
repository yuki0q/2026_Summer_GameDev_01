#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
//#include "../Object/Actor/SkyDome.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
	:
	imgTitle_(-1),
	imgPushSpace_(-1),
	animController_(nullptr),
	//skyDome_(nullptr),
	SceneBase()
{
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	// 画像読み込み
	imgTitle_ = resMng_.Load(ResourceManager::SRC::TITLE_IMG).handleId_;

	imgPushSpace_ = resMng_.Load(ResourceManager::SRC::TITLE_PUSH_SPACE).handleId_;

	// プレイヤー
	top_.SetModel(resMng_.Load(ResourceManager::SRC::TOP).handleId_);
	top_.scl = PLAYER_DEFAULT_SCALE;

	top_.quaRot = Quaternion::Identity();
	top_.quaRot = Quaternion::Mult(top_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-90.0f), AsoUtility::AXIS_Y));

	top_.quaRotLocal = Quaternion::Identity();
	top_.quaRotLocal = Quaternion::Mult(top_.quaRotLocal,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));

	top_.pos = PLAYER_DEFAULT_POS;
	top_.Update();
	animController_ = new AnimationController(top_.modelId);
	//animController_->Add(static_cast<int>(ANIM_TYPE::RUN), 30.0f, Application::PATH_MODEL + "Player/Run.mv1");

	animController_->Play(static_cast<int>(ANIM_TYPE::RUN));

	/*skyDome_ = new SkyDome(empty_);
	skyDome_->Init();*/

	// 定点カメラ
	sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);
}

void TitleScene::Update(void)
{
	//SpherePlanet_.quaRot = Quaternion::Mult(SpherePlanet_.quaRot,
	//	Quaternion::AngleAxis(AsoUtility::Deg2RadF(-1.0f), AsoUtility::AXIS_Y));
	////;Euler(0.0f, 0.0f, AsoUtility::Deg2RadF(-1.0f)));

	//SpherePlanet_.Update();
	top_.quaRot = Quaternion::Mult(top_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(10.0f), AsoUtility::AXIS_Y));

	animController_->Play(static_cast<int>(ANIM_TYPE::RUN));

	animController_->Update();

	//skyDome_->Update();

	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
		//sceMng_.ChangeScene(SceneManager::SCENE_ID::TOP_SELECT);
	}
	top_.Update();
}

void TitleScene::Draw(void)
{
	// モデル描画
	//skyDome_->Draw();

	/*MV1DrawModel(bigPlanet_.modelId);

	MV1DrawModel(SpherePlanet_.modelId);*/

	//DrawRotaGraph(IMG_TITLE_POS_X, IMG_TITLE_POS_Y, 1.0f, 0.0f, imgTitle_, true);
	DrawBillboard3D(VGet(0.0f, 0.0f, -50.0f), 0.5f, 0.5f, 1000.0f, 0.0f, imgTitle_, true);
	DrawRotaGraph(IMG_PUSH_SPACE_POS_X, IMG_PUSH_SPACE_POS_Y, 1.0f, 0.0f, imgPushSpace_, true);

	MV1DrawModel(top_.modelId);


}

void TitleScene::Release(void)
{
	DeleteGraph(imgTitle_);
	DeleteGraph(imgPushSpace_);
	//skyDome_->Release();

	delete animController_;
	//delete skyDome_;

	/*bigPlanet_.Release();
	SpherePlanet_.Release();*/
	top_.Release();
}
