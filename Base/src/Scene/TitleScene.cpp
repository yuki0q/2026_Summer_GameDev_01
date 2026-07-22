#include <DxLib.h>
#include "../Utility/AsoUtility.h"
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/Camera.h"
#include "../Manager/ResourceManager.h"
#include "../Manager/Resource.h"
#include "../Manager/SoundManager.h"
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
	texPlayerNo_ = resMng_.Load(ResourceManager::SRC::TEX_PLAYER_NOSELECT).handleId_;
	texPlayer1_ = resMng_.Load(ResourceManager::SRC::TEX_PLAYER_1).handleId_;
	texPlayer2_ = resMng_.Load(ResourceManager::SRC::TEX_PLAYER_2).handleId_;
	texSpace_ = resMng_.Load(ResourceManager::SRC::TEX_SPACE).handleId_;
	texEscape_ = resMng_.Load(ResourceManager::SRC::TEX_ESCAPE).handleId_;
	//titleBGM_ = LoadSoundMem("Data/Music/Title.mp3");

	select_ = DEFAULT_SELECT;
	count_ = 0;

	// プレイヤー
	topBlue_.SetModel(resMng_.Load(ResourceManager::SRC::BLUE_TOP).handleId_);
	topBlue_.scl = PLAYER_DEFAULT_SCALE;

	topBlue_.quaRot = Quaternion::Identity();
	topBlue_.quaRot = Quaternion::Mult(topBlue_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-90.0f), AsoUtility::AXIS_Y));

	topBlue_.quaRot = Quaternion::Mult(topBlue_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-15.0f), AsoUtility::AXIS_Z));

	topBlue_.quaRot = Quaternion::Mult(topBlue_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-20.0f), AsoUtility::AXIS_X));

	topBlue_.pos = BLUE_DEFAULT_POS;
	topBlue_.Update();

	topRed_.SetModel(resMng_.Load(ResourceManager::SRC::RED_TOP).handleId_);
	topRed_.scl = PLAYER_DEFAULT_SCALE;

	topRed_.quaRot = Quaternion::Identity();
	topRed_.quaRot = Quaternion::Mult(topRed_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-90.0f), AsoUtility::AXIS_Y));

	topRed_.quaRot = Quaternion::Mult(topRed_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(-15.0f), AsoUtility::AXIS_Z));

	topRed_.quaRot = Quaternion::Mult(topRed_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(20.0f), AsoUtility::AXIS_X));

	topRed_.pos = RED_DEFAULT_POS;
	topRed_.Update();

	// 定点カメラ
	sceMng_.GetCamera()->ChangeMode(Camera::MODE::FIXED_POINT);

	introWindow_ = false;
	isStickUpOld = false;
	isStickDownOld = false;

	// タイトル画面に必要なサウンドをロード
	SoundManager::GetInstance()->LoadSceneSound(LoadScene::TITLE);

	// タイトルBGMを再生（自動でループ再生されます）
	SoundManager::GetInstance()->PlayBGM(SoundID::BGM_TITLE);
	SoundManager::GetInstance()->SetBgmVolume(160);

}

void TitleScene::Update(void)
{
	//SpherePlanet_.Update();
	topBlue_.quaRot = Quaternion::Mult(topBlue_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(10.0f), AsoUtility::AXIS_Y));

	topRed_.quaRot = Quaternion::Mult(topRed_.quaRot,
		Quaternion::AngleAxis(AsoUtility::Deg2RadF(10.0f), AsoUtility::AXIS_Y));

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
			SoundManager::GetInstance()->PlaySE(SoundID::SE_CARSOL);
		}
		else if (select_ > 680) {
			select_ = 480;
		}

		if (ins.IsTrgDown(KEY_INPUT_UP) || 
			ins.IsTrgDown(KEY_INPUT_W) || (isStickUpNow && !isStickUpOld)) {
			select_ -= SELECT_MOVE;
			SoundManager::GetInstance()->PlaySE(SoundID::SE_CARSOL);
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
			SoundManager::GetInstance()->PlaySE(SoundID::SE_BUTTON);
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
			SoundManager::GetInstance()->PlaySE(SoundID::SE_CANCEL);
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
			SoundManager::GetInstance()->PlaySE(SoundID::SE_CARSOL);
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
			SoundManager::GetInstance()->PlaySE(SoundID::SE_CANCEL);
		}

		// ウィンドウが開いている状態で決定ボタンが押されたら、ここで初めてシーンを遷移させる
		if ((ins.IsTrgDown(KEY_INPUT_SPACE) ||
			ins.IsPadBtnTrgDown(InputManager::JOYPAD_NO::PAD1, InputManager::JOYPAD_BTN::DOWN)))
		{
			SoundManager::GetInstance()->PlaySE(SoundID::SE_BUTTON);
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

			// ゲームシーンへ遷移
			SoundManager::GetInstance()->StopBGM();
			SoundManager::GetInstance()->DeleteSceneSound(LoadScene::TITLE);
			sceMng_.ChangeScene(SceneManager::SCENE_ID::TOP_SELECT);
		}
	}

	topBlue_.Update();
	topRed_.Update();
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

	MV1DrawModel(topBlue_.modelId);
	MV1DrawModel(topRed_.modelId);

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

		// 人数選択
		DrawRotaGraph(640, 240, 0.6, 0.0, texPlayerNo_, true);
		
		// 1人プレイの文字と選択カーソル
		unsigned int color1P = (windowSelect_ == 0) ? GetColor(255, 255, 0) : GetColor(150, 150, 150);
		//DrawString(580, 320, "1 PLAYER", color1P);
		DrawRotaGraph(700, 320, 0.4, 0.0, texPlayer1_, true);
		if (windowSelect_ == 0) {
			//DrawString(550, 320, "?", color1P);
			DrawRotaGraph(660, 320, 0.5f, 0.0f, selectNow_, true);
		}

		// 2人プレイの文字と選択カーソル
		unsigned int color2P = (windowSelect_ == 1) ? GetColor(255, 255, 0) : GetColor(150, 150, 150);
		//DrawString(580, 380, "2 PLAYERS", color2P);
		DrawRotaGraph(700, 380, 0.4, 0.0, texPlayer2_, true);
		if (windowSelect_ == 1) {
			//DrawString(550, 380, "?", color2P);
			DrawRotaGraph(660, 380, 0.5f, 0.0f, selectNow_, true);
		}

		// 操作案内
		//texSpace_
		DrawRotaGraph(550, 480, 0.45, 0.0, texSpace_, true);
		DrawRotaGraph(720, 480, 0.45, 0.0, texEscape_, true);

		//DrawString(510, 460, "[SPACE]:Enter  [BACKSPACE]:Cancel", GetColor(200, 200, 200));
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
	DeleteGraph(texPlayerNo_);
	DeleteGraph(texPlayer1_);
	DeleteGraph(texPlayer2_);
	SoundManager::GetInstance()->StopBGM();
	SoundManager::GetInstance()->DeleteSceneSound(LoadScene::TITLE);
	topBlue_.Release();
	topRed_.Release();
}
