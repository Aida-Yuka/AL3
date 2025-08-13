#include <Windows.h>
#include "KamataEngine.h"
#include "GameScene.h"
#include "TitleScene.h"

using namespace KamataEngine;

//シーン(型)
enum class Scene {
	kUnknown = 0,

	kTitle,
	kGame,
};

//各シーンのポインタ変数
GameScene* gameScene = nullptr;
TitleScene* titleScene = nullptr;

//現在シーン(型)
Scene scene = Scene::kUnknown;

void ChangeScene()
{
	switch (scene)
	{
	case Scene::kTitle:
		if (titleScene->IsFinished())
		{
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		}
		break;
	}
}

//シーンの更新
void UpdateScene()
{
	switch (scene)
	{ case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

// シーンの描画
void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// エンジンの初期化
	KamataEngine::Initialize(L"LE2D_01_アイダ_ユウカ_AL3");

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// InguiManagerインスタンスの取得
	// ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	//最初のシーンの初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();

	// メインループ
	while (true)
	{
		// エンジンの更新
		if (KamataEngine::Update())
		{
			break;
		}

		// ゲームシーンの更新
		//titleScene->Update();

		//シーン切り替え
		ChangeScene();

		//現在シーン更新 
		UpdateScene();

		// 描画開始
		dxCommon->PreDraw();

		// ゲームシーンの描画
		//titleScene->Draw();

		//現在シーンの描画
		DrawScene();

		// 描画終了
		dxCommon->PostDraw();
	}

	// ゲームシーンの解放
	delete titleScene;
	delete gameScene;

	// nullptrの代入
	//gameScene = nullptr;
	titleScene = nullptr;

	// エンジンの終了処理
	KamataEngine::Finalize();

	return 0;
}