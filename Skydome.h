#pragma once
#include "KamataEngine.h"

class Skydome
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modle">モデル</param>
	/// <param name="camera">カメラ</param>
	
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera);

	void Update();

	void Draw();

private:
	//ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	//モデル
	KamataEngine::Model* model_ = nullptr;

	//カメラ
	KamataEngine::Camera* camera_ = nullptr;
};
