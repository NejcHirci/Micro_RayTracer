-- premake5.lua
workspace "RayTracerApp"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "RayTracerApp"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "WalnutExternal.lua"
include "RayTracerApp"