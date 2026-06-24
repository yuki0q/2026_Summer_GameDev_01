#include "../Manager/ResourceManager.h"
#include "../Manager/SceneManager.h"
#include "../Manager/TopDataManager.h"
#include "SceneBase.h"

SceneBase::SceneBase(void)
	:
	resMng_(ResourceManager::GetInstance()),
	sceMng_(SceneManager::GetInstance()),
	dataMng_(TopDataManager::GetInstance())
{
}

SceneBase::~SceneBase(void)
{
}

void SceneBase::Init(void)
{
}

void SceneBase::Update(void)
{
}

void SceneBase::Draw(void)
{
}
