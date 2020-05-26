------------------------------------------------ Solution
workspace "GLRManager"
    configurations  { "Debug", "Release" }
    location        "GLRManager"
    startproject    "GLRManager"

    filter "system:windows"
        platforms       { "x64" }
        characterset    "MBCS"

------------------------------------------------ GLRManager Project 
project "GLRManager"
    location    "GLRManager"
    dependson   { "Libraries" }
    kind        "WindowedApp"
    language    "C++"
	debugdir	"Main"

    includedirs {
		"Libraries/OpenGL/GLFW/include",
		"Libraries/OpenGL/GL/include",
    }
	
	libdirs {
		"Libraries/OpenGL/GLFW/lib-vc2019",
	}

    files {
        "Main/**.cpp",
		"Main/**.h",
		"Main/Tools/**",
		"Main/Config/**",
    }

    links {
		"Libraries",
        "opengl32",
		"glfw3",
    }
------------------------------------------------ Libraries Project
project "Libraries"
    location    "GLRManager/Libraries"
    kind        "StaticLib"
	language    "C++"
	
    includedirs {
		"Libraries/OpenGL/GLFW/include",
		"Libraries/OpenGL/GL/include",
    }
	
	libdirs {
		"Libraries/OpenGL/GLFW/lib-vc2019",
	}

    files {
        "Libraries/OpenGL/**",
        "Libraries/IMGui/**",
        "Libraries/cJSON/cJSON.c",
        "Libraries/cJSON/cJSON.h",
    }