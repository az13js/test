# Cephalopod C++

Provides HTTP proxy service `proxy`. The program passed testing on Ubuntu and Debian (I am not sure if it will work in all systems, because there are no unit test scripts, testing is manual at the moment). Compiling this program only requires the `make` and `g++` commands (The GCC compiler needs to support the C++17 standard). Most modern GNU/Linux systems meet these requirements. Inputting the `make` command, like this:

    $ make
    g++  -std=c++17 -c -o proxy.o proxy.cpp
    g++  -std=c++17 -c -o cephalopod_black_hole.o cephalopod_black_hole.cpp
    g++  -std=c++17 -c -o cephalopod_http.o cephalopod_http.cpp
    g++  -std=c++17 -c -o cephalopod_http_tools.o cephalopod_http_tools.cpp
    g++  -std=c++17 -c -o cephalopod_pipe.o cephalopod_pipe.cpp
    g++  -std=c++17 -c -o cephalopod_thread_safe_queue.o cephalopod_thread_safe_queue.cpp
    g++  -std=c++17 -c -o cephalopod_host_to_ip.o cephalopod_host_to_ip.cpp
    g++  -std=c++17 -c -o logic.o logic.cpp
    g++  -std=c++17 -c -o cephalopod_argument.o cephalopod_argument.cpp
    g++  -std=c++17 -c -o trans.o trans.cpp
    g++ -o trans trans.o cephalopod_black_hole.o cephalopod_http.o cephalopod_http_tools.o cephalopod_pipe.o cephalopod_thread_safe_queue.o cephalopod_host_to_ip.o logic.o cephalopod_argument.o -lpthread
    g++ -o proxy proxy.o cephalopod_black_hole.o cephalopod_http.o cephalopod_http_tools.o cephalopod_pipe.o cephalopod_thread_safe_queue.o cephalopod_host_to_ip.o logic.o cephalopod_argument.o -lpthread
    mkdir cache

The `proxy` has caching functionality (this is more of a buffering functionality than it is a caching functionality, as it does not store pages or static resources). So when using the `proxy` service, a `cache` directory needs to be created in the current directory. The `cache` directory is currently not configured, and is fixed at the current directory.

When you run the `proxy` command, you need to specify the address and port that the proxy service should listen on, just like this:

    ./proxy --listen-ip=0.0.0.0 --listen-port=80

You can also use an IPv6 address:

    ./proxy --listen-ip=:: --listen-port=80

If the service is running normally, it will show:

    Ok, --listen-ip="::" --listen-port="80"

Then you can set up the address of the proxy service on another host, and access other websites through the proxy service. Here is a record of me accessing the proxy server through IPv6 on my computer:

    $ export HTTPS_PROXY="http://[fe80::cd76:b87d:cbb1:88bb]:80"
    $ export HTTP_PROXY="http://[fe80::cd76:b87d:cbb1:88bb]:80"
    $ curl -o index.html -v http://linux.org
    * Uses proxy env variable http_proxy == 'http://[fe80::cd76:b87d:cbb1:88bb]:80'
    % Total    % Received % Xferd  Average Speed   Time    Time     Time  Current
                                    Dload  Upload   Total   Spent    Left  Speed
    0     0    0     0    0     0      0      0 --:--:-- --:--:-- --:--:--     0*   Trying [fe80::cd76:b87d:cbb1:88bb]:80...
    * Connected to fe80::cd76:b87d:cbb1:88bb (fe80::cd76:b87d:cbb1:88bb) port 80 (#0)
    > GET http://linux.org/ HTTP/1.1
    > Host: linux.org
    > User-Agent: curl/8.1.2
    > Accept: */*
    > Proxy-Connection: Keep-Alive
    >
    < HTTP/1.1 403 Forbidden
    < Date: Sun, 17 Sep 2023 04:37:26 GMT
    < Content-Type: text/html; charset=UTF-8
    < Content-Length: 4460
    < Connection: close
    ... Omit the output ...

If you want to disable IPv6 support, define a constant during compilation like this:

    $ CXXFLAGS=-DCEPHALOPOD_NO_IPV6_SUPPORT make

The proxy service does not support user name and password authentication. If you deploy your service on a public network, you need security, and you can use the `ssh` command to forward data:

    ssh -N -C -L 127.0.0.1:80:127.0.0.1:80 user@<your proxy service>

If you attempt to configure the proxy service and/or the `ssh` command as a Systemd service, you can refer to the `cephalopod-proxy.service` and `cephalopod-ssh.service`.

That's all there is to say about the proxy service, hope you have a great time.
