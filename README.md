# Namiono-Server

Namiono-Server

Allows installing or booting Operating Systems via PXE over the Network. We hopefully can implement RBCP (Intels Remote Boot Configuration Protocol) with LCM Extensions... and also Apple's BSDP (BootServer Discovery Protocol) support. 

Goal is also to handle PXE booting across subnets like VPNs. PXE booting via VPNs is generally done by acting as Relay Agent,
Yes Namiono can also acting as (PXE) Relay Agent... ;) (working together with Windows RIS and WDS) and Apples Netboot 1 / 2...

Most of the work is done in free time and researching technology by looking for Wireshark Dumps or looking in other code of of other developers. Namiono is also a learning and researching software... So new or mainly rarely discovered features like Intels LCM Boot options was reported to the Wireshark Team to merge them Upstream (https://code.wireshark.org/review/#/c/32301/), (https://bugs.wireshark.org/bugzilla/show_bug.cgi?id=11544), (https://bugs.wireshark.org/bugzilla/show_bug.cgi?id=12179), (https://bugs.wireshark.org/bugzilla/show_bug.cgi?id=14312).

Namiono stays mainly as much as possible PXE 2.1 Spec aware. Some Features are hacked (like the Packet Relaying PXE appendings)
or DHCP Snooping. But these Features are mainly hacks so as like "I need an ability... so let us implement it).

Namiono is mainly written for linux... so you can run it on OpenWRT devices!
