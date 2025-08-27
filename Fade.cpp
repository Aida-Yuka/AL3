#include "Fade.h"
#include "Mymath.h"
#include <algorithm>

using namespace KamataEngine;

void Fade::Initialize()
{
	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("white1x1.png");
	//スプライトインスタンスの生成
	sprite_ = Sprite::Create(textureHandle_, {0, 0});
	sprite_->SetSize(Vector2(1280, 720));   // 画面サイズ
	sprite_->SetColor(Vector4(0, 0, 0, 1)); // 黒色
}

void Fade::Update()
{
	//フェード状態による分岐
	switch (status_)
	{
	case Status::None:
		//＝＝＝何もしない＝＝＝

		break;
	case Status::FadeIn:
		//＝＝＝フェードイン中の更新処理＝＝＝
		
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_)
		{
			counter_ = duration_;
		}
		// 経過時間がフェードアウト継続時間に近付くほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f)));

		break;
	
	case Status::FadeOut:
		//＝＝＝フェードアウトの更新処理＝＝＝

		//1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		//フェード継続時間に達したら打ち止め
		if (counter_ >= duration_)
		{
			counter_ = duration_;
		}
		//経過時間がフェードアウト継続時間に近付くほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

void Fade::Draw()
{
	//フェードの終了
	if (status_ == Status::None)
	{
		return;
	}

	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	Sprite::PreDraw(dxCommon->GetCommandList());
	sprite_->Draw();
	Sprite::PostDraw();
}

//フェード開始処理
void Fade::Start(Status status, float duration)
{
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

//フェード終了
void Fade::Stop()
{
	status_ = Status::None;
}

bool Fade::IsFinished() const
{
	// フェード状態による分岐
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}
	
	return true;
}
