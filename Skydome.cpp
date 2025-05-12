#include "Skydome.h"
#include <cassert>

using namespace KamataEngine;

void Skydome::Initialize(Model* model, uint32_t textureHandle,Camera* camera) {
	///インゲームの初期化処理///

	// 引数として受け取ったデータをメンバ変数に記録する
	camera_ = camera;
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

void Skydome::Update()
{
	///インゲームの更新処理///
}

void Skydome::Draw()
{
	///描画処理///
	
	//3Dモデル描画
	model_->Draw(worldTransform_, *camera_);
}