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

function DefaultBuildOptions()
	rtti("Off")
	warnings("Extra")
	flags("NoPCH")
	staticruntime("Off")

	location(projectsFilesLocation)
	targetdir(rootPath .. "/output/bin/" .. outputDirSementic)
	objdir(rootPath .. "/output/obj/" .. outputDirSementic)

	filter {"system:windows"}
		systemversion "latest"

		linkoptions 
		{
			"/NODEFAULTLIB:library"
		}
		defines
		{
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter {"system:Linux"}
	filter {}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"
		optimize "off"

		exceptionhandling("on")

		defines {"APP_DEBUG"}

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
		symbols "off"
		
		exceptionhandling("off")

		defines {"APP_RELEASE"}
	filter {}
end

workspace "Sudokuuu"
	startproject "Tests"

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

	project "Sudoku"
		kind("StaticLib")
		language("C++")
		cppdialect("C++20")

		DefaultBuildOptions()

		files
		{
			sourcePath .. "/sudoku/sudoku.hpp",
			sourcePath .. "/sudoku/sudoku.cpp",
		}

		includedirs
		{
			sourcePath .. "/sudoku/"
		}

	project "Tests"
		kind("ConsoleApp")
		language("C++")
		cppdialect("C++20")

		links{ "sudoku" }

		DefaultBuildOptions()

		files
		{
			sourcePath .. "/catch2/catch2.hpp",
			sourcePath .. "/catch2/catch2.cpp",

			sourcePath .. "/tests/*.hpp",
			sourcePath .. "/tests/*.cpp",
		}

		includedirs
		{
			sourcePath .. "/catch2/",
			sourcePath .. "/tests/",
			sourcePath
		}

		

