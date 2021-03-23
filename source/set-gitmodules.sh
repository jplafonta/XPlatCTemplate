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
git checkout 0b86f1e2b01ebe3f6e8e60ed6d7a41da3645b5bf
git reset --hard 0b86f1e2b01ebe3f6e8e60ed6d7a41da3645b5bf
if [ $? != 0 ]
then
    exit $?
fi
popd

#echo ========= Setting Git Submodules to specific commits (END) =========