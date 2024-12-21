#ifndef GBP_COREH
#define GBP_COREH
#include <algorithm>
#include <filesystem>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <set>
#include <stack>
#include <typeindex>
#include <utility>

#include <optional>

#include <array>
#include <fstream>
#include <map>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <defines.hpp>

using Hash = u64;

namespace GBP {

template <typename T, typename Y>
using UMap = std::unordered_map<T, Y>;

template <typename T, typename Y>
using USet = std::unordered_set<T, Y>;

template <typename T>
using Stack = std::stack<T>;

// Thanks TheCherno | Very Helpful
template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
}

#include <Log/log.hpp>

//--ASSERTIONS-- Again. Thanks TheCherno

#if defined(GBP_WINDOWS)
#define GBP_DEBUGBREAK() __debugbreak()
#elif defined(GBP_LINUX)
#include <signal.h>
#define GBP_DEBUGBREAK() raise(SIGTRAP)
#endif

#define GBP_INTERNAL_ASSERT_IMPL(type, check, msg, ...) \
    {                                                   \
        if (!(check)) {                                 \
            GBP##type##ERROR(msg, __VA_ARGS__);         \
            GBP_DEBUGBREAK();                           \
        }                                               \
    }
#define GBP_INTERNAL_ASSERT_WITH_MSG(type, check, ...) GBP_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define GBP_INTERNAL_ASSERT_NO_MSG(type, check) GBP_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", GBP_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)
#define GBP_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define GBP_INTERNAL_ASSERT_GET_MACRO(...) GBP_EXPAND_MACRO(GBP_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, GBP_INTERNAL_ASSERT_WITH_MSG, GBP_INTERNAL_ASSERT_NO_MSG))

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define GBP_ASSERT(...) GBP_EXPAND_MACRO(GBP_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define GBP_CORE_ASSERT(...) GBP_EXPAND_MACRO(GBP_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))

#endif
