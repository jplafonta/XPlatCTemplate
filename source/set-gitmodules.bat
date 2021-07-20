@echo off
echo ========= Setting Git Submodules to specific commits (BEGIN) =========
if exist ".git" (
    git submodule update --init --recursive
) else (
    git init
    git submodule add https://github.com/Tencent/rapidjson.git ./external/rapidjson
    git submodule add https://github.com/microsoft/libHttpClient.git ./external/libHttpClient
    git submodule add https://github.com/microsoft/DirectXTK12 ./external/DirectXTK12
)

pushd "external/rapidjson"
git checkout f54b0e47a08782a6131cc3d60f94d038fa6e0a51
git reset --hard f54b0e47a08782a6131cc3d60f94d038fa6e0a51
popd


pushd "external/libHttpClient"
git checkout 0b86f1e2b01ebe3f6e8e60ed6d7a41da3645b5bf
git reset --hard 0b86f1e2b01ebe3f6e8e60ed6d7a41da3645b5bf
popd

pushd "external/DirectXTK12"
git checkout 99e1d88e49dcdb07bdc2c4122a6b60784f927033
git reset --hard 99e1d88e49dcdb07bdc2c4122a6b60784f927033
popd

echo ========= Setting Git Submodules to specific commits (END) =========