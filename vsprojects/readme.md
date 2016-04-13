1. build protoc plugin

run _build_grpc_plugin_support_32.bat
run _build_protobuf_lib_32.bat
run build_protobuf_plugin.bat

2. build acoross-rpc-lib

run _build_protobuf_lib_32.bat
run _build_protobuf_lib_64.bat
run build_acoross_rpc_lib.bat