#!/bin/sh -e

echo "--- RUN INTEGRATION TEST ---"

echo "Clear process"
ps -fC ozzl-test-olekd | awk '{print $2}' | sed -n '1!p' | xargs -i{} kill -9 {}
ps -fC ozzl-test-olek-client | awk '{print $2}' | sed -n '1!p' | xargs -i{} kill -9 {}

echo "Start server"
bin/ozzl-test-olekd > /dev/null 2>&1 &
SRV_PID=$!

echo "Run 4 clients"
# expect 1000000 double values 1.5
bin/ozzl-test-olek-client -c etc/ozzl/test-olek-client/client.conf.default -o run/1.dbl -v AAFF0001 -r 0.5 > run/1.log 2>&1 &
PID1=$!
# expect 1500000 double values 2.5
bin/ozzl-test-olek-client -c etc/ozzl/test-olek-client/client.conf.default -o run/2.dbl -v AAFF0002 -r 1.5 > run/2.log 2>&1 &
PID2=$!
# expect version mismatch
bin/ozzl-test-olek-client -c etc/ozzl/test-olek-client/client.conf.default -o run/3.dbl -v AAFF0003 -r 2.5 > run/3.log 2>&1 || true &
PID3=$!
# expect unexpected protocol error
bin/ozzl-test-olek-client -c etc/ozzl/test-olek-client/client.conf.default -o run/4.dbl -v AAEE0003 -r 3.5 > run/4.log 2>&1 || true &
PID4=$!

sleep 2

echo "Check nuber of server threads..."
if [ ! `cat /proc/$SRV_PID/status | grep Threads | awk '{printf "%d", $2}'` -gt 4 ]; then
    echo "Number of threads less than 5. Expect 5 threads."
    exit 1
fi
echo "`cat /proc/$SRV_PID/status | grep Threads`"

echo "Wait for data transfer..."
wait $PID1
wait $PID2
wait $PID3
wait $PID4
echo "All data transfered"

sleep 2

echo "Check number of server threads..."
if [ ! `cat /proc/$SRV_PID/status | grep Threads | awk '{printf "%d", $2}'` -eq 1 ]; then
    echo "Number of threads not equal 1. Expect 1 thread."
    exit 1
fi
echo "`cat /proc/$SRV_PID/status | grep Threads`"

echo "Clear process"
ps -fC ozzl-test-olekd | awk '{print $2}' | sed -n '1!p' | xargs -i{} kill -9 {}
ps -fC ozzl-test-olek-client | awk '{print $2}' | sed -n '1!p' | xargs -i{} kill -9 {}

echo "Check client's stdout and stderr outputs..."
if [ "`cat run/1.log`" != "ozzl test: client: received total bytes: 8000000" ]; then
    echo "Form client 1 received message [`cat run/1.log`]'"
    echo " expect: [ozzl test: client: received total bytes: 8000000]"
    exit 1
fi
if [ "`cat run/2.log`" != "ozzl test: client: received total bytes: 12000000" ];then
    echo "Form client 2 received message [`cat run/2.log`]'"
    echo " expect: [ozzl test: client: received total bytes: 12000000]"
    exit 1
fi
if [ "`cat run/3.log`" != "ozzl test: client: Error: 0x65 Protocol version mismatch. Supported versions: [0xaaff0001] [0xaaff0002]" ]; then
    echo "Form client 3 received message [`cat run/3.log`]'"
    echo " expect: [ozzl test: client: Error: 0x65 Protocol version mismatch. Supported versions: [0xaaff0001] [0xaaff0002]"
    exit 1
fi
if [ "`cat run/4.log`" != "ozzl test: client: unexpected protocol error" ]; then
    echo "Form client 4 received message [`cat run/4.log`]'"
    echo " expect: [ozzl test: client: unexpected protocol error]"
    exit 1
fi
echo "Client's stdout and stderr outputs are ok"

echo "Check data from client 1"
i=0; while [ $i -le 1000000 ]; do
    if [ "`xxd -p -l 8 -s $i run/1.dbl`" != "3ff8000000000000" ]; then
        exit 1
    fi
    i=$((i+8))
done

echo "Check data from client 2"
i=0; while [ $i -le 1500000 ]; do
    if [ "`xxd -p -l 8 -s $i run/2.dbl`" != "4004000000000000" ]; then
        exit 1
    fi
    i=$((i+8))
done

echo "--- TEST PASSED ---"
