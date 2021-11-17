#!/bin/bash

#echo ========= Setting Git Submodules to specific commits (BEGIN) =========
if [ ".git" ]
then
    git submodule update --init --recursive
else
    git init
    git submodule add https://github.com/microsoft/libHttpClient.git ./external/libHttpClient
    if [ $? != 0 ]
    then
        exit $?
    fi
    git submodule add https://github.com/Tencent/rapidjson.git ./external/rapidjson
    if [ $? != 0 ]
    then
        exit $?
    fi
    git submodule add https://github.com/microsoft/DirectXTK12 ./external/DirectXTK12
    if [ $? != 0 ]
    then
        exit $?
    fi
fi

pushd "external/rapidjson"
git checkout f54b0e47a08782a6131cc3d60f94d038fa6e0a51
git reset --hard f54b0e47a08782a6131cc3d60f94d038fa6e0a51
if [ $? != 0 ]
then
    exit $?
fi
popd

pushd "external/libHttpClient"
git checkout 6d65866efd8314a05fd72116120860f63db042ba
git reset --hard 6d65866efd8314a05fd72116120860f63db042ba
if [ $? != 0 ]
then
    exit $?
fi
popd

pushd "external/DirectXTK12"
git checkout 99e1d88e49dcdb07bdc2c4122a6b60784f927033
git reset --hard 99e1d88e49dcdb07bdc2c4122a6b60784f927033
if [ $? != 0 ]
then
    exit $?
fi
popd

#echo ========= Setting Git Submodules to specific commits (END) =========