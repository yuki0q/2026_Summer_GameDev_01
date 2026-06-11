#pragma once
#include "SceneBase.h"
#include "../Object/Common/Transform.h"

class TopSelectScene : public SceneBase
{
public:
	// コンストラクタ
	TopSelectScene(void);
	// デストラクタ
	~TopSelectScene(void) override;

	void Init(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Release(void) override;

private:

	static constexpr int IMG_TITLE_POS_Y = 260;
	static constexpr int IMG_TITLE_POS_X = 640;

	static constexpr int IMG_PUSH_SPACE_POS_Y = 410;
	static constexpr int IMG_PUSH_SPACE_POS_X = 640;

	static constexpr VECTOR SPHERE_PLANET_DEFAULT_POS = { -250.0f, -100.0f, -100.0f };

	static constexpr VECTOR SPHERE_PLANET_DEFAULT_SCALE = { 0.7f,0.7f,0.7f };

	static constexpr VECTOR SPHERE_PLANET_ROT = { 90.0f + DX_PI_F / 180.0f,0.0f,0.0f };

	static constexpr VECTOR PLAYER_DEFAULT_POS = { 0.0f, 0.0f, 0.0f };

	static constexpr VECTOR PLAYER_DEFAULT_SCALE = { 1.0f,1.0f,1.0f };

	Transform top_;

	// 画像
	int imgTopSelectBack_;
	int imgTopSelect_;
	int imgTopSelectText_;
	int imgSkillSelect_;
	int imgSkillSelectText_;

	// モデル
	int modelHandles_[4] = { -1, -1, -1, -1 };

	// 選択管理 0:ATTACK, 1:DEFENSE, 2:STAMINA, 3:BALANCE
	int selectIndex_; 

	// キーの押しっぱなし防止用
	bool isStickLeftOld_;
	bool isStickRightOld_;
};

