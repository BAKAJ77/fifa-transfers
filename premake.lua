workspace "fifa-transfers"
    filename "ftfs"
    configurations { "debug", "release" }
    architecture "x86_64"

------------------------------------------------------------------------------------------------------------------------------------------------

project "fifa-transfers"
    filename "ftfs"
    staticruntime "on"
    language "C++"
    cppdialect "C++17"

    targetname "ftfs"
    targetdir "bin/%{cfg.buildcfg}/"
    objdir "objs/%{prj.name}/%{cfg.buildcfg}/"

    includedirs { "src", "libs/asio/asio/include", "libs/irrklang/include", "libs/freetype/include", "libs/glad/include",
        "libs/glfw/include", "libs/glm", "libs/stb", "libs/json/include" }

    files { "src/**.h", "src/**.cpp", "src/**.c", "src/**.tpp" }

    links { "glfw3", "freetype", "irrKlang" }

    -- Project platform define macro based on identified system
    filter "system:windows"
        defines "_PLATFORM_WINDOWS"

    filter "system:macosx"
        defines "_PLATFORM_MACOSX"

    -- Project settings with values unique to the Debug/Release configurations
    filter "configurations:debug"
        kind "ConsoleApp"
        
        libdirs { "libs/irrklang/bin", "libs/freetype/bin/debug", "libs/glfw/bin/debug" }
        defines { "_DEBUG" }
        symbols "On"

    filter "configurations:release"
        kind "WindowedApp"
        entrypoint "mainCRTStartup"

        libdirs { "libs/irrklang/bin", "libs/freetype/bin/release", "libs/glfw/bin/release" }
        defines { "NDEBUG" }
        optimize "Speed"

    -- Copy required DLL lib files into game executable directory when building
    filter { "system:windows", "configurations:debug" }
        postbuildcommands { "copy ..\\libs\\irrklang\\bin\\irrKlang.dll ..\\bin\\build\\irrKlang.dll",
            "copy ..\\libs\\irrklang\\bin\\ikpMP3.dll ..\\bin\\build\\ikpMP3.dll" }

    filter { "system:windows", "configurations:release" }
        postbuildcommands { "copy ..\\libs\\irrklang\\bin\\irrKlang.dll ..\\bin\\build\\irrKlang.dll",
            "copy ..\\libs\\irrklang\\bin\\ikpMP3.dll ..\\bin\\build\\ikpMP3.dll" }

------------------------------------------------------------------------------------------------------------------------------------------------
