#include "CameraController.h"
#include "KamataEngine.h"
#include "Player.h"

using namespace KamataEngine;

void CameraController::Initialize(Camera* camera_) {
	/// インゲームの初期化処理///

	//カメラの初期化
	camera_ = camera;
}

void CameraController::Update() {
	/// インゲームの更新処理///

	//追従対象のワールドトランスフォームを参照する
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	//追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation_ * targetOffset_;
	//行列を更新する
	camera_.UpdateMatrix();
}

void CameraController::Reset()
{
	//追従対象のワールドトランスフォームを参照
	const worldTransform& targetWorldform = target_->GetworldTransform();
	//追従対象とオフセットからカメラの座標を計算
	camera_.translation_ = targetWorldTransform.translation = targetOffset_;
}