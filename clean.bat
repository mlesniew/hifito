@echo off
del /F HiFiTo.sdf
rd /S /Q Debug Release ipch
rd /S /Q HiFiTo\Debug HiFiTo\Release
del Hifito\*.orig