#pragma once

#include <DxLib.h>
#include <map>
#include <string>
#include "../Object/Actor/Charactor/TopBase.h"

class TopDataManager
{

public:

    // 明示的にインステンスを生成する
    static void CreateInstance(void);

    // 静的インスタンスの取得
    static TopDataManager& GetInstance(void);

    TopDataManager() = default;
    ~TopDataManager() = default;

    // 初期化
    void Init(void);

    // 初期化時にCSVを読み込む
    void LoadCsvData(const std::string& filePath);

    void Destroy(void);

    // 特定のタイプやIDに応じたデータを取得する
    // ここでは例として TOP_TYPE をキーにしていますが、CSVにIDがあるなら int や string でもOK
    const TopBase::TopData* GetTopData(TopBase::TOP_TYPE type) const;

private:

    // 静的インスタンス
    static TopDataManager* instance_;
    std::map<TopBase::TOP_TYPE, TopBase::TopData> dataMap_;
};
