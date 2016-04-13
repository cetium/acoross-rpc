setlocal

@rem enter this directory
@rem cd /d %~dp0

SET cur=%~dp0

@rem enter this directory
PUSHD "../extern/grpc/third_party/protobuf/cmake/"

@rem Set VS variables (uses Visual Studio 2015)
@call "%VS140COMNTOOLS%\..\..\vc\vcvarsall.bat" x64

	mkdir build64 & cd build64

		@rem cmake debug
		mkdir debug & cd debug
		cmake -G "NMake Makefiles" ^
		 -DCMAKE_BUILD_TYPE=Debug ^
		 -DCMAKE_INSTALL_PREFIX=%cur%build\x64\ ^
		 -Dprotobuf_BUILD_TESTS=OFF ^
		 ../..

		nmake install

	cd ..

		@rem cmake release
		mkdir release & cd release
		cmake -G "NMake Makefiles" ^
		 -DCMAKE_BUILD_TYPE=Release ^
		 -DCMAKE_INSTALL_PREFIX=%cur%build\x64\ ^
		 -Dprotobuf_BUILD_TESTS=OFF ^
		 ../..

		nmake install

POPD

endlocal

pause
goto :EOF

:error
echo Failed!
pause
exit /b %errorlevel%
