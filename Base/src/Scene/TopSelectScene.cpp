#include <cmath>
#include <DxLib.h>
#include "../Manager/InputManager.h"
#include "../Manager/SceneManager.h"
#include "../Utility/AsoUtility.h"
#include "../Manager/camera.h"
#include "TopSelectScene.h"

TopSelectScene::TopSelectScene(void)
{
}

TopSelectScene::~TopSelectScene(void)
{
}

void TopSelectScene::Init(void)
{
}

void TopSelectScene::Update(void)
{
	// ÉVÅ[ÉìëJà⁄
	auto const& ins = InputManager::GetInstance();
	if (ins.IsTrgDown(KEY_INPUT_SPACE))
	{
		sceMng_.ChangeScene(SceneManager::SCENE_ID::GAME);
	}
}

void TopSelectScene::Draw(void)
{
}

void TopSelectScene::Release(void)
{
}
