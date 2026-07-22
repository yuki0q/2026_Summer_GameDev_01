#pragma once
#include "SceneBase.h"


class ResultScene : public SceneBase
{
public:
	// コンストラクタ
	ResultScene(void);
	// デストラクタ
	~ResultScene(void)override;

	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

private:

	bool isWin_ = false; // 描画分岐用のローカル変数

	// 画像
	int imgResultWin_;
	int imgResultBackWin_;
	int imgResult1PWin_;
	int imgResult2PWin_;

	int imgResultLose_;
	int imgResultBackLose_;
	// BGM
	int bgmResultLose_;

};