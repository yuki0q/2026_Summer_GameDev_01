#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Manager/SoundManager.h"
#include "../Application.h"
#include "GameScene.h"
#include "ResultScene.h"

ResultScene::ResultScene(void)
	:
	imgResultWin_(0),
	imgResultLose_(0),
	imgResultBackWin_(0),
	imgResultBackLose_(0),
	imgResult1PWin_(0),
	imgResult2PWin_(0),
	bgmResultLose_(0),
	SceneBase()
{
}

ResultScene::~ResultScene(void)
{
}

void ResultScene::Init(void)
{
	// SceneManagerからGameSceneが保存した勝敗フラグを受け取る
	isWin_ = sceMng_.IsPlayerWin();

	// 敗北時のリソースを読み込む
	imgResultLose_ = resMng_.Load(ResourceManager::SRC::RESULT_LOSE).handleId_;

	//imgResultBackLose_ = LoadGraph("Data/Image/I win.png");
	imgResultBackWin_ = resMng_.Load(ResourceManager::SRC::RESULT_IMG_WIN).handleId_;
	imgResultBackLose_ = resMng_.Load(ResourceManager::SRC::RESULT_IMG_LOSE).handleId_;

	// 勝利時のリソースを読み込む
	imgResultWin_ = resMng_.Load(ResourceManager::SRC::RESULT_WIN).handleId_;

	imgResult1PWin_ = resMng_.Load(ResourceManager::SRC::RESULT_1P_WIN).handleId_;

	imgResult2PWin_ = resMng_.Load(ResourceManager::SRC::RESULT_2P_WIN).handleId_;

	// タイトル画面に必要なサウンドをロード
	SoundManager::GetInstance()->LoadSceneSound(LoadScene::RESULT);

	// タイトルBGMを再生（自動でループ再生されます）
	SoundManager::GetInstance()->PlayBGM(SoundID::BGM_RESULT);
	SoundManager::GetInstance()->SetBgmVolume(160);
}

void ResultScene::Update(void)
{
	// シーン遷移
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE)
		|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)
		|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD2, InputManager::JOYPAD_BTN::DOWN))
	{
		SoundManager::GetInstance()->PlaySE(SoundID::SE_BUTTON);
		SoundManager::GetInstance()->StopBGM();
		SoundManager::GetInstance()->DeleteSceneSound(LoadScene::RESULT);
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}

	if (ins.IsTrgDown(KEY_INPUT_R)
		|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
	{
		SoundManager::GetInstance()->PlaySE(SoundID::SE_BUTTON);
		SoundManager::GetInstance()->StopBGM();
		SoundManager::GetInstance()->DeleteSceneSound(LoadScene::RESULT);
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}

	if (ins.IsTrgDown(KEY_INPUT_S)
		|| ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
	{
		SoundManager::GetInstance()->PlaySE(SoundID::SE_BUTTON);
		SoundManager::GetInstance()->StopBGM();
		SoundManager::GetInstance()->DeleteSceneSound(LoadScene::RESULT);
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TOP_SELECT);
	}
}

void ResultScene::Draw(void)
{
	if (sceMng_.GetPlayerNo() == 1) {
		if (isWin_)
		{
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 0.9f, 0.0f, imgResultBackWin_, true);
			//DrawBillboard3D(VGet(0.0f, 0.0f, -50.0f), 0.5f, 0.5f, 6100.0f, 0.0f, imgResultBackWin_, TRUE);
			//DrawBillboard3D(VGet(0.0f, 0.0f, -55.0f), 0.5f, 0.5f, 2500.0f, 0.0f, imgResultWin_, TRUE);
		}
		else
		{
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 0.9f, 0.0f, imgResultBackLose_, true);
			//DrawBillboard3D(VGet(0.0f, 0.0f, -50.0f), 0.5f, 0.5f, 6100.0f, 0.0f, imgResultBackLose_, TRUE);
			//DrawBillboard3D(VGet(0.0f, 0.0f, -55.0f), 0.5f, 0.5f, 2500.0f, 0.0f, imgResultLose_, TRUE);
		}
	}
	else if (sceMng_.GetPlayerNo() == 2) {
		if (isWin_)
		{
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 0.9f, 0.0f, imgResult1PWin_, true);
			//DrawBillboard3D(VGet(0.0f, 0.0f, 0.0f), 0.5f, 0.5f, 4000.0f, 0.0f, imgResultBackLose_, TRUE);
			//DrawBillboard3D(VGet(0.0f, 0.0f, -50.0f), 0.5f, 0.5f, 6100.0f, 0.0f, imgResultBackWin_, TRUE);
			//DrawBillboard3D(VGet(0.0f, -15.0f, -55.0f), 0.5f, 0.5f, 2500.0f, 0.0f, imgResult1PWin_, TRUE);
		}
		else
		{
			DrawRotaGraph(Application::SCREEN_SIZE_X / 2, Application::SCREEN_SIZE_Y / 2, 0.9f, 0.0f, imgResult2PWin_, true);
			//DrawBillboard3D(VGet(0.0f, 0.0f, -50.0f), 0.5f, 0.5f, 6100.0f, 0.0f, imgResultBackWin_, TRUE);
			//DrawBillboard3D(VGet(0.0f, -15.0f, -55.0f), 0.5f, 0.5f, 2500.0f, 0.0f, imgResult2PWin_, TRUE);
		}
	}
}

void ResultScene::Release(void)
{
	DeleteGraph(imgResultWin_);
	DeleteGraph(imgResult1PWin_);
	DeleteGraph(imgResult2PWin_);
	DeleteGraph(imgResultLose_);
	DeleteGraph(imgResultBackWin_);
	//DeleteGraph(imgResultBackLose_);
	DeleteGraph(bgmResultLose_);

	SoundManager::GetInstance()->StopBGM();
	SoundManager::GetInstance()->DeleteSceneSound(LoadScene::RESULT);
}
