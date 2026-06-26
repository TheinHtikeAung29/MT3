#include <Novice.h>
#include <cmath>
#include <imgui.h>

const char kWindowTitle[] = "GC2A_02_テイン_タイ_アウン";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

typedef struct Vector3 {
	float x, y, z;
} Vector3;

typedef struct Matrix4x4 {
	float m[4][4];
} Matrix4x4;

typedef struct Sphere {
	Vector3 center;
	float radius;
} Sphere;

typedef struct Plane {
	Vector3 normal;
	float distance; 
} Plane;

Vector3 Add(const Vector3& a, const Vector3& b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }

Vector3 Multiply(float s, const Vector3& v) { return {s * v.x, s * v.y, s * v.z}; }

float Dot(const Vector3& a, const Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

float Length(const Vector3& v) { return sqrtf(Dot(v, v)); }

Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	if (len == 0.0f)
		return {0.0f, 0.0f, 0.0f};
	return {v.x / len, v.y / len, v.z / len};
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) { return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x}; }

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int row = 0; row < 4; row++)
		for (int col = 0; col < 4; col++)
			for (int k = 0; k < 4; k++)
				result.m[row][col] += m1.m[row][k] * m2.m[k][col];
	return result;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result = {};
	float det =
	    m.m[0][0] *
	        (m.m[1][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2]) - m.m[1][2] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) + m.m[1][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1])) -
	    m.m[0][1] *
	        (m.m[1][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2]) - m.m[1][2] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0]) + m.m[1][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0])) +
	    m.m[0][2] *
	        (m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) - m.m[1][1] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0]) + m.m[1][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0])) -
	    m.m[0][3] *
	        (m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]) - m.m[1][1] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]) + m.m[1][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]));
	float invDet = 1.0f / det;
	result.m[0][0] = invDet * (m.m[1][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2]) - m.m[1][2] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) +
	                           m.m[1][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]));
	result.m[0][1] = invDet * -(m.m[0][1] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2]) - m.m[0][2] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) +
	                            m.m[0][3] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]));
	result.m[0][2] = invDet * (m.m[0][1] * (m.m[1][2] * m.m[3][3] - m.m[1][3] * m.m[3][2]) - m.m[0][2] * (m.m[1][1] * m.m[3][3] - m.m[1][3] * m.m[3][1]) +
	                           m.m[0][3] * (m.m[1][1] * m.m[3][2] - m.m[1][2] * m.m[3][1]));
	result.m[0][3] = invDet * -(m.m[0][1] * (m.m[1][2] * m.m[2][3] - m.m[1][3] * m.m[2][2]) - m.m[0][2] * (m.m[1][1] * m.m[2][3] - m.m[1][3] * m.m[2][1]) +
	                            m.m[0][3] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]));
	result.m[1][0] = invDet * -(m.m[1][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2]) - m.m[1][2] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0]) +
	                            m.m[1][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]));
	result.m[1][1] = invDet * (m.m[0][0] * (m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2]) - m.m[0][2] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0]) +
	                           m.m[0][3] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]));
	result.m[1][2] = invDet * -(m.m[0][0] * (m.m[1][2] * m.m[3][3] - m.m[1][3] * m.m[3][2]) - m.m[0][2] * (m.m[1][0] * m.m[3][3] - m.m[1][3] * m.m[3][0]) +
	                            m.m[0][3] * (m.m[1][0] * m.m[3][2] - m.m[1][2] * m.m[3][0]));
	result.m[1][3] = invDet * (m.m[0][0] * (m.m[1][2] * m.m[2][3] - m.m[1][3] * m.m[2][2]) - m.m[0][2] * (m.m[1][0] * m.m[2][3] - m.m[1][3] * m.m[2][0]) +
	                           m.m[0][3] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0]));
	result.m[2][0] = invDet * (m.m[1][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) - m.m[1][1] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0]) +
	                           m.m[1][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]));
	result.m[2][1] = invDet * -(m.m[0][0] * (m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1]) - m.m[0][1] * (m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0]) +
	                            m.m[0][3] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]));
	result.m[2][2] = invDet * (m.m[0][0] * (m.m[1][1] * m.m[3][3] - m.m[1][3] * m.m[3][1]) - m.m[0][1] * (m.m[1][0] * m.m[3][3] - m.m[1][3] * m.m[3][0]) +
	                           m.m[0][3] * (m.m[1][0] * m.m[3][1] - m.m[1][1] * m.m[3][0]));
	result.m[2][3] = invDet * -(m.m[0][0] * (m.m[1][1] * m.m[2][3] - m.m[1][3] * m.m[2][1]) - m.m[0][1] * (m.m[1][0] * m.m[2][3] - m.m[1][3] * m.m[2][0]) +
	                            m.m[0][3] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]));
	result.m[3][0] = invDet * -(m.m[1][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]) - m.m[1][1] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]) +
	                            m.m[1][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]));
	result.m[3][1] = invDet * (m.m[0][0] * (m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1]) - m.m[0][1] * (m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0]) +
	                           m.m[0][2] * (m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0]));
	result.m[3][2] = invDet * -(m.m[0][0] * (m.m[1][1] * m.m[3][2] - m.m[1][2] * m.m[3][1]) - m.m[0][1] * (m.m[1][0] * m.m[3][2] - m.m[1][2] * m.m[3][0]) +
	                            m.m[0][2] * (m.m[1][0] * m.m[3][1] - m.m[1][1] * m.m[3][0]));
	result.m[3][3] = invDet * (m.m[0][0] * (m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1]) - m.m[0][1] * (m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0]) +
	                           m.m[0][2] * (m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0]));
	return result;
}

Vector3 Transform(const Vector3& v, const Matrix4x4& m) {
	float x = v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0] + m.m[3][0];
	float y = v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1] + m.m[3][1];
	float z = v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2] + m.m[3][2];
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];
	return {x / w, y / w, z / w};
}

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[0][0] = 1.0f;
	result.m[1][1] = cosf(radian);
	result.m[1][2] = -sinf(radian);
	result.m[2][1] = sinf(radian);
	result.m[2][2] = cosf(radian);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[0][0] = cosf(radian);
	result.m[0][2] = sinf(radian);
	result.m[1][1] = 1.0f;
	result.m[2][0] = -sinf(radian);
	result.m[2][2] = cosf(radian);
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[0][0] = cosf(radian);
	result.m[0][1] = -sinf(radian);
	result.m[1][0] = sinf(radian);
	result.m[1][1] = cosf(radian);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));
	Matrix4x4 result = {};
	for (int row = 0; row < 3; row++)
		for (int col = 0; col < 3; col++)
			result.m[row][col] = rotateMatrix.m[row][col] * (&scale.x)[row];
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;
	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};
	float tanHalfFovY = tanf(fovY / 2.0f);
	result.m[0][0] = 1.0f / (aspectRatio * tanHalfFovY);
	result.m[1][1] = 1.0f / tanHalfFovY;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = -nearClip * farClip / (farClip - nearClip);
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

bool IsCollision(const Sphere& sphere, const Plane& plane) {
	float d = Dot(plane.normal, sphere.center) - plane.distance;
	return fabsf(d) <= sphere.radius;
}

Vector3 Perpendicular(const Vector3& v) {
	if (v.x != 0.0f || v.y != 0.0f) {
		return {-v.y, v.x, 0.0f};
	}
	return {0.0f, -v.z, v.y};
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfExtent = 2.0f;
	const uint32_t kSubdivision = 10;
	const float step = kGridHalfExtent * 2.0f / float(kSubdivision);

	for (uint32_t i = 0; i <= kSubdivision; ++i) {
		float x = -kGridHalfExtent + step * float(i);
		float z = -kGridHalfExtent + step * float(i);

		Vector3 startX = Transform(Transform({x, 0.0f, -kGridHalfExtent}, viewProjectionMatrix), viewportMatrix);
		Vector3 endX = Transform(Transform({x, 0.0f, kGridHalfExtent}, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(int(startX.x), int(startX.y), int(endX.x), int(endX.y), 0xAAAAAAFF);

		Vector3 startZ = Transform(Transform({-kGridHalfExtent, 0.0f, z}, viewProjectionMatrix), viewportMatrix);
		Vector3 endZ = Transform(Transform({kGridHalfExtent, 0.0f, z}, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(int(startZ.x), int(startZ.y), int(endZ.x), int(endZ.y), 0xAAAAAAFF);
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kpi = 3.14159265f;
	const float kLatStep = kpi / float(kSubdivision);
	const float kLonStep = 2.0f * kpi / float(kSubdivision);

	for (uint32_t lat = 0; lat < kSubdivision; ++lat) {
		float theta = -kpi / 2.0f + kLatStep * float(lat);
		float theta1 = theta + kLatStep;

		for (uint32_t lon = 0; lon < kSubdivision; ++lon) {
			float phi = kLonStep * float(lon);
			float phi1 = phi + kLonStep;

			Vector3 a = {sphere.center.x + sphere.radius * cosf(theta) * cosf(phi), sphere.center.y + sphere.radius * sinf(theta), sphere.center.z + sphere.radius * cosf(theta) * sinf(phi)};
			Vector3 b = {sphere.center.x + sphere.radius * cosf(theta1) * cosf(phi), sphere.center.y + sphere.radius * sinf(theta1), sphere.center.z + sphere.radius * cosf(theta1) * sinf(phi)};
			Vector3 c = {sphere.center.x + sphere.radius * cosf(theta) * cosf(phi1), sphere.center.y + sphere.radius * sinf(theta), sphere.center.z + sphere.radius * cosf(theta) * sinf(phi1)};
			Vector3 d = {sphere.center.x + sphere.radius * cosf(theta1) * cosf(phi1), sphere.center.y + sphere.radius * sinf(theta1), sphere.center.z + sphere.radius * cosf(theta1) * sinf(phi1)};

			Vector3 sa = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			Vector3 sb = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			Vector3 sc = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);
			Vector3 sd = Transform(Transform(d, viewProjectionMatrix), viewportMatrix);

			Novice::DrawLine(int(sa.x), int(sa.y), int(sb.x), int(sb.y), color);
			Novice::DrawLine(int(sa.x), int(sa.y), int(sc.x), int(sc.y), color);
			Novice::DrawLine(int(sb.x), int(sb.y), int(sd.x), int(sd.y), color);
			Novice::DrawLine(int(sc.x), int(sc.y), int(sd.x), int(sd.y), color);
		}
	}
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = Multiply(plane.distance, plane.normal);

	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal));
	perpendiculars[1] = {-perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z};
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = {-perpendiculars[2].x, -perpendiculars[2].y, -perpendiculars[2].z};

	Vector3 points[4];
	for (int32_t i = 0; i < 4; ++i) {
		Vector3 extend = Multiply(2.0f, perpendiculars[i]);
		Vector3 point = Add(center, extend);
		points[i] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 cameraTranslate = {0.180f, -2.0f, 6.49f};
	Vector3 cameraRotate = {-0.226f, -0.02f, 0.0f};

	Sphere sphere = {
	    {0.12f, 0.24f, 0.0f},
        0.6f
    };

	Plane plane = {Normalize({0.0f, 1.0f, 0.0f}), 0.0f};

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

		ImGui::Begin("Window");
		ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("Sphere.Center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("Sphere.Radius", &sphere.radius, 0.01f);
		ImGui::DragFloat3("Plane.Normal", &plane.normal.x, 0.01f);
		ImGui::DragFloat("Plane.Distance", &plane.distance, 0.01f);
		ImGui::End();

		plane.normal = Normalize(plane.normal);

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

		bool collision = IsCollision(sphere, plane);
		uint32_t sphereColor = collision ? RED : WHITE;

		DrawGrid(viewProjectionMatrix, viewportMatrix);
		DrawPlane(plane, viewProjectionMatrix, viewportMatrix, WHITE);       
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, sphereColor); 

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
