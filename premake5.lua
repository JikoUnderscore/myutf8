workspace "test_utf8"
	architecture "x64"

    configurations { 
        "Debug", 
        "Release" 
    }

    startproject "test_utf8"


project "test_utf8"
    location "test_utf8"
    kind "ConsoleApp" 
    -- defines { "_CONSOLE" }
    -- kind "WindowedApp"
    language "C++"
    warnings "Everything"

    targetdir ("%{wks.location}/target/dis.%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}")
    objdir ("%{wks.location}/target/build.%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}")

    files {
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.ixx",
        "%{prj.name}/src/**.cppm",
        -- "premake5.lua",
		-- "%{wks.location}/target/**.*"
    }

    includedirs {
        
    }



    filter { "system:windows" }
        buildoptions "/utf-8 /interface /w14242 /w14254 /w14263 /w14265 /w14287 /we4289 /w14296 /w14311 /w14545 /w14546 /w14547 /w14549 /w14555 /w14619 /w14640 /w14826 /w14905 /w14906 /w14928"
        disablewarnings {
            "4464" --  warning C4464: relative include path contains '..'
        }
        cppdialect "C++latest"
        staticruntime "On"
        systemversion "latest"
		conformancemode "Yes"
        externalanglebrackets "On"


        filter { "system:windows", "configurations:Debug" }
            runtime "Debug"
            symbols "On"
            
            -- editandcontinue "Off" -- for /Zi
            -- buildoptions "/fsanitize=address"
            
            links { 
                -- "SDL2-staticd", 
                -- "SDL2_image-staticd", 
                -- "SDL2maind", ''
                -- "SDL2d", 
                -- "SDL2_imaged",
                -- "d3d11.lib",
                -- "Setupapi", "winmm", "imm32", "version",
                -- "LIBCMTD.lib",
                --  "fmtd" 
            }
            libdirs { 
                -- "$(VCPKG_ROOT)/installed/x64-windows-static/debug/lib/manual-link", "$(VCPKG_ROOT)/installed/x64-windows-static/debug/lib",
                -- "$(VCPKG_ROOT)/installed/x64-windows/debug/lib/manual-link", "$(VCPKG_ROOT)/installed/x64-windows/debug/lib"
            }



        filter { "system:windows", "configurations:Release" }
			exceptionhandling ("Off")
            runtime "Release"
			flags { "LinkTimeOptimization" }
            floatingpoint "Fast"
            optimize "Speed"

            links { 
                -- "SDL2-static", 
                -- "SDL2_image-static", 
                -- "SDL2main", 
                -- "SDL2", 
                -- "SDL2_image", 
                -- "d3d11.lib",
                -- "Setupapi", "winmm", "imm32", "version", 
                -- "fmt"
            }
            libdirs { 
                -- "$(VCPKG_ROOT)/installed/x64-windows-static/lib/manual-link", "$(VCPKG_ROOT)/installed/x64-windows-static/lib",
                -- "$(VCPKG_ROOT)/installed/x64-windows/lib/manual-link", "$(VCPKG_ROOT)/installed/x64-windows/lib" 
            }

