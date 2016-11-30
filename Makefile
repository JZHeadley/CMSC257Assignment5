

all : server_sequential server_threaded client
    
server_sequential : server_dir/server_sequential.c
	gcc $^ -g -o $@

server_threaded : server_dir/server_threaded.c
	gcc $^ -g -o $@ -lpthread -lrt

client : client_dir/client.c
	gcc $^ -g -o $@

clean : 
	rm -f client server_threaded server_sequential client_dir/asdf
