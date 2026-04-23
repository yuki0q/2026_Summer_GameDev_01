#pragma once
#include "SceneBase.h"

class StageSelectScene : public SceneBase
{
public:
	// コンストラクタ
	StageSelectScene(void);
	// デストラクタ
	~StageSelectScene(void)override;

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:

	// 画像
	int imgStage_;
	int imgTitleText_;
	int imgSelectBack_;
};

