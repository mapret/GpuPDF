export module gpupdf.math:Vector;

import :Matrix;

export using Vector2 = Matrix<float, 2, 1>;
export using Vector3 = Matrix<float, 3, 1>;
export using Vector4 = Matrix<float, 4, 1>;

export using Vector2i = Matrix<int, 2, 1>;
export using Vector3i = Matrix<int, 3, 1>;
export using Vector4i = Matrix<int, 4, 1>;

static_assert(sizeof(Vector2) == 8);
static_assert(sizeof(Vector3) == 12);
static_assert(sizeof(Vector4) == 16);
