#include "TitleScene.h"
#include "MyMath.h"
#include "CameraController.h"
#include "MapChipField.h"

using namespace KamataEngine;

// 初期化
void TitleScene::Initialize()
{
	//3Dモデルの生成
	modelTitle_ = Model::CreateFromOBJ("titleFont");
	modelPlayer_ = Model::CreateFromOBJ("player");
	//カメラの初期化
	camera_.Initialize();
	//ワールド変換の初期化
	worldTransformTitle_.Initialize();
	worldTransformPlayer_.Initialize();
	
	//フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

// 更新
void TitleScene::Update()
{
	switch (phase_) {
	case Phase::kMain:
		// タイトルシーンの終了条件
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			// フェードアウト開始
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}

		break;
	case Phase::kFadeIn:
		//フェード
		fade_->Update();
		if (fade_->IsFinished())
		{
			phase_ = Phase::kMain;
		}

		break;
	case Phase::kFadeOut:
		// フェード
		fade_->Update();
		if (fade_->IsFinished())
		{
			finished_ = true;
		}
	}

	////アフィン変換行列の作成
	//worldTransform.matWorld = MakeAffineMatrix(world)
	//// 行列を定数バッファに転送
	//worldTransform_.TransferMatrix();

	////回転
	//rotate += 0.1f;
	//worldTransformPlayer_.rotation_.y = 3.14f + sin(rotate)

	//	//アフィン変換行列の作成
	//	worldTransform.matWorld = MakeAffineMatrix(world)
	
	//行列を定数バッファに転送
	camera_.TransferMatrix();

	//フェード
	fade_->Update();
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

	//フェード
	fade_->Draw();
}

// デストラクタ
TitleScene::~TitleScene() {
	// モデル
	delete modelTitle_;
	delete modelPlayer_;

	// フェード
	delete fade_;
}