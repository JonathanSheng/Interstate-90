### Context
This project is an educational project to help understand more about Data Plane Development Kit (DPDK).

### Terms
**Network Interface Card** (NIC) is a hardware component that connects a computer or server to a network. It takes digital data from the computer and turns it into electrical, optical, or radio signals. It also works to translate those signals back into data.
See https://www.geeksforgeeks.org/computer-networks/nic-full-form/

**Ethernet Frame** is a fundamental unit of data transmission at Layer 2 of OSI. This wraps an IP packet and provides information, such as sender and recipient hardware addresses.

### Processing Packets in Linux
First, a packet is sent to a Network Interface Card.

### DPDK (Data Plane Development Kit)
NIC Poll Mode Driver continuously checks hardware rings instead of waiting for interrupts.
https://kernel-internals.org/net/life-of-packet-rx/


### Running this project
Using Lima to run Linux VM through CLI
```
limactl shell Interstate90
limactl stop Interstate90
```

Installation:
```
sudo apt update
sudo apt install -y build-essential meson ninja-build pkg-config \
  g++ libdpdk-dev libpcap-dev python3 python3-pyelftools
```
