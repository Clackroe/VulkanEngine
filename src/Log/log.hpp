#ifndef VKE_LOGH
#define VKE_LOGH

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <memory>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h> //

// #include <fmt/format.h>
#include <glm/glm.hpp>

namespace VKE {

class Log {
public:
    static void Init();

    static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
};

}

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
    return os << glm::to_string(vector);
}
//
template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
    return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
    return os << glm::to_string(quaternion);
}

// Core log macros
#define VKE_TRACE(...) VKE::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define VKE_INFO(...) VKE::Log::GetCoreLogger()->info(__VA_ARGS__);
#define VKE_WARN(...) VKE::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define VKE_ERROR(...) VKE::Log::GetCoreLogger()->error(__VA_ARGS__);
#define VKE_CRITICAL(...) VKE::Log::GetCoreLogger()->critical(__VA_ARGS__);
#endif
