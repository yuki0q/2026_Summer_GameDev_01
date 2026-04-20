#include <string>
#include <DxLib.h>
#include "../Application.h"
#include "../Utility/MatrixUtility.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/SceneManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/Camera.h"
#include "../Common/AnimationController.h"
#include "TitleScene.h"

TitleScene::TitleScene(void)
{
	logoTitle_ = -1;
	imgTitle_ = -1;
	bgmTitle_ = -1;
}

TitleScene::~TitleScene(void)
{
}

void TitleScene::Init(void)
{
	logoTitle_ = LoadGraph((Application::PATH_IMAGE + "TitleLogo.png").c_str());
	imgTitle_ = LoadGraph((Application::PATH_IMAGE + "TitleImg.png").c_str());
	bgmTitle_ = LoadSoundMem((Application::PATH_MUSIC + "TitleBgm.mp3").c_str());

	modelId_ = MV1LoadModel((Application::PATH_MODEL + "Top/NormalTop.mv1").c_str());

	animationController_ = new AnimationController(modelId_);

	animationController_->Add(static_cast<int>(ANIM_TYPE::IDLE), 100.0f,
		Application::PATH_MODEL + "Top/Idle.mv1");

	animationController_->Add(static_cast<int>(ANIM_TYPE::ATTACK), 30.0f,
		Application::PATH_MODEL + "Top/Attack.mv1");

	animationController_->Play(static_cast<int>(ANIM_TYPE::IDLE));

	pos_ = { 25.0f, 150.0f, -200.0f };
	scale_ = { 0.5f,0.5f,0.5f };
	angles_ = { 0.0f, 0.0f, 0.0f };
	localAngles_ = { AsoUtility::Deg2RadF(-15.0f), AsoUtility::Deg2RadF(30.0f), 0.0f };

	// パンの設定
	//ChangePanSoundMem(105, bgmTitle_);

	// 音量ボリュームの設定
	ChangeVolumeSoundMem(240, bgmTitle_);

	// BGMループ
	PlaySoundMem(bgmTitle_, DX_PLAYTYPE_LOOP);

	Camera* camera = SceneManager::GetInstance().GetCamera();
	camera->ChangeMode(Camera::MODE::FIXED_POINT);
	camera->Init();

	// 大きさをモデルに反映
	MV1SetScale(modelId_, scale_);

	// 角度から方向に変換する
	moveDir_ = { sinf(angles_.y), 0.0f, cosf(angles_.y) };
	//preInputDir_ = moveDir_;

	// 行列の合成(子, 親と指定すると親⇒子の順に適用される)
	MATRIX mat = MatrixUtility::Multiplication(localAngles_, angles_);

	// 回転行列をモデルに反映
	MV1SetRotationMatrix(modelId_, mat);

	// 座標をモデルに反映
	MV1SetPosition(modelId_, pos_);
	MV1SetScale(modelId_, scale_);
}

void TitleScene::Update(void)
{
	// シーン遷移
	InputManager& ins = InputManager::GetInstance();
	bool isHitKey = ins.IsNew(KEY_INPUT_SPACE) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
		InputManager::JOYPAD_BTN::DOWN);

	if (isHitKey)
	{
		animationController_->Play(static_cast<int>(ANIM_TYPE::ATTACK), false);
	}

	if (animationController_->IsEnd()) {
		// 再生終了
		StopSoundMem(bgmTitle_);
		SceneManager::GetInstance().ChangeScene(SceneManager::SCENE_ID::GAME);
	}

	MV1SetPosition(modelId_, pos_);
	MV1SetScale(modelId_, scale_);
	animationController_->Update();
}

void TitleScene::Draw(void)
{
	MV1DrawModel(modelId_);

	DrawBillboard3D({ 0.0f,200.0f,-300.0f },
		0.5f, 0.5f, 500.0f, 0.0, logoTitle_, true);

	DrawBillboard3D({ 0.0f,200.0f,-300.0f },
		0.5f, 0.5f, 500.0f, 0.0, imgTitle_, true);
}

void TitleScene::Release(void)
{
	MV1DeleteModel(modelId_);

	DeleteGraph(imgTitle_);
	DeleteGraph(logoTitle_);
	DeleteSoundMem(bgmTitle_);

	delete animationController_;
}
