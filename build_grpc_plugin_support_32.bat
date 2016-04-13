setlocal

@rem enter this directory
@rem cd /d %~dp0

@rem enter this directory
PUSHD "../extern/grpc/vsprojects"

@rem Set VS variables (uses Visual Studio 2013)
@call "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" x86

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
