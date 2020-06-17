# Ping Application

CLI applications for macOS X and above (macOS Catalina specifically) and Linux that perform the 'ping' command.
The programs are similar in functionality but cannot be used on either OS because of the different source header files.

#### To Compile and Run:
Run the Makefiles provided and run by `sudo ./ping <domain_name/IP_address>`.
The Makefile for the Linux version has the debug option (run `make debug`) for creating an executable suitable for debugging using lldb or gdb command-line debuggers.

#### The Program:
The program uses `getaddrinfo()` to obtain the IP address from a URL entered as a command-line argument. It obtains a raw socket for ICMP protocol and sets TTL (Time To Live) to 64 and the timeout to 3 seconds. A packet is created and then sent to the IP address using `sendto()` and received using `recvfrom()`. The relavant calculations are done and info is displayed.
Displays:
- RTT (Round Trip Time)
- Packet Success Percentage
- Error Number if encountered

#### Issues:
- The macOS version gets the error code 65 (errno 65): no route to host.
- The Linux version fails to receive anything before it times out (errno 35) and restarts the process.

#### Resources Consulted:
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)
