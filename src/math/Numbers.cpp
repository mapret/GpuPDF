module;

#include <numbers>

export module gpupdf.math:Numbers;

export namespace numbers
{
constexpr float PI{ std::numbers::pi_v<float> };
}
