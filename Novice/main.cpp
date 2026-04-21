#include <Novice.h>
#include <math.h>
#include <cassert>

const char kWindowTitle[] = "GC2A_02_テイン_タイ_アウン";

struct Matrix4x4 {
	float m[4][4];
};

Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
}

Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 0;
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 m = {};
	for (int i = 0; i < 4; i++) {
		m.m[i][i] = 1.0f;
	}
	return m;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result = MakeIdentity4x4();
	Matrix4x4 temp = m;

	for (int i = 0; i < 4; i++) {
		assert(temp.m[i][i] != 0.0f);

		float pivot = temp.m[i][i];

		for (int j = 0; j < 4; j++) {
			temp.m[i][j] /= pivot;
			result.m[i][j] /= pivot;
		}

		for (int k = 0; k < 4; k++) {
			if (k == i)
				continue;

			float factor = temp.m[k][i];

			for (int j = 0; j < 4; j++) {
				temp.m[k][j] -= factor * temp.m[i][j];
				result.m[k][j] -= factor * result.m[i][j];
			}
		}
	}
	return result;
}

Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
}

static const int kColumnWidth = 60;
static const int kRowHeight = 20;
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y - 20, label);

	for (int row = 0; row < 4; row++) {
		for (int column = 0; column < 4; column++) {
			Novice::ScreenPrintf(x + column * kColumnWidth, y + row * kRowHeight, "%6.02f", matrix.m[row][column]);
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

	Matrix4x4 m1 = {{
		{3.2f, 0.7f, 9.6f, 4.4f},
		{5.5f, 1.3f, 7.8f, 2.1f}, 
		{6.9f, 8.0f, 2.6f, 1.0f}, 
		{0.5f, 7.2f, 5.1f, 3.3f}
	}};

	Matrix4x4 m2 = {{
		{4.1f, 6.5f, 3.3f, 2.2f},
		{8.8f, 0.6f, 9.9f, 7.7f}, 
		{1.1f, 5.5f, 6.6f, 0.0f}, 
		{3.3f, 9.9f, 8.8f, 2.2f}
	}};

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

		Matrix4x4 resultAdd = Add(m1, m2);
		Matrix4x4 resultMultiply = Multiply(m1, m2);
		Matrix4x4 resultSubtract = Subtract(m1, m2);
		Matrix4x4 inverseM1 = Inverse(m1);
		Matrix4x4 inverseM2 = Inverse(m2);
		Matrix4x4 transposeM1 = Transpose(m1);
		Matrix4x4 transposeM2 = Transpose(m2);
		Matrix4x4 identity = MakeIdentity4x4();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(0, 0, resultAdd, "Add");
		MatrixScreenPrintf(0, kRowHeight*5, resultSubtract, "Subtract");
		MatrixScreenPrintf(0, kRowHeight * 5*2, resultMultiply, "Multiply");
		MatrixScreenPrintf(0, kRowHeight * 5*3, inverseM1, "inverseM1");
		MatrixScreenPrintf(0, kRowHeight * 5*4, inverseM2, "inverseM2");
		MatrixScreenPrintf(kColumnWidth*5, 0, transposeM1, "transposeM1");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5, transposeM2, "transposeM2");
		MatrixScreenPrintf(kColumnWidth * 5, kRowHeight * 5 * 2,identity, "identity");

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
