#!/bin/bash

#examine command line arguments
#echo "Test: I see $# arguments."
if [ ! $# == 1 ]; then
  echo "TEST: Usage: one of \"run\" or \"build\"."
  exit
fi

if [ $1 == "clean" ]; then
	rm server client
	exit 0
fi

if [ $1 == "run" ] || [ $1 == "build" ]; then
	# Ensure both files exist
	echo "TEST: Compiling server and client."
	if [ ! -f "server.c" ] || [ ! -f "client.c" ]; then
    	echo "TEST: server.c or client.c not found."
    	exit 1
	fi

	# Compile server
	gcc -o server server.c
	if [ $? -ne 0 ]; then
    	echo "TEST: Failed to compile server.c"
    	exit 1
	fi

	# Compile client
	gcc -o client client.c
	if [ $? -ne 0 ]; then
    	echo "TEST: Failed to compile client.c"
    	exit 1
	fi
fi

if [ $1 == "run" ]; then
	echo "TEST: Starting server and client."
	# Start the server in the background
	./server &
	server_pid=$!
    echo "TEST: Started server has process id: $server_pid"

	# Start 5 clients in the background
	for i in {1..5}; do
    	./client localhost 4221 & 
    	client_pids[$i]=$!
        #echo "Started client $i with process id: $!"
	done

    echo "TEST: Process ids for clients started:" 
    for client in "${client_pids[@]}"; do
        echo "$client"
    done
	

    # Wait for all background processes to complete
	wait $server_pid ${client_pids[@]}

	# Clean up
	rm server client

	echo "TEST: Halting Test."	
fi
