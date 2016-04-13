setlocal

@rem enter this directory
@rem cd /d %~dp0

pushd "../extern/grpc/third_party/protobuf/cmake"

@rem Set VS variables (uses Visual Studio 2013)
@call "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" x86

@rem Build third_party/protobuf

cmake -G "Visual Studio 14 2015" -Dprotobuf_BUILD_TESTS=OFF

POPD

pause

@rem enter this directory
PUSHD "../extern/grpc/vsprojects"

@rem Build third_party/protobuf
msbuild ..\third_party\protobuf\cmake\protobuf.sln /p:Configuration=Release || goto :error

pause

@rem Build the C++ protoc plugins
msbuild grpc_protoc_plugins.sln /t:grpc_plugin_support /p:Configuration=Release /p:Platform="Win32" /p:BuildProjectReferences=false || goto :error


POPD
endlocal

pause
goto :EOF

:error
echo Failed!
pause
exit /b %errorlevel%
