#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/camera.h"
#include "StageSelectScene.h"

StageSelectScene::StageSelectScene(void)
{
}

StageSelectScene::~StageSelectScene(void)
{
}

void StageSelectScene::Init(void)
{
}

void StageSelectScene::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::TOP_SELECT);
	}
}

void StageSelectScene::Draw(void)
{
}

void StageSelectScene::Release(void)
{
}
