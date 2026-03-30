# Packet Sniffer

This is a network capture utility that promiscuously captures raw frames across all, or selected, interfaces and writes them to a rolling 2 hour .pcap file that can be opened or read by tools like tcpdump or Wireshark. By default it excludes loopback

It operates as a systemd service. The intention being that it passively records all traffic that passes through the device in the designated time window to a local file, and whenever necessary a remote host can retrieve the .pcap file for analysis /var/lib/packet-sniffer/captures.pcap via scp

Main features:
- Passive and promiscuous capture on all interfaces or on a user-specified set of interfaces
- Excludes lo by default
- Writes a valid .pcap file with proper global and per packet headers
- Rolling 2 hour capture window: file is overwritten and reinitialized
- Systemd service that starts on boot and restarts on failure.
- Graceful shutdown on SIGTERM or SIGINT, disabling promiscuous mode on the interfaces and cleaning up resources.

## Architecture & Implementation Details

**Socket**

The main socket is very simple technically, it catches all frames on all interfaces. Filtering for the interface happens later on outside of the socket. After opening the raw socket and an error check, the next step is to turn on promiscuous mode on all interfaces but lo (by default) or all specified interfaces in arguments. This works by first checking for any arguments and then looping thorugh them calling a function to set promiscuous for each one very simply, and skipping argv[0] which is the name of the program. If there are no arguments then it uses the POSIX call if_nameindex() to get an array of sructs containing interface names and indeces. It then loops through the array, incrementing the pointer and checking for valid fields as a loop condition, doing set_promisc on each interface and string compares with lo to skip if true. It then frees the memory of this array after setting promisc mode on all interfaces.

**Promiscuous Mode**

Setting promiscuous mode on an interface means that the NIC will pass all frames to the kernel regardless of destination MAC address... This is an important feature for a packet sniffer and it works like this: ioctl is used for this by reading the permission bits and then writing them back with the promiscuous bit added. Get all the of the interface flags first with SIOCGIFFLAGS. Use the bitwise operator OR to set the permission bit IFF_PROMISC. The interface name is tracked in heap in a global array for cleanup purposes later. In principle, the cleanup function works a very similar way as the set promiscuous function, but rather than using bitwise OR, it uses bitwise NOT to first invert the retrieved flags before doing AND to turn on all previous flags but the promisc bit.

**PCAP Format**

On startup and every initiliazation, a global header of 24 bytes needs to be written to the file which consists of a magic number, major version, minor version, snaplen, and link type. The global header is defined in a struct before reaching the main loop. Preceding every packet that is captured, a 16 byte packet header must be written too, consisting of timestamp in seconds and microseconds, length of packet saved in file, and original length of packet.

**Rolling Window**

Every iteration of the main loop checks if 2 hours have elapsed, if true, then the file is truncated and reinitialized with a fresh pcap header. Only the current 2 hour window is retained on disk.

**Capture Loop**

Every received frame from recvfrom is checked against the interface filter using if_indextoname, argv, and a for loop. Every matching frame is written and flushed to the disk

**Shutdown & Cleanup**

Upon SIGTERM or SIGINT, keep_running is set to 0 and the main loop terminates. Interfaces are restored to non-promiscuous mode, socket is closed, and all allocated memory is freed.

### Scripts

The installation script creates a service user by the name of "pcapsnifferd", compiles and installs the binary "pcapsnifferd" into /usr/local/bin, and creates a systemd service file "pcapsniffer.service" for the binary with capabilities: CAP_NET_RAW and CAP_NET_ADMIN.

The uninstallation script simply reverses and deletes everything that the installation script created.

## Installation & Usage

```
$ sudo chmod +x install.sh
$ sudo ./install.sh
$ systemctl enable --now pcapsniffer.service
```

### Retrieval

Pull the capture file from the remote host with scp:
```
$ scp user@host:/var/lib/pcap-sniffer/file.pcap
$ ./capture.pcap

```
Open in tcpdump or Wireshark:
```
$ tcpdump -r capture.pcap
$ wireshark capture.pcap
```

## Uninstallation

```
$ sudo ./uninstall.sh
```

## Note:

- The rolling overwrite means only the most recent 2 hour window is kept on disk. Pull the file before rotation if longer retention is needed.
- Packet loss is possible under high traffic load since no kernel ring buffer is used.
