#include "MapChipField.h"
#include <map>
#include <fstream>
#include <sstream>

using namespace KamataEngine;

namespace
{
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

void MapChipField::ResetMapChipData()
{
	// マップチップデータをリセット
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data)
	{
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath)
{
	// マップチップデータをリセット
	ResetMapChipData();

	// ファイルを開く
	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());
	
	// マップチップCSV
	std::stringstream mapChipCsv;

	// ファイルの内容を文字列ストリームにコピー
	mapChipCsv << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// CSVのマップチップデータを読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i)
	{
		std::string line;
		getline(mapChipCsv, line);

		// 1行目の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j)
		{
			std::string word;
			getline(line_stream, word, ',');

			if (mapChipTable.contains(word))
			{
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

// マップチップ種別の取得
MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex)
{
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex)
	{
		return MapChipType::kBlank;
	}

	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex)
	{
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

// マップチップ座標の取得
KamataEngine::Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex)		
 {
	return KamataEngine::Vector3(kBlockWidth * xIndex, kBlockHeight * (kNumBlockVirtical - 1 - yIndex), 0);
 }

 MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position)
 {
	 IndexSet indexSet = {};
	 indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	 indexSet.yIndex = kNumBlockVirtical - 1-static_cast < uint32_t >((position.y + kBlockHeight / 2)/kBlockHeight);

	 return indexSet;
 }

 //ブロックの範囲取得係数
 MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex)
 {
	 //指定ブロックの中心座標を取得する
	 Vector3 center = MapChipField::GetMapChipPositionByIndex(xIndex, yIndex);

	 MapChipField::Rect rect{};
	 rect.left = center.x - kBlockWidth / 2.0f;
	 rect.right = center.x + kBlockWidth / 2.0f;
	 rect.bottom = center.y + kBlockHeight / 2.0f;
	 rect.top = center.y - kBlockHeight / 2.0f;

	 return rect;
 }