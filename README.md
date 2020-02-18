# sntp
 
Frame 1: 90 bytes on wire (720 bits), 90 bytes captured (720 bits) on interface 0
    Interface id: 0 (eth0)
        Interface name: eth0
    Encapsulation type: Ethernet (1)
    Arrival Time: Feb 18, 2020 13:40:58.960153391 CET
    [Time shift for this packet: 0.000000000 seconds]
    Epoch Time: 1582029658.960153391 seconds
    [Time delta from previous captured frame: 0.000000000 seconds]
    [Time delta from previous displayed frame: 0.000000000 seconds]
    [Time since reference or first frame: 0.000000000 seconds]
    Frame Number: 1
    Frame Length: 90 bytes (720 bits)
    Capture Length: 90 bytes (720 bits)
    [Frame is marked: False]
    [Frame is ignored: False]
    [Protocols in frame: eth:ethertype:ip:udp:ntp]
Ethernet II, Src: Raspberr_65:11:08 (b8:27:eb:65:11:08), Dst: AvmAudio_e5:6e:bf (44:4e:6d:e5:6e:bf)
    Destination: AvmAudio_e5:6e:bf (44:4e:6d:e5:6e:bf)
        Address: AvmAudio_e5:6e:bf (44:4e:6d:e5:6e:bf)
        .... ..0. .... .... .... .... = LG bit: Globally unique address (factory default)
        .... ...0 .... .... .... .... = IG bit: Individual address (unicast)
    Source: Raspberr_65:11:08 (b8:27:eb:65:11:08)
        Address: Raspberr_65:11:08 (b8:27:eb:65:11:08)
        .... ..0. .... .... .... .... = LG bit: Globally unique address (factory default)
        .... ...0 .... .... .... .... = IG bit: Individual address (unicast)
    Type: IPv4 (0x0800)
Internet Protocol Version 4, Src: 192.168.178.47, Dst: 192.168.178.1
    0100 .... = Version: 4
    .... 0101 = Header Length: 20 bytes (5)
    Differentiated Services Field: 0x00 (DSCP: CS0, ECN: Not-ECT)
        0000 00.. = Differentiated Services Codepoint: Default (0)
        .... ..00 = Explicit Congestion Notification: Not ECN-Capable Transport (0)
    Total Length: 76
    Identification: 0x27c7 (10183)
    Flags: 0x4000, Don't fragment
        0... .... .... .... = Reserved bit: Not set
        .1.. .... .... .... = Don't fragment: Set
        ..0. .... .... .... = More fragments: Not set
        ...0 0000 0000 0000 = Fragment offset: 0
    Time to live: 64
    Protocol: UDP (17)
    Header checksum: 0x2d58 [validation disabled]
    [Header checksum status: Unverified]
    Source: 192.168.178.47
    Destination: 192.168.178.1
User Datagram Protocol, Src Port: 55287, Dst Port: 123
    Source Port: 55287
    Destination Port: 123
    Length: 56
    Checksum: 0xe5cb [unverified]
    [Checksum Status: Unverified]
    [Stream index: 0]
Network Time Protocol (NTP Version 3, client)
    Flags: 0x1b, Leap Indicator: no warning, Version number: NTP Version 3, Mode: client
        00.. .... = Leap Indicator: no warning (0)
        ..01 1... = Version number: NTP Version 3 (3)
        .... .011 = Mode: client (3)
    Peer Clock Stratum: unspecified or invalid (0)
    Peer Polling Interval: invalid (0)
    Peer Clock Precision: 1.000000 sec
    Root Delay: 0 seconds
    Root Dispersion: 0 seconds
    Reference ID: NULL
    Reference Timestamp: Jan  1, 1970 00:00:00.000000000 UTC
    Origin Timestamp: Jan  1, 1970 00:00:00.000000000 UTC
    Receive Timestamp: Jan  1, 1970 00:00:00.000000000 UTC
    Transmit Timestamp: Jan  1, 1970 00:00:00.000000000 UTC

Frame 2: 90 bytes on wire (720 bits), 90 bytes captured (720 bits) on interface 0
    Interface id: 0 (eth0)
        Interface name: eth0
    Encapsulation type: Ethernet (1)
    Arrival Time: Feb 18, 2020 13:40:58.960653594 CET
    [Time shift for this packet: 0.000000000 seconds]
    Epoch Time: 1582029658.960653594 seconds
    [Time delta from previous captured frame: 0.000500203 seconds]
    [Time delta from previous displayed frame: 0.000500203 seconds]
    [Time since reference or first frame: 0.000500203 seconds]
    Frame Number: 2
    Frame Length: 90 bytes (720 bits)
    Capture Length: 90 bytes (720 bits)
    [Frame is marked: False]
    [Frame is ignored: False]
    [Protocols in frame: eth:ethertype:ip:udp:ntp]
Ethernet II, Src: AvmAudio_e5:6e:bf (44:4e:6d:e5:6e:bf), Dst: Raspberr_65:11:08 (b8:27:eb:65:11:08)
    Destination: Raspberr_65:11:08 (b8:27:eb:65:11:08)
        Address: Raspberr_65:11:08 (b8:27:eb:65:11:08)
        .... ..0. .... .... .... .... = LG bit: Globally unique address (factory default)
        .... ...0 .... .... .... .... = IG bit: Individual address (unicast)
    Source: AvmAudio_e5:6e:bf (44:4e:6d:e5:6e:bf)
        Address: AvmAudio_e5:6e:bf (44:4e:6d:e5:6e:bf)
        .... ..0. .... .... .... .... = LG bit: Globally unique address (factory default)
        .... ...0 .... .... .... .... = IG bit: Individual address (unicast)
    Type: IPv4 (0x0800)
Internet Protocol Version 4, Src: 192.168.178.1, Dst: 192.168.178.47
    0100 .... = Version: 4
    .... 0101 = Header Length: 20 bytes (5)
    Differentiated Services Field: 0x00 (DSCP: CS0, ECN: Not-ECT)
        0000 00.. = Differentiated Services Codepoint: Default (0)
        .... ..00 = Explicit Congestion Notification: Not ECN-Capable Transport (0)
    Total Length: 76
    Identification: 0x0000 (0)
    Flags: 0x4000, Don't fragment
        0... .... .... .... = Reserved bit: Not set
        .1.. .... .... .... = Don't fragment: Set
        ..0. .... .... .... = More fragments: Not set
        ...0 0000 0000 0000 = Fragment offset: 0
    Time to live: 64
    Protocol: UDP (17)
    Header checksum: 0x551f [validation disabled]
    [Header checksum status: Unverified]
    Source: 192.168.178.1
    Destination: 192.168.178.47
User Datagram Protocol, Src Port: 123, Dst Port: 55287
    Source Port: 123
    Destination Port: 55287
    Length: 56
    Checksum: 0xeb11 [unverified]
    [Checksum Status: Unverified]
    [Stream index: 0]
Network Time Protocol (NTP Version 3, server)
    Flags: 0x1c, Leap Indicator: no warning, Version number: NTP Version 3, Mode: server
        00.. .... = Leap Indicator: no warning (0)
        ..01 1... = Version number: NTP Version 3 (3)
        .... .100 = Mode: server (4)
    Peer Clock Stratum: secondary reference (3)
    Peer Polling Interval: invalid (0)
    Peer Clock Precision: 0.000001 sec
    Root Delay: 0.02569580078125 seconds
    Root Dispersion: 0.0064544677734375 seconds
    Reference ID: 37.247.53.178
    Reference Timestamp: Feb 18, 2020 12:36:18.803289821 UTC
    Origin Timestamp: Jan  1, 1970 00:00:00.000000000 UTC
    Receive Timestamp: Feb 18, 2020 12:40:58.961177052 UTC
    Transmit Timestamp: Feb 18, 2020 12:40:58.961415264 UTC
