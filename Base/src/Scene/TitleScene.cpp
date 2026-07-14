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
	button_(0),
	pushedButton_(0),
	gameEnd_(0),
	gameStart_(0),
	configImg_(0),
	instructions_(0),
	selectNow_(0),
	select_(0),
	count_(0),
	introWindow_(false),
	window_(false),
	windowSelect_(0),
	isStickUpOld(false),
	titleBGM_(0),
	isStickDownOld(false),
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
	//imgPushSpace_ = resMng_.Load(ResourceManager::SRC::TITLE_PUSH_SPACE).handleId_;
	button_ = resMng_.Load(ResourceManager::SRC::IMAGE_BUTTON).handleId_;
	pushedButton_ = resMng_.Load(ResourceManager::SRC::IMAGE_PUSHED_BUTTON).handleId_;
	gameStart_ = resMng_.Load(ResourceManager::SRC::GAME_START).handleId_;
	configImg_ = resMng_.Load(ResourceManager::SRC::CONFIG_IMG).handleId_;
	instructions_ = resMng_.Load(ResourceManager::SRC::IMAGE_INSTRUCTUION).handleId_;
	gameEnd_ = resMng_.Load(ResourceManager::SRC::GAME_END).handleId_;
	selectNow_ = resMng_.Load(ResourceManager::SRC::SELECT_NOW).handleId_;
	titleBGM_ = LoadSoundMem("Data/Music/Title.mp3");
	PlaySoundMem(titleBGM_, DX_PLAYTYPE_LOOP, true);
	ChangeVolumeSoundMem(150, titleBGM_);

	select_ = DEFAULT_SELECT;
	count_ = 0;

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

	introWindow_ = false;
	isStickUpOld = false;
	isStickDownOld = false;
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
	auto& ins = InputManager::GetInstance();

	// 接続されているゲームパッド１の情報を取得
	InputManager::JOYPAD_IN_STATE padState =
		ins.GetJPadInputState(InputManager::JOYPAD_NO::PAD1);

	VECTOR inputDir = ins.GetDirectionXZAKey(padState.AKeyLX, padState.AKeyLY);

	// スティックが上/下に一定以上倒されているかの判定フラグ
	// (inputDir.z > 0.5f が「上」、inputDir.z < -0.5f が「下」になります)
	bool isStickUpNow = (inputDir.z > 0.5f);
	bool isStickDownNow = (inputDir.z < -0.5f);

	if (!window_)
	{

		if (ins.IsTrgDown(KEY_INPUT_DOWN) || 
			ins.IsTrgDown(KEY_INPUT_S) || (isStickDownNow && !isStickDownOld)) {
			select_ += SELECT_MOVE;
		}
		else if (select_ > 680) {
			select_ = 480;
		}

		if (ins.IsTrgDown(KEY_INPUT_UP) || 
			ins.IsTrgDown(KEY_INPUT_W) || (isStickUpNow && !isStickUpOld)) {
			select_ -= SELECT_MOVE;
		}
		else if (select_ < 480) {
			select_ = 680;
		}

		isStickUpOld = isStickUpNow;
		isStickDownOld = isStickDownNow;

		count_ = (select_ - DEFAULT_SELECT) / SELECT_MOVE;

		if ((ins.IsTrgDown(KEY_INPUT_SPACE) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
			&& !introWindow_)
		{
			switch (count_) {
			case 0:
				window_ = true;
				windowSelect_ = 0;
				//
				break;
			case 1:
				//sceMng_.ChangeScene(SceneManager::SCENE_ID::TITLE);
				introWindow_ = true;
				break;
			case 2:
				Application::GetInstance().Shutdown();
				break;
			}
		}
		else if ((ins.IsTrgDown(KEY_INPUT_SPACE) || ins.IsTrgDown(KEY_INPUT_ESCAPE) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN))
			&& introWindow_)
		{
			introWindow_ = false;
		}
	}else
	{
		// ウィンドウ内での上下選択（1人プレイか2人プレイか）
		if (ins.IsTrgDown(KEY_INPUT_UP) || ins.IsTrgDown(KEY_INPUT_DOWN)
		|| ins.IsTrgDown(KEY_INPUT_W) || ins.IsTrgDown(KEY_INPUT_S)
			|| (isStickUpNow && !isStickUpOld)
			|| (isStickDownNow && !isStickDownOld))
		{
			// 0 と 1 を反転させる
			windowSelect_ += 1;
		}
		else if (windowSelect_ > 1) {
			windowSelect_ = 0;
		}

		isStickUpOld = isStickUpNow;
		isStickDownOld = isStickDownNow;

		// キャンセルボタン（例: BACKSPACEやESCキーなど）でウィンドウを閉じる
		if (ins.IsTrgDown(KEY_INPUT_ESCAPE) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::START))
		{
			window_ = false;
		}

		// ウィンドウが開いている状態で決定ボタンが押されたら、ここで初めてシーンを遷移させる
		if ((ins.IsTrgDown(KEY_INPUT_SPACE) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)))
		{
			// プレイ人数をSceneManagerなどに保存する
			if (windowSelect_ == 0)
			{
				// 1人プレイ用の設定を設定
				sceMng_.SetPlayerNo(1);
			}
			else
			{
				// 2人プレイ用の設定を設定
				sceMng_.SetPlayerNo(2);
			}

			StopSoundMem(titleBGM_);

			// ゲームシーンへ遷移
			sceMng_.ChangeScene(SceneManager::SCENE_ID::TOP_SELECT);
		}
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
	DrawRotaGraph(Application::SCREEN_SIZE_X/2,Application::SCREEN_SIZE_Y / 2, 0.9f, 0.0f, imgTitle_, true);
	DrawRotaGraph(IMG_PUSH_SPACE_POS_X, IMG_PUSH_SPACE_POS_Y, 1.0f, 0.0f, imgPushSpace_, true);

	MV1DrawModel(top_.modelId);

	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 480, 0.5f, 0.0f, button_, true);
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 580, 0.5f, 0.0f, button_, true);
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 680, 0.5f, 0.0f, button_, true);

	// 文字の描画
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 480, 0.5f, 0.0f, gameStart_, true);
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 580, 0.5f, 0.0f, instructions_, true);
	DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 680, 0.5f, 0.0f, gameEnd_, true);

	// 選択中のボタン用枠
	if ((select_ >= 480) && (select_ <= 680)) {
		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, select_, 0.6f, 0.0f, selectNow_, true);
	}

	if (introWindow_) {
		// 操作説明用の画像を描画
		DrawRotaGraph(Application::SCREEN_SIZE_X / 2, 360, 0.7f, 0.0f, configImg_, true);
	}
	if (window_)
	{
		// 画面中央に黒透過のウィンドウ（四角形）を描画
		// 画面サイズが 1280 x 720 だと仮定した場合の中央付近
		int winX1 = 440, winY1 = 200;
		int winX2 = 840, winY2 = 520;

		// ウィンドウの背景（少し透ける黒）
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(winX1, winY1, winX2, winY2, GetColor(0, 0, 0), true);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// ウィンドウの枠線（白）
		DrawBox(winX1, winY1, winX2, winY2, GetColor(255, 255, 255), false);

		// タイトル文字
		DrawString(540, 240, "PLAY MODE SELECT", GetColor(255, 255, 255));

		// 1人プレイの文字と選択カーソル
		unsigned int color1P = (windowSelect_ == 0) ? GetColor(255, 255, 0) : GetColor(150, 150, 150);
		DrawString(580, 320, "1 PLAYER", color1P);
		if (windowSelect_ == 0) {
			DrawString(550, 320, "?", color1P);
		}

		// 2人プレイの文字と選択カーソル
		unsigned int color2P = (windowSelect_ == 1) ? GetColor(255, 255, 0) : GetColor(150, 150, 150);
		DrawString(580, 380, "2 PLAYERS", color2P);
		if (windowSelect_ == 1) {
			DrawString(550, 380, "?", color2P);
		}

		// 操作案内
		DrawString(510, 460, "[SPACE]:Enter  [BACKSPACE]:Cancel", GetColor(200, 200, 200));
	}
}

void TitleScene::Release(void)
{
	DeleteGraph(imgTitle_);
	DeleteGraph(imgPushSpace_);
	DeleteGraph(button_);
	DeleteGraph(pushedButton_);
	DeleteGraph(gameEnd_);
	DeleteGraph(gameStart_);
	DeleteGraph(configImg_);
	DeleteGraph(instructions_);
	DeleteGraph(selectNow_);
	DeleteSoundMem(titleBGM_);
	//skyDome_->Release();

	delete animController_;
	//delete skyDome_;

	/*bigPlanet_.Release();
	SpherePlanet_.Release();*/
	top_.Release();
}
