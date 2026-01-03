set_project("GameEngine")
set_version("0.1.0")

add_rules("mode.debug", "mode.release")
set_languages("c++23")

add_requires("glfw 3.4", {alias = "glfw"})
add_requires("glad 0.1.36", {alias = "glad"})
add_requires("glm 0.9.9", {alias = "glm"})
add_requires("conan::spdlog/1.14.1", {alias = "spdlog"})
add_requires("conan::catch2/3.6.0", {alias = "catch2"})
add_requires("entt 3.13.1", {alias = "entt"})
add_requires("taskflow", {alias = "taskflow"})
add_requires("tracy", {alias = "tracy"})
add_requires("imgui", {alias = "imgui", configs = {opengl3 = true}})
add_requires("wgpu-native", {alias = "wgpu"})
add_requires("stb", {alias = "stb"})

option("enable_profiling")
    set_default(true)
    set_showmenu(true)
    set_description("Enable Tracy profiling")
option_end()

option("use_webgpu")
    set_default(true)
    set_showmenu(true)
    set_description("Use WebGPU instead of OpenGL")
option_end()

target("game_engine")
    set_kind("static")
    add_files("src/**.cpp")
    add_headerfiles("include/(game_engine/**.hpp)")
    add_includedirs("include", {public = true})
    add_packages("glfw", "glad", "glm", "spdlog", "entt", "taskflow", "imgui", "wgpu", "stb")
    add_includedirs("/opt/homebrew/include", {public = true})
    add_linkdirs("/opt/homebrew/lib", {public = true})
    add_links("SDL3", {public = true})
    -- GLM configuration for WebGPU (Z clip space 0 to 1, left-handed)
    add_defines("GLM_FORCE_DEPTH_ZERO_TO_ONE", {public = true})
    add_defines("GLM_FORCE_LEFT_HANDED", {public = true})
    if has_config("enable_profiling") then
        add_packages("tracy")
        add_defines("TRACY_ENABLE")
    end
    if has_config("use_webgpu") then
        add_defines("USE_WEBGPU")
    end
    add_rules("plugin.cmake.autoreload")


target("triangle_example")
    set_kind("binary")
    add_deps("game_engine")
    add_files("examples/triangle/main.cpp")
    add_packages("glfw", "glad", "glm", "spdlog", "entt", "taskflow", "imgui", "wgpu")
    add_links("SDL3")
    add_linkdirs("/opt/homebrew/lib")
    add_rpathdirs("/opt/homebrew/lib")
    if has_config("enable_profiling") then
        add_packages("tracy")
        add_defines("TRACY_ENABLE")
    end
    if has_config("use_webgpu") then
        add_defines("USE_WEBGPU")
    end

target("glass_sphere_example")
    set_kind("binary")
    add_deps("game_engine")
    add_files("examples/glass_sphere/main.cpp")
    add_packages("glfw", "glad", "glm", "spdlog", "entt", "taskflow", "imgui", "wgpu", "stb")
    add_links("SDL3")
    add_linkdirs("/opt/homebrew/lib")
    add_rpathdirs("/opt/homebrew/lib")
    if has_config("enable_profiling") then
        add_packages("tracy")
        add_defines("TRACY_ENABLE")
    end
    if has_config("use_webgpu") then
        add_defines("USE_WEBGPU")
    end

target("playground_example")
    set_kind("binary")
    set_rundir("$(projectdir)")
    add_deps("game_engine")
    add_files("examples/playground/main.cpp")
    add_packages("glfw", "glad", "glm", "spdlog", "entt", "taskflow", "imgui", "wgpu", "stb")
    add_links("SDL3")
    add_linkdirs("/opt/homebrew/lib")
    add_rpathdirs("/opt/homebrew/lib")
    if has_config("enable_profiling") then
        add_packages("tracy")
        add_defines("TRACY_ENABLE")
    end
    if has_config("use_webgpu") then
        add_defines("USE_WEBGPU")
    end

target("tests")
    set_kind("binary")
    add_deps("game_engine")
    add_files("tests/**.cpp")
    add_includedirs("tests", "include")
    add_packages("catch2", "glfw", "glad", "glm", "spdlog")

on_load(function (target)
    local main_content = [[
            #define CATCH_CONFIG_MAIN
            #include <catch2/catch_all.hpp>
        ]]
    target:add("files", target:autogenfile("tests/main.cpp", main_content))
end)

if is_mode("debug") then
    add_cxflags("-Wall", "-Wextra", "-Werror", "-Wpedantic")
end

rule("plugin.cmake.autoreload")
set_kind("project")
after_build(function (_)
    import("core.project.config")
    import("core.project.depend")
    import("core.project.project")
    import("core.base.option")

    local verbose = option.get("verbose")

    if not os.exec("cmake --version") then
        if verbose then
            print("CMake not found. Skipping CMake reload.")
        end
        return
    end

    if not os.isfile("CMakeLists.txt") then
        if verbose then
            print("CMakeLists.txt not found. Skipping CMake reload.")
        end
        return
    end

    local dependfile = path.join(config.buildir(), ".gens", "rules", "plugin.cmake.autoreload.d")
    depend.on_changed(function ()
        print("Changes detected. Reloading CMakeFiles...")
        local result = os.exec("cmake .")
        if result == 0 then
            print("CMakeFiles successfully reloaded after changes")
        else
            print("Error occurred while reloading CMakeFiles")
        end
    end, {dependfile = dependfile, files = project.allfiles()})
end)