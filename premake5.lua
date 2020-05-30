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
	ignoredefaultlibraries { "MSVCRT", "LIBCMTD" }
    location    "GLRManager"
    dependson   { "Libraries" }
    kind        "WindowedApp"
    language    "C++"
	debugdir	"Main"

    includedirs {
		"Libraries/OpenGL/GLFW/include",
		"Libraries/OpenGL/GL/include",
		"Libraries/cUrl/include",
		"Libraries/Freetype/include",
		"Libraries/simdjson",
    }
	
	libdirs {
		"Libraries/OpenGL/GLFW/lib-vc2019",
		"Libraries/cUrl/lib",
		"Libraries/Freetype/lib",
	}

    files {
        "Main/**.cpp",
		"Main/**.h",
		"Main/Tools/**",
    }

	filter "configurations:Debug"
        defines         { "_DEBUG", "CURL_STATICLIB", "_CRT_SECURE_NO_WARNINGS" }
        symbols         "on"
		links {
			"Libraries",
			"opengl32",
			"glfw3",
			"Normaliz",
			"Ws2_32",
			"Wldap32",
			"Crypt32",
			"advapi32",
			"libcurl_a_debug",
			"freetype",
		}
		
    filter "configurations:Release"
        defines         { "NDEBUG", "CURL_STATICLIB", "_CRT_SECURE_NO_WARNINGS" }
		symbols         "on"
		links {
			"Libraries",
			"opengl32",
			"glfw3",
			"Normaliz",
			"Ws2_32",
			"Wldap32",
			"Crypt32",
			"advapi32",
			"libcurl_a",
			"freetype",
		}
------------------------------------------------ Libraries Project
project "Libraries"
    location    "GLRManager/Libraries"
    kind        "StaticLib"
	language    "C++"
	
    includedirs {
		"Libraries/OpenGL/GLFW/include",
		"Libraries/OpenGL/GL/include",
		"Libraries/cUrl/include",
		"Libraries/Freetype/include",
		"Libraries/simdjson",
    }
	
	libdirs {
		"Libraries/OpenGL/GLFW/lib-vc2019",
		"Libraries/cUrl/lib",
		"Libraries/Freetype/lib",
	}

    files {
        "Libraries/OpenGL/**",
        "Libraries/IMGui/**",
		"Libraries/cUrl/**",
		"Libraries/Freetype/**",
		"Libraries/simdjson/**",
    }