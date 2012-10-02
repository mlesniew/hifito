@echo off
del /F HiFiTo.sdf
rd /S /Q Debug Release "Debug Portable" "Release Portable" ipch
rd /S /Q HiFiTo\Debug HiFiTo\Release "HiFiTo\Debug Portable" "HiFiTo\Release Portable" 
del Hifito\*.orig
