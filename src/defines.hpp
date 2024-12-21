#ifndef VKP_DEFINES
#define VKP_DEFINES

// Core Defines

//----TYPES----

typedef unsigned char u8;

typedef unsigned short u16;

typedef unsigned int u32;

typedef unsigned long long u64;

// Signed int types.

typedef signed char i8;

typedef signed short i16;

typedef signed int i32;

typedef signed long long i64;

// Floating point types

typedef float f32;

typedef double f64;

//----HELPFUL----

// Thanks TheCherno!
#define BIT(x) (1 << x)

#define VKE_EXPAND_MACRO(x) x
#define VKE_STRINGIFY_MACRO(x) #x

//----SYSTEM INFO----
#if defined(_WIN32) || defined(_WIN64)
#define VKP_WINDOWS
#define VKP_PLATFORM "WINDOWS"

#elif defined(__linux__)
#define VKE_LINUX
#define VKE_PLATFORM "LINUX"

#elif defined(__APPLE__) && defined(__MACH__)
#define VKE_MACOS
#define VKE_PLATFORM "MACOS"

#else
#error UNKNOWN_PLATFORM
#endif
#endif
