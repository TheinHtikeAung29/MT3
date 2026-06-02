#include <Novice.h>
#include <cmath>
#include <imgui.h>
#include "corecrt_math_defines.h"

const char kWindowTitle[] = "GC2A_02_テイン_タイ_アウン";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

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

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
	return result;
}

float Length(const Vector3& v) {
	float result;
	result = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return result;
};

bool IsCollision(const Sphere& s1, const Sphere& s2) {
	float dist = Length(Subtract(s1.center, s2.center));
	return dist <= s1.radius + s2.radius;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
	return result;
}

Matrix4x4 MakeRotateXMatrix(float angle) { Matrix4x4 r = {};
	r.m[0][0] = 1.0f;
	r.m[1][1] = std::cos(angle);
	r.m[1][2] = std::sin(angle);
	r.m[2][1] = -std::sin(angle);
	r.m[2][2] = std::cos(angle);
	r.m[3][3] = 1.0f;
	return r;
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

Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result = {};
	float src[4][4] = {};
	float inv[4][4] = {};
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			src[i][j] = m.m[i][j];

	float det = 0.0f;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float minor[3][3] = {};
			int ri = 0;
			for (int r = 0; r < 4; r++) {
				if (r == i)
					continue;
				int ci = 0;
				for (int c = 0; c < 4; c++) {
					if (c == j)
						continue;
					minor[ri][ci] = src[r][c];
					ci++;
				}
				ri++;
			}
			float cof = minor[0][0] * (minor[1][1] * minor[2][2] - minor[1][2] * minor[2][1]) - minor[0][1] * (minor[1][0] * minor[2][2] - minor[1][2] * minor[2][0]) +
			            minor[0][2] * (minor[1][0] * minor[2][1] - minor[1][1] * minor[2][0]);
			if ((i + j) % 2 != 0)
				cof = -cof;
			inv[j][i] = cof;
			if (j == 0)
				det += src[i][0] * cof;
		}
	}
	float invDet = 1.0f / det;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			result.m[i][j] = inv[i][j] * invDet;
	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspect, float nearClip, float farClip) {
	Matrix4x4 result = {};
	float t = std::tan(fovY / 2.0f);
	result.m[0][0] = 1.0f / (aspect * t);
	result.m[1][1] = 1.0f / t;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minD, float maxD) {
	Matrix4x4 result = {};
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxD - minD;
	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minD;
	result.m[3][3] = 1.0f;
	return result;
}

Vector3 Transform(const Vector3& v, const Matrix4x4& mat) {
	float x = v.x * mat.m[0][0] + v.y * mat.m[1][0] + v.z * mat.m[2][0] + mat.m[3][0];
	float y = v.x * mat.m[0][1] + v.y * mat.m[1][1] + v.z * mat.m[2][1] + mat.m[3][1];
	float z = v.x * mat.m[0][2] + v.y * mat.m[1][2] + v.z * mat.m[2][2] + mat.m[3][2];
	float w = v.x * mat.m[0][3] + v.y * mat.m[1][3] + v.z * mat.m[2][3] + mat.m[3][3];
	return {x / w, y / w, z / w};
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + kGridEvery * float(xIndex);
		Vector3 start = {x, 0.0f, -kGridHalfWidth};
		Vector3 end = {x, 0.0f, kGridHalfWidth};
		Vector3 sScr = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 eScr = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		uint32_t color = (xIndex == kSubdivision / 2) ? 0x000000FF : 0xAAAAAAFF;
		Novice::DrawLine(int(sScr.x), int(sScr.y), int(eScr.x), int(eScr.y), color);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + kGridEvery * float(zIndex);
		Vector3 start = {-kGridHalfWidth, 0.0f, z};
		Vector3 end = {kGridHalfWidth, 0.0f, z};
		Vector3 sScr = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 eScr = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		uint32_t color = (zIndex == kSubdivision / 2) ? 0x000000FF : 0xAAAAAAFF;
		Novice::DrawLine(int(sScr.x), int(sScr.y), int(eScr.x), int(eScr.y), color);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLonEvery = (2.0f * float(M_PI)) / float(kSubdivision);
	const float kLatEvery = float(M_PI) / float(kSubdivision);

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI) / 2.0f + kLatEvery * float(latIndex);
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			Vector3 a = {
			    sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon), sphere.center.y + sphere.radius * std::sin(lat), sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon)};
			Vector3 b = {
			    sphere.center.x + sphere.radius * std::cos(lat + kLatEvery) * std::cos(lon), sphere.center.y + sphere.radius * std::sin(lat + kLatEvery),
			    sphere.center.z + sphere.radius * std::cos(lat + kLatEvery) * std::sin(lon)};
			Vector3 c = {
			    sphere.center.x + sphere.radius * std::cos(lat) * std::cos(lon + kLonEvery), sphere.center.y + sphere.radius * std::sin(lat),
			    sphere.center.z + sphere.radius * std::cos(lat) * std::sin(lon + kLonEvery)};
			Vector3 as = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 bs = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 cs = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);
			Novice::DrawLine(int(as.x), int(as.y), int(bs.x), int(bs.y), color);
			Novice::DrawLine(int(as.x), int(as.y), int(cs.x), int(cs.y), color);
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 cameraTranslate = {0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate = {0.26f, 0.0f, 0.0f};

	Sphere sphere1 = {
	    {0.0f, 0.0f, 0.0f},
        1.0f
    };
	Sphere sphere2 = {
	    {0.0f, 0.0f, -3.0f},
        1.0f
    };

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

		const float kMoveSpeed = 0.05f;
		const float kRotateSpeed = 0.02f;

		if (keys[DIK_W])
			cameraTranslate.z += kMoveSpeed;
		if (keys[DIK_S])
			cameraTranslate.z -= kMoveSpeed;
		if (keys[DIK_A])
			cameraTranslate.x -= kMoveSpeed;
		if (keys[DIK_D])
			cameraTranslate.x += kMoveSpeed;
		if (keys[DIK_UP])
			cameraTranslate.y += kMoveSpeed;
		if (keys[DIK_DOWN])
			cameraTranslate.y -= kMoveSpeed;
		if (keys[DIK_Q])
			cameraRotate.y -= kRotateSpeed;
		if (keys[DIK_E])
			cameraRotate.y += kRotateSpeed;

		bool collision = IsCollision(sphere1, sphere2);

		ImGui::Begin("Window");
		ImGui::DragFloat3("Sphere[0].Center", &sphere1.center.x, 0.01f);
		ImGui::DragFloat("Sphere[0].Radius", &sphere1.radius, 0.01f);
		ImGui::DragFloat3("Sphere[1].Center", &sphere2.center.x, 0.01f);
		ImGui::DragFloat("Sphere[1].Radius", &sphere2.radius, 0.01f);
		ImGui::End();

		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawSphere(sphere1, viewProjectionMatrix, viewportMatrix, collision ? RED : WHITE);
		DrawSphere(sphere2, viewProjectionMatrix, viewportMatrix, WHITE);

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
