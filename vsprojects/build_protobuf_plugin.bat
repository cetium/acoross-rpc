setlocal

@rem Set VS variables (uses Visual Studio 2013)
@call "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" x86

@rem Build acoross-rpc protoc plugin
msbuild protobuf_plugin.sln /m /p:Configuration=Release /p:Platform="x86" /p:BuildProjectReferences=false || goto :error

copy build\Win32\bin\protoc.exe build\protoc.exe

endlocal

pause
goto :EOF

:error
echo Failed!
pause
exit /b %errorlevel%
