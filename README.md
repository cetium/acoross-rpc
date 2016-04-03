# acoross-rpc
C++ rpc using asio and protobuf

build on VS2015

1. build grpc-plugin-support
* Open extern/grpc/vcprojects/grpc_protoc_plugins.sln
* build grpc_plugin_support project

2. build protoc.lib
* extern/protobuf
* see: extern/protobuf/cmake/README.md
* build protoc.lib

3. place static library files on /extern/lib/$(Platform)/$(Configuration)

4. build
* vsprojects/protobuf_plugin.sln
 * plugin for protoc.exe
 * output: acoross_cpp_plugin.exe
 * usage: Use acoross_cpp_plugin with protoc.exe (protobuf compiler)
protoc.exe -I ./ --grpc_out=./ --plugin=protoc-gen-grpc="acoross_cpp_plugin.exe" myproto.proto

* vsprojects/rpc.sln
 * acoross_rpc static library
 
