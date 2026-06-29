#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Object/Actor/Charactor/TopBase.h"
#include "TopSelectScene.h"

TopSelectScene::TopSelectScene(void)
	:
	imgTopSelectBack_(0),
	imgTopSelect_(0),
	imgTopSelectText_(0),
	imgSkillSelect_(0),
	imgSkillSelectText_(0),
	selectIndex_(0),
	imgTopIntro_(0),
	modelHandles_(0),
	isStickLeftOld_(false),
	isStickRightOld_(false),
	SceneBase()
{
}

TopSelectScene::~TopSelectScene(void)
{
}

void TopSelectScene::Init(void)
{
	modelHandles_[0] = resMng_.Load(ResourceManager::SRC::BLUE_TOP).handleId_;	// 青
	modelHandles_[1] = resMng_.Load(ResourceManager::SRC::GREEN_TOP).handleId_;	// 緑
	modelHandles_[2] = resMng_.Load(ResourceManager::SRC::YELLOW_TOP).handleId_;// 黄
	modelHandles_[3] = resMng_.Load(ResourceManager::SRC::RED_TOP).handleId_;	// 赤

	imgTopIntro_[0] = resMng_.Load(ResourceManager::SRC::IMAGE_INTRO_AT).handleId_;	// 攻撃
	imgTopIntro_[1] = resMng_.Load(ResourceManager::SRC::IMAGE_INTRO_DF).handleId_;	// 防御
	imgTopIntro_[2] = resMng_.Load(ResourceManager::SRC::IMAGE_INTRO_ST).handleId_;	// スタミナ
	imgTopIntro_[3] = resMng_.Load(ResourceManager::SRC::IMAGE_INTRO_BL).handleId_;	// バランス

	imgTopSelectText_ = resMng_.Load(ResourceManager::SRC::SELECT_NOW).handleId_;

	selectIndex_ = 0;

	top_.SetModel(modelHandles_[selectIndex_]);
	top_.scl = PLAYER_DEFAULT_SCALE;

	top_.quaRot = Quaternion::Identity();
	top_.quaRot = Quaternion::Mult(top_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-90.0f), AsoUtility::AXIS_Y));

	top_.quaRotLocal = Quaternion::Identity();
	top_.quaRotLocal = Quaternion::Mult(top_.quaRotLocal,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(180.0f), AsoUtility::AXIS_Y));

	top_.pos = PLAYER_DEFAULT_POS;
	top_.Update();

	// 定点カメラ
	sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	// カメラの初期設定（選択中のコマが綺麗に見える位置に配置）
	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 150.0f, -200.0f), top_.pos);
}

void TopSelectScene::Update(void)
{
	auto& ins = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	VECTOR inputDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	// スティックが右/左に一定以上倒されているかの判定フラグ
	bool isStickLeftNow = (inputDir.x > 0.5f);
	bool isStickRightNow = (inputDir.x < -0.5f);

	if (sceMng_.GetPlayerNo() == 1) {

		if (ins.IsTrgDown(KEY_INPUT_A) || (isStickLeftNow && !isStickLeftOld_)) {
			selectIndex_ = (selectIndex_ + 3) % 4; // 左へループ
		}

		if (ins.IsTrgDown(KEY_INPUT_D) || (isStickRightNow && !isStickRightOld_)) {
			selectIndex_ = (selectIndex_ + 1) % 4; // 右へループ
		}

		isStickLeftOld_ = isStickLeftNow;
		isStickRightOld_ = isStickRightNow;

		if (modelHandles_[selectIndex_] != -1)
		{
			top_.quaRot = Quaternion::Mult(top_.quaRot,
				Quaternion::AngleAxis(AsoUtility::Deg2RadF(0.5f), AsoUtility::AXIS_Y));
		}
		top_.SetModel(modelHandles_[selectIndex_]);
		// シーン遷移
		if (ins.IsTrgDown(KEY_INPUT_SPACE) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::DOWN))
		{
			sceMng_.SetPlayerTopType(selectIndex_);
			sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
		}

		if (ins.IsTrgDown(KEY_INPUT_ESCAPE) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
		{
			sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
		}
	}
	else if(sceMng_.GetPlayerNo() == 2) {

		if (ins.IsTrgDown(KEY_INPUT_A) || (isStickLeftNow && !isStickLeftOld_)) {
			selectIndex_ = (selectIndex_ + 3) % 4; // 左へループ
		}

		if (ins.IsTrgDown(KEY_INPUT_D) || (isStickRightNow && !isStickRightOld_)) {
			selectIndex_ = (selectIndex_ + 1) % 4; // 右へループ
		}

		isStickLeftOld_ = isStickLeftNow;
		isStickRightOld_ = isStickRightNow;

		if (modelHandles_[selectIndex_] != -1)
		{
			top_.quaRot = Quaternion::Mult(top_.quaRot,
				Quaternion::AngleAxis(AsoUtility::Deg2RadF(0.5f), AsoUtility::AXIS_Y));
		}
		top_.SetModel(modelHandles_[selectIndex_]);
		// シーン遷移
		if (ins.IsTrgDown(KEY_INPUT_SPACE) || ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1,
			InputManager::JOYPAD_BTN::DOWN))
		{
			sceMng_.SetPlayerTopType(selectIndex_);
			sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
		}

		if (ins.IsTrgDown(KEY_INPUT_ESCAPE) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
		{
			sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
		}
	}

	top_.Update();
}

void TopSelectScene::Draw(void)
{
	// 背景の描画など
	DrawBox(0, 0, 1280, 720, 0x222222, true);

	// テキスト表示（現在の選択を表示）
	const char* typeNames[] = { "ATTACK (BLUE)", "DEFENSE (GREEN)", "STAMINA (YELLOW)", "BALANCE (RED)" };
	DrawString(100, 100, "SELECT YOUR SPINNING TOP", 0xffffff);
	DrawString(100, 150, typeNames[selectIndex_], 0xffff00);
	DrawString(100, 600, "Press LEFT / RIGHT to Select, SPACE to Decide", 0xffffff);
	DrawRotaGraph(900, 500, 1.5f, 0.0f, imgTopSelectText_, true);
	DrawRotaGraph(900, 500, 0.7f, 0.0f, imgTopIntro_[selectIndex_], true);

	// 選択中の3Dモデルを原点に描画
	if (modelHandles_[selectIndex_] != -1)
	{
		top_.pos = PLAYER_DEFAULT_POS;
		MV1DrawModel(top_.modelId);
	}
}

void TopSelectScene::Release(void)
{
	DeleteGraph(imgTopSelectBack_);
	DeleteGraph(imgTopSelect_);
	DeleteGraph(imgTopSelectText_);
	DeleteGraph(imgSkillSelect_);
	DeleteGraph(imgSkillSelectText_);

	// ロードしたモデルの解放
	for (int i = 0; i < 4; ++i)
	{
		if (modelHandles_[i] != -1)
		{
			MV1DeleteModel(modelHandles_[i]);
			modelHandles_[i] = -1;
		}

		if (imgTopIntro_[i] != -1)
		{
			DeleteGraph(imgTopIntro_[i]);
			imgTopIntro_[i] = -1;
		}
	}
}
