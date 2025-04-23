#include "GameScene.h"
// #include "ImGuiManager.h"

using namespace KamataEngine;

void GameScene::Initialize() {
	///インゲームの初期化処理///

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");

	//サウンドデータの読み込み
	soundDateHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav");

	//3Dモデル
	model_ = Model::Create();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//カメラの初期化
	camera_.Initialize();

	//音声再生
	//Audio::GetInstance()->PlayWave(soundDateHandle_);

	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDateHandle_, true);
}

void GameScene::Update() {
	///インゲームの更新処理///

	//スペースキーを押した瞬間
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 音声停止
		Audio::GetInstance()->StopWave(voiceHandle_);
	}

	/*/デバッグテキストの表示
	ImGui::Begin("Debug1");

	//float3入力ボックス
	//ImGui::InputFloat3("InputFloat3", inputFloat3);

	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);

	ImGui::End();*/
}

void GameScene::Draw() {
	///描画処理///

	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	///モデルインスタンスの描画処理///

	//3Dモデル描画
	model_->Draw(worldTransform_, camera_, textureHandle_);

	///＝＝＝＝＝＝＝＝＝＝＝＝＝＝///

	//3Dモデル描画後処理
	Model::PostDraw();
}

//デストラクタ
GameScene::~GameScene() { delete model_; }