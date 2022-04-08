fork form https://github.com/cdmeister/Basic-Thrift-Tutorial, remove boost lib.

compile thrift file:

`thrift --gen cpp hello.thrift`

compile server:

`g++ gen-cpp/hello_types.cpp gen-cpp/HelloSvc.cpp gen-cpp/hello_constants.cpp gen-cpp/HelloSvc_server.skeleton.cpp -o server -lthrift`

compile client:

`g++ gen-cpp/hello_types.cpp gen-cpp/HelloSvc.cpp gen-cpp/hello_constants.cpp client.cpp -o client -lthrift`
