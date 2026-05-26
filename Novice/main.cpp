#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "GC2A_02_テイン_タイ_アウン";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

struct Vector3 {
	float x, y, z;
};

struct Matrix4x4 {
	float m[4][4];
};

Vector3 Cross(const Vector3& v1, const Vector3& v2) { return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x}; }

Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

Matrix4x4 MakeRotateYMatrix(float angle) {
	Matrix4x4 result = {};
	result.m[0][0] = std::cos(angle);
	result.m[0][2] = -std::sin(angle);
	result.m[1][1] = 1.0f;
	result.m[2][0] = std::sin(angle);
	result.m[2][2] = std::cos(angle);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate) {
	Matrix4x4 rotY = MakeRotateYMatrix(rotate.y);

	Matrix4x4 result = {};
	result.m[0][0] = scale.x * rotY.m[0][0];
	result.m[0][1] = scale.x * rotY.m[0][1];
	result.m[0][2] = scale.x * rotY.m[0][2];
	result.m[1][0] = scale.y * rotY.m[1][0];
	result.m[1][1] = scale.y * rotY.m[1][1];
	result.m[1][2] = scale.y * rotY.m[1][2];
	result.m[2][0] = scale.z * rotY.m[2][0];
	result.m[2][1] = scale.z * rotY.m[2][1];
	result.m[2][2] = scale.z * rotY.m[2][2];
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result = {};
	float det = 0.0f;

	float inv[4][4] = {};
	float src[4][4] = {};

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			src[i][j] = m.m[i][j];

	// cofactor expansion
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			// 3x3 minor
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
			float cofactor = minor[0][0] * (minor[1][1] * minor[2][2] - minor[1][2] * minor[2][1]) - minor[0][1] * (minor[1][0] * minor[2][2] - minor[1][2] * minor[2][0]) +
			                 minor[0][2] * (minor[1][0] * minor[2][1] - minor[1][1] * minor[2][0]);
			if ((i + j) % 2 != 0)
				cofactor = -cofactor;
			inv[j][i] = cofactor; 
			if (j == 0)
				det += src[i][0] * cofactor;
		}
	}

	float invDet = 1.0f / det;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			result.m[i][j] = inv[i][j] * invDet;

	return result;
}

Vector3 Transform(const Vector3& v, const Matrix4x4& mat) {
	float x = v.x * mat.m[0][0] + v.y * mat.m[1][0] + v.z * mat.m[2][0] + mat.m[3][0];
	float y = v.x * mat.m[0][1] + v.y * mat.m[1][1] + v.z * mat.m[2][1] + mat.m[3][1];
	float z = v.x * mat.m[0][2] + v.y * mat.m[1][2] + v.z * mat.m[2][2] + mat.m[3][2];
	float w = v.x * mat.m[0][3] + v.y * mat.m[1][3] + v.z * mat.m[2][3] + mat.m[3][3];
	return {x / w, y / w, z / w};
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};
	float tanHalfFov = std::tan(fovY / 2.0f);
	result.m[0][0] = 1.0f / (aspectRatio * tanHalfFov);
	result.m[1][1] = 1.0f / tanHalfFov;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result = {};
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;
	return result;
}

static const int kRowHeight = 20;
static const int kColumnWidth = 60;

void VectorScreenPrintf(int x, int y, const Vector3& v, const char* label) { Novice::ScreenPrintf(x, y, "%s: (%6.02f, %6.02f, %6.02f)", label, v.x, v.y, v.z); }

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	const Vector3 kLocalVertices[3] = {
	    {0.0f,  1.0f,  0.0f},
	    {1.0f,  -1.0f, 0.0f},
	    {-1.0f, -1.0f, 0.0f},
	};

	const Vector3 kCameraPosition = {0.0f, 0.0f, -10.0f};

		Vector3 translate = {0.0f, 0.0f, 0.0f};
	Vector3 rotate = {0.0f, 0.0f, 0.0f};
 
	Vector3 v1 = {1.2f, -3.9f, 2.5f};
	Vector3 v2 = {2.8f, 0.4f, -1.3f};
	Vector3 cross = Cross(v1, v2);

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
		if (keys[DIK_W])
			translate.z += kMoveSpeed;
		if (keys[DIK_S])
			translate.z -= kMoveSpeed;
		if (keys[DIK_A])
			translate.x -= kMoveSpeed;
		if (keys[DIK_D])
			translate.x += kMoveSpeed;

		rotate.y += 0.02f;

		Matrix4x4 worldMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, kCameraPosition);
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

			Vector3 screenVertices[3];
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, cross, "Cross");

		Novice::DrawTriangle(
		    int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), int(screenVertices[2].x), int(screenVertices[2].y), RED, kFillModeSolid);

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
