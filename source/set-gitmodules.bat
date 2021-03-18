@echo off
echo ========= Setting Git Submodules to specific commits (BEGIN) =========
if exist ".git" (
    git submodule update --init --recursive
) else (
    git init
    git submodule add https://github.com/open-source-parsers/jsoncpp.git ./external/jsoncpp
    if /I "%ERRORLEVEL%" gtr "1" (
        exit /B %ERRORLEVEL%
    )
	git submodule add https://github.com/microsoft/libHttpClient.git ./external/libHttpClient
	if /I "%ERRORLEVEL%" gtr "1" (
        exit /B %ERRORLEVEL%
    )
)

pushd "external/jsoncpp"
git checkout 2baad4923e6d9a7e09982cfa4b1c5fd0b67ebd87
git reset --hard 2baad4923e6d9a7e09982cfa4b1c5fd0b67ebd87
if /I "%ERRORLEVEL%" neq "0" (
exit /B %ERRORLEVEL%
)
popd

pushd "external/libHttpClient"
git checkout 0b86f1e2b01ebe3f6e8e60ed6d7a41da3645b5bf
git reset --hard 0b86f1e2b01ebe3f6e8e60ed6d7a41da3645b5bf
if /I "%ERRORLEVEL%" neq "0" (
exit /B %ERRORLEVEL%
)
popd

echo ========= Setting Git Submodules to specific commits (END) =========