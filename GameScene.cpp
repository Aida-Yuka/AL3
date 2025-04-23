#include "GameScene.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	// インゲームの初期化処理///

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// 3Dモデルの生成
	model_ = Model::Create();

	// 自キャラの生成
	player_ = new Player();

	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &camera_);
}

void GameScene::Update() {
	/// インゲームの更新処理///

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラの初期化
	camera_.Initialize();

	// 自キャラの更新
	player_->Update();
}

void GameScene::Draw() {
	/// 描画処理///

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	/// モデルインスタンスの描画処理///

	// 自キャラの描画
	player_->Draw();

	/// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝///

	// 3Dモデル描画後処理
	Model::PostDraw();
}

// デストラクタ
GameScene::~GameScene() {
	delete model_;
	delete player_;
}