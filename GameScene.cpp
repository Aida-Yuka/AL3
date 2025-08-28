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

//全ての当たり判定を行う(最後尾)
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

//フェーズの切り替え
void GameScene::ChangePhase()
{
	switch (phase_)
	{
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			// ゲームプレイに切り替え
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			// シーン終了へ
			finished_ = true;
		}
		break;
	case Phase::kPlay:

		//＝＝＝ゲームプレイフェーズの処理＝＝＝

		// Mキーを押すと
		if (Input::GetInstance()->PushKey(DIK_M))
		{
			//メニュー画面に切り替え
			phase_ = Phase::kPauseMenu;
		}

		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;

			// 自キャラの座標を取得
			const Vector3& deathParticlePosition = player_->GetWorldPosition();

			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticle_, &camera_, deathParticlePosition);
		}

		//if (player_->IsDead())
		//{
		//	// 死亡演出フェーズに切り替え
		//	phase_ = Phase::kDeath;
		//}

		break;
	case Phase::kDeath:

		//＝＝＝デス演出フェーズの処理＝＝＝

		// デスパーティクルが終了したらシーンを終了する
		if (deathParticles_ && deathParticles_->IsFinished())
		{
			//フェードアウト開始
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}

		break;

	case Phase::kPauseMenu:

		//1キーを押すと
		if (Input::GetInstance()->PushKey(DIK_1))
		{
			// フェードアウト開始
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1.0f);
		}

		//2を押すと
		if (Input::GetInstance()->PushKey(DIK_2))
		{
			//ゲームに戻る
			phase_ = Phase::kPlay;
		}

		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
		}

		break;
	}
}

void GameScene::Initialize()
{
	///インゲームの初期化処理///

	// ゲームプレイフェーズからの開始
	phase_ = Phase::kFadeIn;

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// 3Dモデルの生成
	model_ = Model::CreateFromOBJ("player",true);
	modelBlock_ = Model::CreateFromOBJ("block");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	modelDeathParticle_ = Model::CreateFromOBJ("deathParticle", true);
	modelMenu_ = Model::CreateFromOBJ("pauseMenu", true);
	modelMenu2_ = Model::CreateFromOBJ("menu", true);

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

	// フェード
	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//表示メニュー
	Vector3 PausePosition = player_->GetWorldPosition();
	PausePosition.x = -5.0f;
	PausePosition.y = 8.0f;
	PausePosition.z = -2.0f;
	worldTransform_.translation_ = PausePosition;
	worldTransform_.rotation_.x = 3.14f / 2.0f;
	worldTransform_.rotation_.y = 3.14f;
}

void GameScene::Update()
{
	/// インゲームの更新処理///

	Vector3 PausePosition = player_->GetWorldPosition();
	PausePosition.y = 8.0f;
	PausePosition.z = -2.0f;
	worldTransform_.translation_ = PausePosition;
	worldTransform_.rotation_.x = 3.14f / 2.0f;
	worldTransform_.rotation_.y = 3.14f;

	// アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// フェード
	fade_->Update();

	switch (phase_) {
	case Phase::kPlay:

		// ＝＝＝ゲームプレイフェーズの処理＝＝＝
		// 全ての当たり判定を行う
		CheckAllCollisions();

		/*Vector3 PausePosition = player_->GetWorldPosition();
		PausePosition.y = 8.0f;
		PausePosition.z = -2.0f;
		worldTransform_.translation_ = PausePosition;
		worldTransform_.rotation_.x = 3.14f / 2.0f;
		worldTransform_.rotation_.y = 3.14f;*/

		// アフィン変換行列の作成
		worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

		// 行列を定数バッファに転送
		worldTransform_.TransferMatrix();

		break;

	case Phase::kDeath:

		// ＝＝＝デス演出フェーズの処理＝＝＝

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// デスパーティクルの更新
		if (deathParticles_) {
			deathParticles_->Update();
		}

		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();

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
		break;
	case Phase::kPauseMenu:

		// 全ての当たり判定を行う
		CheckAllCollisions();

		//
		/*Vector3 PausePosition = player_->GetWorldPosition();
		PausePosition.y = 8.0f;
		PausePosition.z = -2.0f;
		worldTransform_.translation_ = PausePosition;
		worldTransform_.rotation_.x = 3.14f / 2.0f;
		worldTransform_.rotation_.y = 3.14f;*/

		// アフィン変換行列の作成
		worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

		// 行列を定数バッファに転送
		worldTransform_.TransferMatrix();

		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();

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
		break;	
	case Phase::kFadeIn:
		// フェード
		fade_->Update();
		break;
	 case Phase::kFadeOut:
		// フェード
		fade_->Update();
		break;
	}

	///＝＝＝共通の処理＝＝＝

	//フェーズの切り替え
	ChangePhase();

	// 自キャラの更新
	player_->Update();

	// 敵の更新
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	// カメラの処理
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	if (isDebugCameraActive_) {
		debugCamera_->Update();

		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;

		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();

	} else {
		// ビュープロジェクション行列の更新
		// camera_.UpdateMatrix();

		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;

		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	}

	// カメラコントローラーの更新
	cameraController_->Update();

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

	if (phase_ == Phase::kPlay)
	{
		// 3Dモデルを描画
		modelMenu2_->Draw(worldTransform_, camera_);
	}

	if (phase_ == Phase::kPauseMenu)
	{
		// 3Dモデルを描画
		modelMenu_->Draw(worldTransform_, camera_);
	}

	//デスパーティクルの描画
	if (deathParticles_) {
		deathParticles_->Draw();
	}

	// フェード
	fade_->Draw();

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
	delete fade_;
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