workspace "Second Wind"
    architecture "x64"

    configurations {
        "debug",
        "release"
    }

outputdir = "%{cfg.buildcfg}"

project "Second Wind"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir)
    objdir ("build/" .. outputdir)

    files{
        "src/**.h",
        "src/**.cpp",
        "src/**.c"
    }

    includedirs {
        "include/"
    }

    libdirs{
        "lib/SDL2/"
    }

    links{
        "SDL2",
        "SDL2main"
    }
    filter "configurations:debug"
        symbols "On"
        optimize "Debug"

    filter "configurations:release"
        optimize "On"

project "test"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("test/bin/")
    objdir ("test/build/")

    files{
        "test/**.h",
        "test/**.cpp",
        "src/**.h",
        "src/**.cpp",
        "src/**.c"
    }
    excludes {
        "src/game.cpp"
    }

    includedirs {
        "include/",
        "src/"
    }

    libdirs{
        "lib/SDL2/"
    }

    links{
        "SDL2",
        "SDL2main"
    }

    symbols "On"
    optimize "Debug"