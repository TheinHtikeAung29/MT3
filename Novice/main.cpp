#include <Novice.h>
#include <cmath>
#include <imgui.h>

const char kWindowTitle[] = "GC2A_02_テイン_タイ_アウン";

struct Vector3 {
	float x, y, z;
};

struct Matrix4x4 {
	float m[4][4];
};

struct Sphere {
	Vector3 center;
	float radius;
};

Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
	return result;
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
	return result;
}

Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 result;
	result = {scalar * v.x, scalar * v.y, scalar * v.z};
	return result;
}

float Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = {(v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z)};
	return result;
};

float Length(const Vector3& v) {
	float result;
	result = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
};

Vector3 Normalize(const Vector3& v) {
	Vector3 result;

	float length = Length(v);

	if (length != 0.0f) {
		result = {v.x / length, v.y / length, v.z / length};
	} else {
		result = {0.0f};
	}

	return result;
};

Vector3 Scale(const Vector3& v, float s) { return {v.x * s, v.y * s, v.z * s}; }

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float dist = Length(Subtract(s1.center, s2.center));
	return dist <= s1.radius + s2.radius;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4 m2) { Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
				return result;
			}
		}
	}
}

Matrix4x4 MakeRotateXMatrix(float angle) { Matrix4x4 r = {};
	r.m[0][0] = 1.0f;
	r.m[1][1] = std::cos(angle);
	r.m[1][2] = std::sin(angle);
	r.m[2][1] = -std::sin(angle);
	r.m[2][2] = std::cos(angle);
	r.m[3][3] = 1.0f;
}

Matrix4x4 MakeRotateYMatrix(float angle) { Matrix4x4 r = {};
	r.m[0][0] = std::cos(angle);
	r.m[0][2] = -std::sin(angle);
	r.m[1][1] = 1.0f;
	r.m[2][0] = std::sin(angle);
	r.m[2][2] = std::cos(angle);
	r.m[3][3] = 1.0f;
	return r;
}

Matrix4x4 MakeRotateZMatrix(float angle) {
	Matrix4x4 r = {};
	r.m[0][0] = std::cos(angle);
	r.m[0][1] = std::sin(angle);
	r.m[1][0] = -std::sin(angle);
	r.m[1][1] = std::cos(angle);
	r.m[2][2] = 1.0f;
	r.m[3][3] = 1.0f;
	return r;
}

Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate) {
	Matrix4x4 rotX = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotY = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotZ = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rot = Multiply(rotX, Multiply(rotY, rotZ));
	Matrix4x4 result = {};
	result.m[0][0] = scale.x * rot.m[0][0];
	result.m[0][1] = scale.x * rot.m[0][1];
	result.m[0][2] = scale.x * rot.m[0][2];
	result.m[1][0] = scale.y * rot.m[1][0];
	result.m[1][1] = scale.y * rot.m[1][1];
	result.m[1][2] = scale.y * rot.m[1][2];
	result.m[2][0] = scale.z * rot.m[2][0];
	result.m[2][1] = scale.z * rot.m[2][1];
	result.m[2][2] = scale.z * rot.m[2][2];
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;
	return result;
}


static const int kColumnWidth = 60;
static const int kRowHeight = 20;
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%0.2f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 v1{1.0f, 3.0f, -5.0f};
	Vector3 v2{4.0f, -1.0f, 2.0f};
	float k = {4.0f};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Vector3 resultAdd = Add(v1, v2);
		Vector3 resultSubtract = Subtract(v1, v2);
		Vector3 resultMultiply = Multiply(k, v1);
		float resultDot = Dot(v1, v2);
		float resultLength = Length(v1);
		Vector3 resultNormalize = Normalize(v2);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, resultAdd, " : Add");
		VectorScreenPrintf(0, kRowHeight, resultSubtract, " : Subtract");
		VectorScreenPrintf(0, kRowHeight * 2, resultMultiply, " : Multiply");
		Novice::ScreenPrintf(0, kRowHeight * 3, "%.02f  : Dot", resultDot);
		Novice::ScreenPrintf(0, kRowHeight * 4, "%.02f  : Length", resultLength);
		VectorScreenPrintf(0, kRowHeight * 5, resultNormalize, " : Normalize");

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
