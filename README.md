# VKE | C++ BluePrint / C++ BoilerPlate For graphics

Includes CMake files to compile and link to GLFW and GLM libraries for addes ease.

-- Checkout [CPPBP](https://github.com/clackroe/CPPBoilerPlate) --

I like my C++ projects to have similar basic structures. This project provides simple and easy to use boiler plate to get started with C++ projects of any size.

Simple CMakeSetup with easy to use macros for efficient logging with [SPDLOG](https://github.com/gabime/spdlog).
 - ```cpp
   VKE_INFO("Print Numbers: {0} and {1}.", 420, 69);
   ```
 - ```CPP
   VKE_ERROR("Failed to {0} | {1} ", "resize window", error.c_str());
   ```
 - ```CPP
   VKE_WARN("Tried to print null value: {0}", neededValue);
   ```
 - ```CPP
   VKE_ASSERT(result, "Failed to process numbers. {0}", error.c_str());
   ```

