
### Notes

`sudo ./build/interstate90 -l 0 --vdev=net_null0 --no-pci`

`-l`, `--lcores <core list>`
List of cores to run on

`-vdev`
Virtual device.

`--no-pci` 
Disable PCI, so in our case, do not rely on our hardware NIC.

EAL

`lcore` is a thread that EAL creates and pins to one specific CPU.