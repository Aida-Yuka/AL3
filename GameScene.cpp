#include "GameScene.h"
#include "MyMath.h"
#include "Skydome.h"
#include "CameraController.h"

using namespace KamataEngine;


void GameScene::GenerateBlocks() {
	// ＝＝＝ブロック配置の初期化＝＝＝//
	// 要素数
	uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();     // 縦
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal(); // 横

	/// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVirtical);

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			/*/模様の処理
			if ((i + j) % 2 == 0)
			{
			    continue;
			}*/

			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::Initialize()
{
	///インゲームの初期化処理///

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("player");
	modelBlock_ = Model::CreateFromOBJ("block");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	//追従カメラの生成
	camera_ = new CameraController();//☆☆☆

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1600, 800);

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	GenerateBlocks();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	// 自キャラの生成
	player_ = new Player();

	// 自キャラの初期化
	player_->Initialize(model_, &camera_, playerPosition);

	player_ = SetMapChipField_(mapChipField_);
}

void GameScene::Update()
{
	/// インゲームの更新処理///

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// カメラの初期化
	camera_.Initialize();

	// 自キャラの更新
	player_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0))
	{
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	// カメラの処理
	if (isDebugCameraActive_) {
		debugCamera_->Update();

		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;

		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();

	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

	//追従カメラ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝

}

void GameScene::Draw()
{
	/// 描画処理///

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	/// モデルインスタンスの描画処理///

	// 自キャラの描画
	player_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
		{
			if (!worldTransformBlock)
				continue;

			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	//天球の描画
	modelSkydome_->Draw(worldTransform_,camera_);

	/// ＝＝＝＝＝＝＝＝＝＝＝＝＝＝///

	// 3Dモデル描画後処理
	Model::PostDraw();
}

// デストラクタ
GameScene::~GameScene()
{
	delete model_;
	delete player_;
	delete modelBlock_;
	delete debugCamera_;
	delete modelSkydome_;
	delete mapChipField_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
		{
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
}