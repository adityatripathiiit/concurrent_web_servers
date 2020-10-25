# !/bin/bash
./wclient localhost 5000 /tester4.html &
./wclient localhost 5000 /tester.html &

./wclient localhost 5000 /tester3.html &
./wclient localhost 5000 /tester2.html &
# ./wclient localhost 5000 /tester2.html &
# ./wclient localhost 5000 /tester3.html &
# ./wclient localhost 5000 /tester.html &
# ./wclient localhost 5000 /tester2.html &
# ./wclient localhost 5000 /tester3.html &
# ./wclient localhost 5000 /tester4.html &
