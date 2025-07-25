#pragma once
#include "KamataEngine.h"

enum class MapChipType
{
	kBlank,//空白
	kBlock,//ブロック
};

///<summary>
///マップチップフィールド
///<summary>
class MapChipField
{
public:

	struct MapChipDate {
		std::vector<std::vector<MapChipType>> data;
	};

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	// 範囲矩形
	struct Rect {
		float left;   // 左端
		float right;  // 右端
		float bottom; // 下端
		float top;    // 上端
	};

	//リセット
	void ResetMapChipData();

	//読み込み
	void LoadMapChipCsv(const std::string& filePath);

	//マップチップ種別の取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	//マップチップ座標の取得
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }
	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

	IndexSet GetMapChipIndexSetByPosition(const KamataEngine::Vector3& position);

	//ブロックの範囲取得関数
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:

	// マップチップデータ構造体
	MapChipDate mapChipData_;

	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
};