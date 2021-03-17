@echo off
echo ========= Copying non-templated files from generated SDK back to template repository =========

set mypath=%~dp0
set destpath=%mypath%code\
set srcpath=%mypath%..\..\..\..\sdks\XPlatCSdk\code\


