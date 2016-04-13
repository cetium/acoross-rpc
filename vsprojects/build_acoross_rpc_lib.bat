setlocal

@rem Set VS variables (uses Visual Studio 2013)
@call "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" x86

nuget restore rpc.sln

@rem Build acoross rpc lib
msbuild rpc.sln /m /p:Configuration=Release /p:Platform="Win32" /p:BuildProjectReferences=false || goto :error
msbuild rpc.sln /m /p:Configuration=Release /p:Platform="x64" /p:BuildProjectReferences=false || goto :error
msbuild rpc.sln /m /p:Configuration=Debug /p:Platform="Win32" /p:BuildProjectReferences=false || goto :error
msbuild rpc.sln /m /p:Configuration=Debug /p:Platform="x64" /p:BuildProjectReferences=false || goto :error

endlocal

pause
goto :EOF

:error
echo Failed!
pause
exit /b %errorlevel%
