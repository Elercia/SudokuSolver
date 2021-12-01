outputDirSementic = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}"

rootPath = "%{wks.location}/../../../"
sourcePath = rootPath .. "Sources/"
projectsFilesLocation = "./projects/" .. _ACTION

availablePlatforms={}
if os.istarget("linux") then
	table.insert(availablePlatforms, "Linux64")
elseif os.istarget("windows") then
	table.insert(availablePlatforms, "Win64")
end

workspace "Sudokuuu"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}

	platforms 
	{
		availablePlatforms
	}

	flags
	{
		"MultiProcessorCompile",
	}

	filter { "platforms:Win64" }
		system "windows"
		architecture "x64"
	filter {}

	filter { "platforms:Linux64" }
		system "linux"
		architecture "x64"
	filter {}

	location(projectsFilesLocation);

	project "Sandbox"
		kind("ConsoleApp")
		language("C++")
		cppdialect("C++20")

		rtti("Off")
		exceptionhandling("Off")
		warnings("Extra")
		flags("NoPCH")
		staticruntime("Off")

		location(projectsFilesLocation)
		targetdir(rootPath .. "/output/bin/" .. outputDirSementic)
		objdir(rootPath .. "/output/obj/" .. outputDirSementic)

		files
		{
			sourcePath .. "/**.hpp",
			sourcePath .. "/**.cpp",
			
		}

		includedirs
		{
			sourcePath
		}

		configuration { "linux", "gmake" }
	  		buildoptions { "`wx-config --cxxflags`", "-ansi", "-pedantic" }

		configuration { "vs2019" }
	  		buildoptions { "" }

		filter "system:windows"
			systemversion "latest"

			linkoptions 
			{
				"/NODEFAULTLIB:library"
			}
			defines
			{
				"_CRT_SECURE_NO_WARNINGS"
			}

		filter "system:Linux"

		filter "configurations:Debug"
			runtime "Debug"
			symbols "on"
			defines {"APP_DEBUG"}

		filter "configurations:Release"
			runtime "Release"
			optimize "on"	
		filter {}

