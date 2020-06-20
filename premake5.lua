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
	cppdialect 	"C++17"
	debugdir	"Main"

    includedirs {
		"Libraries/OpenGL/GLFW/include",
		"Libraries/OpenGL/GL/include",
		"Libraries/cUrl/include",
		"Libraries/Freetype/include",
		"Libraries/simdjson",
		"Libraries/UltralightHeadless/include",
    }
	
	libdirs {
		"Libraries/OpenGL/GLFW/lib-vc2019",
		"Libraries/cUrl/lib",
		"Libraries/Freetype/lib",
		"Libraries/UltralightHeadless/lib",
	}

    files {
        "Main/**.cpp",
		"Main/**.h",
		"Main/Tools/**",
    }

	filter "configurations:Debug"
		ignoredefaultlibraries { "MSVCRT", "LIBCMTD" }
        defines         { "_DEBUG", "CURL_STATICLIB", "_CRT_SECURE_NO_WARNINGS",}
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
			"Ultralight",
			"UltralightCore",
			"WebCore",
			"AppCore",
		}
		
    filter "configurations:Release"
		ignoredefaultlibraries { "LIBCMT" }
        defines         { "NDEBUG", "CURL_STATICLIB", "_CRT_SECURE_NO_WARNINGS",}
		runtime "release"
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
			"Ultralight",
			"UltralightCore",
			"WebCore",
			"AppCore",
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
		"Libraries/UltralightHeadless/include",
    }
	
	libdirs {
		"Libraries/OpenGL/GLFW/lib-vc2019",
		"Libraries/cUrl/lib",
		"Libraries/Freetype/lib",
		"Libraries/UltralightHeadless/lib",
	}

    files {
        "Libraries/OpenGL/**",
        "Libraries/IMGui/**",
		"Libraries/cUrl/**",
		"Libraries/Freetype/**",
		"Libraries/simdjson/**",
		"Libraries/UltralightHeadless/**",
    }
	
	filter "configurations:Debug"
		runtime "debug"