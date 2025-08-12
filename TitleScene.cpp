#include "TitleScene.h"
#include "CameraController.h"
#include "MapChipField.h"

using namespace KamataEngine;

// 初期化
void TitleScene::Initialize()
{
	//3Dモデルの生成
	modelTitle_ = Model::CreateFromOBJ("player");
	modelPlayer_ = Model::CreateFromOBJ("player");
	//カメラの初期化
	camera_.Initialize();
	//ワールド変換の初期化
	worldTransformTitle_.Initialize();
	worldTransformPlayer_.Initialize();
}

// 更新
void TitleScene::Update()
{
	// ビュープロジェクション行列の転送
	camera_.TransferMatrix();

	//タイトルシーンの終了条件
	if (Input::GetInstance()->PushKey(DIK_SPACE))
	{
		finished_ = true;
	}
}

// 描画
void TitleScene::Draw()
{
	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();
	//3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());
	//ここに3Dモデルインスタンスの描画処理を記述する
	modelTitle_->Draw(worldTransformTitle_, camera_);
	modelPlayer_->Draw(worldTransformPlayer_, camera_);
	//3Dモデル描画後処理
	Model::PostDraw();
}