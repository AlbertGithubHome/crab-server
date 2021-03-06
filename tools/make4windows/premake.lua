solution "encoding"
  configurations { "Debug", "Release" } 
  location "../../code"

  includedirs {
    "../../code/share",
    "../../code/share/detection",
    "../../code/share/utilities",
    "../../code/detection",
    "../../code/utilities",
  }

  defines{"_DEBUG","_SERVER","WIN32","_WINDOWS","_LIB","MY_CMD","_STANDALONE","_OPENALLFUNCTION","NO_DIRECT_X","_USE_32BIT_TIME_T"}
  language "C++"

  project "main"
    location "../../code/main"
    kind "ConsoleApp"
    files { "../../code/main/**.h", "../../code/main/**.cpp"}
    links {"detection","utilities"}
    libdirs{"../../code/share/library/debug"}
    targetdir "../../bin"
    debugdir "../../bin"
      configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols" }
      configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }

  project "detection"
    location "../../code/detection"
    kind "StaticLib"
    files { "../../code/detection/**.h", "../../code/detection/**.cpp"}
    targetdir "../../code/share/library/debug"
    targetname "detection"
      configuration "Debug"
        flags { "Symbols","FatalWarnings" }
      configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize", "FatalWarnings"}

  project "utilities"
    location "../../code/utilities"
    kind "StaticLib"
    files { "../../code/utilities/**.h", "../../code/utilities/**.cpp"}
    targetdir "../../code/share/library/debug"
    targetname "utilities"
      configuration "Debug"
        flags { "Symbols","FatalWarnings" }
      configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize", "FatalWarnings"}

  project "share"
    location "../../code/share"
    kind "StaticLib"
    files { "../../code/share/**.h"}
    targetdir "../../code/share/library/debug"
    targetname "share"
      configuration "Debug"
        flags { "Symbols" }
      configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }

