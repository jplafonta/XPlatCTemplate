@echo off
echo ========= Copying non-templated files from generated SDK back to template repository =========

set mypath=%~dp0
set destpath=%mypath%source\
set srcpath=%mypath%..\..\..\sdks\XPlatCSdk\

xcopy %srcpath% %destpath% /u /s /e /y
