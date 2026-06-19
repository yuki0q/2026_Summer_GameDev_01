#pragma once
#include "SceneBase.h"
#include "../Application.h"
#include "../Object/Common/Transform.h"
#include "../Object/Common/AnimationController.h"
class SkyDome;

class TitleScene : public SceneBase
{

public:

	enum class ANIM_TYPE
	{
		RUN,
		MAX,
	};

	// コンストラクタ
	TitleScene(void);

	// デストラクタ
	~TitleScene(void) override;

	// 初期化
	void Init(void) override;

	// 更新
	void Update(void) override;

	// 描画
	void Draw(void) override;

	// 解放
	void Release(void) override;

private:

	static constexpr int IMG_TITLE_POS_Y = 260;
	static constexpr int IMG_TITLE_POS_X = 640;

	static constexpr int IMG_PUSH_SPACE_POS_Y = 410;
	static constexpr int IMG_PUSH_SPACE_POS_X = 640;

	static constexpr VECTOR SPHERE_PLANET_DEFAULT_POS = { -250.0f, -100.0f, -100.0f };

	static constexpr VECTOR SPHERE_PLANET_DEFAULT_SCALE = { 0.7f,0.7f,0.7f };

	static constexpr VECTOR SPHERE_PLANET_ROT = { 90.0f + DX_PI_F / 180.0f,0.0f,0.0f };

	static constexpr VECTOR PLAYER_DEFAULT_POS = { 160.0f, 130.0f, -105.0f };

	static constexpr VECTOR PLAYER_DEFAULT_SCALE = { 0.4f,0.4f,0.4f };

	// 惑星
	Transform bigPlanet_;
	Transform SpherePlanet_;

	Transform top_;

	AnimationController* animController_;
	SkyDome* skyDome_;

	// スカイドーム用の空Transform
	Transform empty_;

	int imgTitle_;
	int imgPushSpace_;

	// デフォルトの選択位置
	static constexpr int DEFAULT_SELECT = 480;

	// ボタンの移動量
	static constexpr int SELECT_MOVE = 100;

	// 選択枠の移動
	int select_;
	int count_;

	bool isStickUpOld;
	bool isStickDownOld;

	// 操作説明ウィンドウのフラグ
	bool window_;

	// 描画用
	int button_;
	int pushedButton_;
	int gameEnd_;
	int gameStart_;
	int configImg_;
	int instructions_;
	int selectNow_;

};
