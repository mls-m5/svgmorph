#include <cmath>
#include <functional>
#include <numbers>
#include <string_view>
// block 1
enum class EaseType {
    Linear,
    Sine,
    Exp,
    Elastic,
    Bouncy,
};

constexpr auto pi = std::numbers::pi_v<float>;
// end
// block 2

inline float easeLinear(float t) {
    return t;
}
// end
// block 6

inline float easeExp(float t) {
    static const auto multiplier = 2.f;
    static const auto min = std::exp(-1 * multiplier);
    static const auto rangeInv = 1.f / (1.f - min);
    auto ret = (std::exp(-t * multiplier) - min) * rangeInv;
    return (1.f - ret);
}
// end
// block 7

inline float easeSine(float t) {
    return .5f - std::cos(pi * t) * .5f;
}

inline float easeElastic(float t) {
    return (1.f - std::cos(pi * 5.f * t) * (1.f - easeExp(t)));
}

inline float easeBouncy(float t) {
    return 1.f - std::abs(1.f - easeElastic(t));
}
// end
// block 5

inline std::function<float(float)> ease(EaseType type) {
    // block a 8
    using T = EaseType;
    switch (type) {
    case T::Linear:
        return easeLinear;
    case T::Exp:
        return easeExp;
    case T::Sine:
        return easeSine;
    case T::Elastic:
        return easeElastic;
    case T::Bouncy:
        return easeBouncy;
    }
    // end a
    return easeLinear;
}
// end
// block 4

inline EaseType stringToEaseType(std::string_view str) {
    // block b 9
    using T = EaseType;
    if (str == "sine" || str == "sin") {
        return T::Sine;
    }
    if (str == "elastic") {
        return T::Elastic;
    }
    if (str == "exp") {
        return T::Exp;
    }
    if (str == "bouncy" || str == "bounce") {
        return T::Bouncy;
    }
    // end b
    return T::Linear;
}

inline std::string_view typeToStr(EaseType type) {
    // block c 10
    using T = EaseType;
    switch (type) {
    case T::Linear:
        return "linear";
    case T::Exp:
        return "exp";
    case T::Sine:
        return "sine";
    case T::Elastic:
        return "elastic";
    case T::Bouncy:
        return "bouncy";
    }
    // end c 10
    return "linear";
}
// end
// block 3

inline std::function<float(float)> ease(std::string_view name) {
    auto type = stringToEaseType(name);
    return ease(type);
}
// end
