#include <cmath>
#include <functional>
#include <numbers>
#include <string_view>

enum class EaseType {
    Linear,
    Sine,
    Elastic,
    Bouncy,
};

constexpr auto pi = std::numbers::pi_v<float>;

inline float easeLinear(float t) {
    return t;
}

inline float easeSine(float t) {
    return .5f - std::cos(pi * t) * .5f;
}

inline float easeElastic(float t) {
    return (1.f - std::cos(pi * 5.f * t) * (1.f - t));
}

inline float easeBouncy(float t) {
    return 1.f - std::abs(1.f - easeElastic(t));
}

std::function<float(float)> ease(EaseType type) {
    using T = EaseType;
    switch (type) {
    case T::Linear:
        return easeLinear;
    case T::Sine:
        return easeSine;
    case T::Elastic:
        return easeElastic;
    case T::Bouncy:
        return easeBouncy;
    }

    return easeLinear;
}

inline EaseType stringToEaseType(std::string_view str) {
    using T = EaseType;
    if (str == "sine") {
        return T::Sine;
    }
    if (str == "elastic") {
        return T::Elastic;
    }
    if (str == "bouncy" || str == "bounce") {
        return T::Bouncy;
    }
    return T::Linear;
}

std::function<float(float)> ease(std::string_view name) {
    auto type = stringToEaseType(name);
    return ease(type);
}
