#!/bin/bash

echo "Doing 3 requests on single process server"
echo; time ./wclient 127.0.0.1 8080 "/spin.cgi?1" "/spin.cgi?2" "/spin.cgi?3"
echo; echo "Same requests on server with new process on each request"
time ./wclient 127.0.0.1 8081 "/spin.cgi?1" "/spin.cgi?2" "/spin.cgi?3"

