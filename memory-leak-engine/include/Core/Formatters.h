#pragma once

// vec2
template<>
struct fmt::formatter<glm::vec2> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(glm::vec2 const& vector, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "({0}, {1})", vector.x, vector.y);
    }
};

// vec3

template<>
struct fmt::formatter<glm::vec3> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    };

    template<typename FormatContext>
    auto format(glm::vec3 const& vector, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "({0}, {1}, {2})", vector.x, vector.y, vector.z);
    };
};

//vec4

template<>
struct fmt::formatter<glm::vec4> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(glm::vec4 const& vector, FormatContext& ctx) {
        return fmt::format_to(
                ctx.out(),
                "({0}, {1}, {2}, {3})",
                vector.x, vector.y, vector.z, vector.w);
    }
};