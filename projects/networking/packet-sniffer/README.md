# Pcap Sniffer

This is a network capture utility that promiscuously captures raw frames across all, or selected, interfaces and writes them to a rolling 2 hour .pcap file that can be opened or read by tools like tcpdump or Wireshark. By default it excludes loopback

It operates as a systemd service. The intention being that it passively records all traffic that passes through the device from the last 2 hours, and whenever necessary a remote host can retrieve the .pcap file for analysis.

Main features:
- Passive promiscuous capture on all interfaces or on a user-specified set of interfaces
- Excludes lo by default
- Writes a valid .pcap file with proper global and per-packet headers
- Rolling 2 hour capture window: file is overwritten and reinitialized
- Systemd service that starts on boot and restarts on failure.
- Graceful shutdown on SIGTERM or SIGINT, disabling promiscuous mode on the interfaces and cleaning up resources.

## Architecture & Implementation Details

**Socket**
**Promiscuous Mode**
**Pcap Formatting**
**Rolling Window**
**Capture Loop**
**Shutdown**

## Installation

## Retrieval

## Uninstallation

## Notes

