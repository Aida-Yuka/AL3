#pragma once

//前方宣言
class Player;

/// <summary>
/// カメラコントローラー
/// </summary>
class CameraController
{
public:

	void Initialize(KamataEngine::Camera* camera);

	void Update();

	void SetTarget_(Player Target) { target_ = target; }

	//瞬間合わせ
	void Reset();

private:

	//カメラ
	KamataEngine::Camera camera_;

	//プレイヤーにポインタ
	Player* target_ = nullptr;

	//追従対象とカメラの座標の差(オフセット)
	Vector3 targetOffset_ = { 0,0,-15.0f };
};

