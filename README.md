# Milad Izra Project 2: RPC Server-Client System 
This project implements a simple RPC (Remote Procedure Call) system that allows clients to remotely execute functions on a server. The system supports basic data structures and multithreading. The server contains a simple adding function for testing purposes however can be modified to suite other basic computation requirements. 

For example, the RPC system can transmit a variety of data types through the ```void *```pointer in the ```rpc_data``` structure. This flexibility allows the system to handle different kinds of inputs and outputs, making it versatile for various use cases.

The system uses TCP/IP for communication, relying on standard socket programming interfaces. 


## Usage/Examples
**To Compile**
```c
make clean && make -B
```
**To run the server, type:**
```c
./rpc-server -p <port> &
./rpc-client -i <ip-address> -p <port>
```

**Sample Output**
```shell
Result of adding 0 and 100: 100
Result of adding 1 and 100: 101
```
## Authors

- [@mizra2](https://www.github.com/mizra2)

