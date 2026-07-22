#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Manager/SoundManager.h"
#include "../Object/Actor/Charactor/TopBase.h"
#include "TopSelectScene.h"

TopSelectScene::TopSelectScene(void)
	:
	imgTopSelectBack_(0),
	imgTopSelect_(0),
	imgTopSelectText_(0),
	imgSkillSelect_(0),
	imgSkillSelectText_(0),
	selectIndex1P_(0),
	selectIndex2P_(0),
	imgTopIntro_(0),
	modelHandles_(0),
	isStickLeftOld_(false),
	isStickRightOld_(false),
	redySelect1P_(false),
	redySelect2P_(false),
	fontHandel_(0),
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
	fontHandel_ = resMng_.Load(ResourceManager::SRC::FONT_1).handleId_;	// バランス

	imgTopSelectText_ = resMng_.Load(ResourceManager::SRC::SELECT_NOW).handleId_;

	selectIndex1P_ = selectIndex2P_ = 0;

	top_.SetModel(modelHandles_[selectIndex1P_]);
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

	// タイトル画面に必要なサウンドをロード
	SoundManager::GetInstance()->LoadSceneSound(LoadScene::SELECT);

	// タイトルBGMを再生（自動でループ再生されます）
	SoundManager::GetInstance()->PlayBGM(SoundID::BGM_SELECT);
	SoundManager::GetInstance()->SetBgmVolume(160);
}

void TopSelectScene::Update(void)
{
auto& ins = InputManager::GetInstance();

// --- パッドが1台だけ接続されている状態を想定 ---
// PAD1の情報を取得（これを2P用、あるいは1Pの補助用として使います）
InputManager::JOYPAD_IN_STATE padState1 = ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);
VECTOR inputDir1 = ins.GetDirectionXZAKey(padState1.AKeyLX, padState1.AKeyLY);

bool isPad1Left = (inputDir1.x > 0.5f);
bool isPad1Right = (inputDir1.x < -0.5f);
bool pad1Down = ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN);
bool pad1Start = ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START);

// ==========================================
// 1Pの入力判定（キーボードのみ、または必要に応じてPAD1も許可）
// ==========================================
bool isLeftInput1P = ins.IsTrgDown(KEY_INPUT_A);  // Aキー
bool isRightInput1P = ins.IsTrgDown(KEY_INPUT_D);  // Dキー
bool isDecide1P = ins.IsTrgDown(KEY_INPUT_SPACE); // SPACEキー

// もし1Pプレイ時（シングルプレイ）はパッドでも1Pを動かしたい場合は以下を有効にします
if (sceMng_.GetPlayerNo() == 1) {
	isLeftInput1P = isLeftInput1P || (isPad1Left && !isStickLeftOld_);
	isRightInput1P = isRightInput1P || (isPad1Right && !isStickRightOld_);
	isDecide1P = isDecide1P || pad1Down;
}

// 1Pの選択・決定処理
if (!redySelect1P_)
{
	if (isLeftInput1P) {
		selectIndex1P_ = (selectIndex1P_ + 3) % 4; // 左ループ
		SoundManager::GetInstance()->PlaySE(SoundID::SE_CARSOL);
	}
	if (isRightInput1P) {
		selectIndex1P_ = (selectIndex1P_ + 1) % 4; // 右ループ
		SoundManager::GetInstance()->PlaySE(SoundID::SE_CARSOL);
	}

	// 1P決定
	if (isDecide1P) {
		sceMng_.SetPlayerTopType(selectIndex1P_);
		redySelect1P_ = true;
		SoundManager::GetInstance()->PlaySE(SoundID::SE_BUTTON);
	}
}

// ==========================================
// 2Pの入力判定（2人プレイ時は、接続された唯一のパッド「PAD1」を2P専用にする）
// ==========================================
if (sceMng_.GetPlayerNo() == 2)
{
	// 2PはPAD1の入力を見る
	bool isLeftInput2P = (isPad1Left && !isStickLeftOld_);
	bool isRightInput2P = (isPad1Right && !isStickRightOld_);
	bool isDecide2P = pad1Down;

	if (!redySelect2P_)
	{
		if (isLeftInput2P) {
			selectIndex2P_ = (selectIndex2P_ + 3) % 4; // 左ループ
			SoundManager::GetInstance()->PlaySE(SoundID::SE_CARSOL);
		}
		if (isRightInput2P) {
			selectIndex2P_ = (selectIndex2P_ + 1) % 4; // 右ループ
			SoundManager::GetInstance()->PlaySE(SoundID::SE_CARSOL);
		}

		// 2P決定
		if (isDecide2P) {
			sceMng_.SetPlayerTopType2P(selectIndex2P_);
			SoundManager::GetInstance()->PlaySE(SoundID::SE_BUTTON);
			redySelect2P_ = true;
		}
	}
}

// スティックの押しっぱなし防止用フラグ更新（PAD1の入力を監視）
isStickLeftOld_ = isPad1Left;
isStickRightOld_ = isPad1Right;

// ==========================================
// モデルの見た目更新
// ==========================================
if (!redySelect1P_) {
	top_.SetModel(modelHandles_[selectIndex1P_]);
}
else if (!redySelect2P_ && sceMng_.GetPlayerNo() == 2) {
	top_.SetModel(modelHandles_[selectIndex2P_]);
}

if (modelHandles_[selectIndex1P_] != -1) {
	top_.quaRot = Quaternion::Mult(top_.quaRot, Quaternion::AngleAxis(AsoUtility::Deg2RadF(0.5f), AsoUtility::AXIS_Y));
}

// ==========================================
// シーン遷移処理
// ==========================================
if (sceMng_.GetPlayerNo() == 1)
{
	if (redySelect1P_) {
		SoundManager::GetInstance()->StopBGM();
		SoundManager::GetInstance()->DeleteSceneSound(LoadScene::SELECT);
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);

	}
}
else if (sceMng_.GetPlayerNo() == 2)
{
	// 両者が揃ったら、キーボードのSPACE、またはパッドの決定ボタンで遷移
	if (redySelect1P_ && redySelect2P_) {
		if (ins.IsTrgDown(KEY_INPUT_SPACE) || pad1Down) {
			SoundManager::GetInstance()->PlaySE(SoundID::SE_BUTTON);

			SoundManager::GetInstance()->StopBGM();
			SoundManager::GetInstance()->DeleteSceneSound(LoadScene::SELECT);
			sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
		}
	}
}

// ==========================================
// キャンセル・戻る処理
// ==========================================
if (ins.IsTrgDown(KEY_INPUT_ESCAPE) || pad1Start)
{
	if (!redySelect1P_ && !redySelect2P_) {
		SoundManager::GetInstance()->PlaySE(SoundID::SE_CANCEL);

		SoundManager::GetInstance()->StopBGM();
		SoundManager::GetInstance()->DeleteSceneSound(LoadScene::SELECT);
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
	}
	else {
		SoundManager::GetInstance()->PlaySE(SoundID::SE_CANCEL);
		redySelect1P_ = false;
		redySelect2P_ = false;
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
	DrawStringToHandle(100, 100, "SELECT YOUR SPINNING TOP", 0xffffff, fontHandel_);
	DrawStringToHandle(100, 600, "Press LEFT / RIGHT to Select, SPACE to Decide", 0xffffff, fontHandel_);
	DrawRotaGraph(900, 500, 1.5f, 0.0f, imgTopSelectText_, true);

	if (!redySelect1P_) {
		DrawStringToHandle(100, 150, typeNames[selectIndex1P_], 0xffff00, fontHandel_);
		DrawRotaGraph(900, 500, 0.7f, 0.0f, imgTopIntro_[selectIndex1P_], true);
	}
	else if (!redySelect2P_) {

		DrawStringToHandle(100, 150, typeNames[selectIndex2P_], 0xffff00, fontHandel_);
		DrawRotaGraph(900, 500, 0.7f, 0.0f, imgTopIntro_[selectIndex2P_], true);
	}

	// 選択中の3Dモデルを原点に描画
	if (modelHandles_[selectIndex1P_] != -1)
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

	SoundManager::GetInstance()->StopBGM();
	SoundManager::GetInstance()->DeleteSceneSound(LoadScene::SELECT);

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
