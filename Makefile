all: proxy trans
clean:
	rm -f proxy trans *.o
	rm -rf cache

proxy: proxy.o cephalopod_black_hole.o cephalopod_http.o cephalopod_http_tools.o cephalopod_pipe.o cephalopod_thread_safe_queue.o cephalopod_host_to_ip.o logic.o cephalopod_argument.o
	$(CXX) -o proxy proxy.o cephalopod_black_hole.o cephalopod_http.o cephalopod_http_tools.o cephalopod_pipe.o cephalopod_thread_safe_queue.o cephalopod_host_to_ip.o logic.o cephalopod_argument.o -lpthread
	mkdir -p cache

trans: trans.o cephalopod_black_hole.o cephalopod_http.o cephalopod_http_tools.o cephalopod_pipe.o cephalopod_thread_safe_queue.o cephalopod_host_to_ip.o logic.o cephalopod_argument.o
	$(CXX) -o trans trans.o cephalopod_black_hole.o cephalopod_http.o cephalopod_http_tools.o cephalopod_pipe.o cephalopod_thread_safe_queue.o cephalopod_host_to_ip.o logic.o cephalopod_argument.o -lpthread

trans.o: trans.cpp
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o trans.o trans.cpp

proxy.o: proxy.cpp
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o proxy.o proxy.cpp

cephalopod_argument.o: cephalopod_argument.cpp cephalopod_argument.h
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o cephalopod_argument.o cephalopod_argument.cpp

cephalopod_black_hole.o: cephalopod_black_hole.cpp cephalopod_black_hole.h
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o cephalopod_black_hole.o cephalopod_black_hole.cpp

cephalopod_http.o: cephalopod_http.cpp cephalopod_http.h
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o cephalopod_http.o cephalopod_http.cpp

cephalopod_http_tools.o: cephalopod_http_tools.cpp cephalopod_http_tools.h
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o cephalopod_http_tools.o cephalopod_http_tools.cpp

cephalopod_pipe.o: cephalopod_pipe.cpp cephalopod_pipe.h
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o cephalopod_pipe.o cephalopod_pipe.cpp

cephalopod_thread_safe_queue.o: cephalopod_thread_safe_queue.cpp cephalopod_thread_safe_queue.h
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o cephalopod_thread_safe_queue.o cephalopod_thread_safe_queue.cpp

cephalopod_host_to_ip.o: cephalopod_host_to_ip.cpp cephalopod_host_to_ip.h
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o cephalopod_host_to_ip.o cephalopod_host_to_ip.cpp

logic.o: logic.cpp logic.h
	$(CXX) $(CXXFLAGS) -std=c++17 -c -o logic.o logic.cpp
