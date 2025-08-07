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

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i)
{
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

// 全ての当たり判定を行う(最後尾)
void GameScene::CheckAllCollisions()
{
#pragma region 自キャラと敵キャラの当たり判定
	// 判定対象1と2の座標
	AABB aabb1, aabb2;
	//自キャラの座標
	aabb1 = player_->GetAABB();
	//自キャラと敵弾全ての当たり判定
	for (Enemy* enemy : enemies_)
	{
		//敵の座標
		aabb2 = enemy->GetAABB();
		//AABBの交差判定
		if (IsCollision(aabb1, aabb2))
		{
			//自キャラの衝突時関数を呼び出す
			player_->OnCollision(enemy);
			//敵の衝突時関数を呼び出す
			enemy->OnCollision(player_);
		}
	}

#pragma endregion
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
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle", true);

	//マップチップフィールドの設定
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	GenerateBlocks();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 18);

	// 自キャラの生成
	player_ = new Player();

	// 自キャラの初期化
	player_->Initialize(model_, &camera_, playerPosition);

	//マップチップデータのセット
	player_->SetMapChipField(mapChipField_);

	//天球の生成
	skydome_ = new Skydome();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1600, 800);

	// カメラの初期化
	camera_.Initialize();

	//天球の初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	//敵の生成と初期化
	const int enemyNum = 3;
	for (int32_t i = 0; i < enemyNum; i++)
	{
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(30, 18 - i);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);
		enemies_.push_back(newEnemy);
	}

	//デスパーティクルの初期化
	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(modelDeathParticle_, &camera_, playerPosition);

	///===カメラコントローラの初期化===
	//生成
	cameraController_ = new CameraController();
	//初期化
	cameraController_->Intialize();
	//追従対象をセット
	cameraController_->SetTarget(player_);
	//リセット(瞬間合わせ)
	cameraController_->Reset();

	// カメラ移動範囲
	CameraController::Rect cameraArea = {12.0f, 100.0f - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
}

void GameScene::Update()
{
	/// インゲームの更新処理///

	// ワールドトランスフォームの初期化
	//worldTransform_.Initialize();
	
	// カメラコントローラーの更新
	cameraController_->Update();

	// 自キャラの更新
	player_->Update();

	// 敵の更新
	for (Enemy* enemy : enemies_)
	{
		enemy->Update();
	}

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
	if (isDebugCameraActive_)
	{
		debugCamera_->Update();

		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;

		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();

	} else
	{
		// ビュープロジェクション行列の更新
		//camera_.UpdateMatrix();
		
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;

		//ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	}

	//追従カメラ＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝	

	if (deathParticles_)
	{
		deathParticles_->Update();
	}

	CheckAllCollisions();
}

void GameScene::Draw()
{
	// 描画処理///

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	/// モデルインスタンスの描画処理//
	// 自キャラの描画
	player_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	// 天球の描画
	skydome_->Draw();

	//敵の描画
	for (Enemy* enemy : enemies_)
	{
		enemy->Draw();
	}

	if (deathParticles_)
	{
		deathParticles_->Draw();
	}

	///＝＝＝＝＝＝＝＝＝＝＝＝＝＝///

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
	delete skydome_;
	delete mapChipField_;
	for (Enemy* enemy : enemies_)
	{
		delete enemy;
	}
	delete modelEnemy_;
	delete modelDeathParticle_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_)
	{
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine)
		{
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
}