# SimpleFireWall
Illumio Coding Assignment 2019-2020, PCE teams

# Build
```
//gcc version 7.4.0, C++ standrand = C++14
g++ firewall.cpp -o firewall
```

# How to test
Just run the complied program  
The structure of the `test.csv` as follows:

|direction|protocol|port|ip|result(1 for true 0 for false)| 
|-------|----------|--------|--------|--------| 
|inbound|tcp|80|192.168.1.2|1| 

Here are the outputs in command line:
```
testing 1 inbound tcp 80 192.168.1.2 passed
testing 2 inbound tcp 79 192.168.1.2 passed
testing 3 outbound tcp 10234 192.168.10.11 passed
testing 4 outbound tcp 10000 192.168.10.9 passed
testing 5 outbound tcp 10000 192.168.10.10 passed
testing 6 outbound tcp 20000 192.168.10.11 passed
testing 7 outbound tcp 10234 192.168.10.9 passed
testing 8 inbound tcp 81 192.168.1.2 passed
testing 9 inbound udp 24 52.12.48.92 passed
testing 10 inbound udp 3000 192.168.1.0 passed
testing 11 inbound udp 3001 192.2.0.0 passed
testing 12 inbound udp 3999 192.168.255.255 passed
testing 13 inbound udp 4001 192.168.255.255 passed
```

# Design
1. I use 4 map to store the input information. Each of them is a port->IP interval tree. Since the range of port is much less than ip, port becomes the key.
2. The IP interval tree is a structure which translates a pair of IP address into a tree node(which map the formal IP address to 32 bit unsigned int).
3. When reading a line from `fw.csv `, first translate the input inline into the port interval and IP interval. For every port, insert the interval to its corresponding interval tree. The time complexity is the same as the `O(range(port))` for each line.
4. When calling accept_packet, first the corresponding interval tree by direction, protocol, and port number. Next, try to find whether there is a overleap between the tree and the interval. The time complexity is `O(log(n))`  n is the number of the interval for that root.

# Any improvement?
1. I will try to reduce the memory usage for port since every port in the range has been allocated an interval tree. Maybe I can try a nested interval tree.
2. The interval tree may become unbalanced with the number of insertions. I will try to merge the interval and build a new tree if it is too unbalanced. 

# My interests
I am very interested in 1.) Platform Team and 2.) Policy Team second. I hope to join you in the 2020 summer!
