#

OFPROTOSRCS += datapath.c match.c crc32.c ofcache.c murmur3.c city.c
OFPROTOSRCS += flowinfo.c flowinfo_basic.c flowinfo_ether.c
OFPROTOSRCS += flowinfo_ipv4_proto.c flowinfo_ipv4_dst.c
OFPROTOSRCS += flowinfo_ipv6.c flowinfo_mpls.c flowinfo_port.c flowinfo_vlan.c
OFPROTOSRCS += flowinfo_metadata.c
OFPROTOSRCS += pcap.c thread.c comm.c ofcache_show.c version.c

LDFLAGS+= -lpcap
