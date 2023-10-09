
#ifdef _MSC_VER // 如果是 Visual Studio 编译器
/*
 * we do not want the warnings about the old deprecated and unsecure CRT functions
 * since these examples can be compiled under *nix as well
 */
#define _CRT_SECURE_NO_WARNINGS // 禁用编译器警告
#endif

#include <pcap.h> // 捕获网络数据包的库
#include <time.h> // 获取和操作系统时间有关的信息

#ifdef _WIN32 // 如果是 Windows 环境下编译
#include <tchar.h> // 尤其在需要在 Windows 环境下同时支持 ANSI 和 Unicode 字符集的情况下非常有用

/*
把系统目录中的npcap目录放置到dll搜索路径中。
*/
BOOL LoadNpcapDlls()
{
	_TCHAR npcap_dir[512]; // 其实就是一个 Char，根据Visual Studio设定的字符类型（Unicode）自适应
	UINT len; // unsigned int 缩写
	len = GetSystemDirectory(npcap_dir, 480); // npcap_dir 指向的空间里放 C:\\Windows\\system32 返回 长度
	if (!len) {
		fprintf(stderr, "Error in GetSystemDirectory: %x", GetLastError()); // 这时候会打印在 console ； GetLastError 用于获取最近发生的错误代码
		return FALSE;
	}
	_tcscat_s(npcap_dir, 512, _T("\\Npcap")); // 字符串连接，512表示不能超过这大小
	if (SetDllDirectory(npcap_dir) == 0) { // 调用 SetDllDirectory 函数，将 npcap_dir 添加到 DLL 搜索路径中。如果调用失败，false表明失败了
		fprintf(stderr, "Error in SetDllDirectory: %x", GetLastError());
		return FALSE;
	}
	return TRUE;
}
#endif

/* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1; // unsigned char 0 ~ 255
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits) 例子: 0x45，其中版本号是 4 (IPv4)，头部长度是 5（5 * 4 字节 = 20 字节）
	u_char	tos;			// Type of service									  示例：0x00，通常不设置特殊的服务类型
	u_short tlen;			// Total length										  示例：0x0014，总长度为 20 字节（不包括数据部分）
	u_short identification; // Identification 								      示例：0x1234，用于数据包的唯一标识
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)         示例：0x4000，其中标志位为 010，分片偏移为 0
	u_char	ttl;			// Time to live									      示例：0x40，生存时间为 64 跳
	u_char	proto;			// Protocol											  示例：0x06，表示上层协议是 TCP
	u_short crc;			// Header checksum									  示例：0x7A42，用于校验头部数据的完整性
	ip_address	saddr;		// Source address									  示例：0xC0A80101，表示源 IP 地址为 192.168.1.1
	ip_address	daddr;		// Destination address								  示例：0xC0A80102，表示目标 IP 地址为 192.168.1.2
	u_int	op_pad;			// Option + Padding									  示例：通常为空，不包含选项和填充数据
}ip_header;

/* UDP header*/
typedef struct udp_header
{
	u_short sport;			// Source port			示例：0x1234，表示源端口号为 4660。
	u_short dport;			// Destination port		示例：0x80，表示目标端口号为 128。
	u_short len;			// Datagram length		示例：0x0040，表示数据报的总长度为 64 字节。
	u_short crc;			// Checksum				示例：通常为空（0x0000），因为校验和在 UDP 头部中不是必需的，可以选择性使用。
}udp_header;

/* TCP header */
typedef struct tcp_header
{
	u_short sport;         // 源端口(Source Port)						示例：0x1234，表示源端口号为 4660。
	u_short dport;         // 目标端口(Destination Port)					示例：0x80，表示目标端口号为 128。
	u_int seq_num;         // 序列号(Sequence Number)					示例：0xabcdef01，表示序列号为 0xabcdef01。
	u_int ack_num;         // 确认号(Acknowledgment Number)				示例：0x12345678，表示确认号为 0x12345678。
	u_short offset_flags;  // 数据偏移(Data Offset) + 控制标志(Flags)		示例：0x5002，其中数据偏移为 5（5 * 4 字节 = 20 字节），控制标志中的 SYN 标志位被设置。
	u_short window_size;   // 窗口大小(Window Size)						示例：0x7fff，表示窗口大小为 32767 字节。
	u_short checksum;      // 校验和(Checksum)							示例：通常为一些校验和值，用于验证 TCP 头部和数据的完整性。
	u_short urgent_ptr;    // 紧急指针(Urgent Pointer)					示例：0x0000，通常不设置紧急指针。
} tcp_header;

/* prototype of the packet handler */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data); // 函数声明


int main()
{
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i=0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	char packet_filter[] = "ip and tcp";
	struct bpf_program fcode;
	
#ifdef _WIN32
	/* Load Npcap and its functions. */
	if (!LoadNpcapDlls())
	{
		fprintf(stderr, "Couldn't load Npcap\n");
		exit(1);
	}
#endif

	/* Retrieve the device list */
	if(pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}
	
	/* Print the list */
	for(d=alldevs; d; d=d->next)
	{
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}

	if(i==0)
	{
		printf("\nNo interfaces found! Make sure Npcap is installed.\n");
		return -1;
	}
	
	printf("Enter the interface number (1-%d):",i);
	scanf("%d", &inum);
	
	/* Check if the user specified a valid adapter */
	if(inum < 1 || inum > i)
	{
		printf("\nAdapter number out of range.\n");
		
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}

	/* Jump to the selected adapter */
	for(d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
	
	/* Open the adapter */
	if ((adhandle= pcap_open_live(d->name,	// name of the device
							 65536,			// portion of the packet to capture. 
											// 65536 grants that the whole packet will be captured on all the MACs.
							 1,				// promiscuous mode (nonzero means promiscuous)
							 1000,			// read timeout
							 errbuf			// error buffer
							 )) == NULL)
	{
		fprintf(stderr,"\nUnable to open the adapter: %s\n", errbuf);
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	/* Check the link layer. We support only Ethernet for simplicity. */
	if(pcap_datalink(adhandle) != DLT_EN10MB)
	{
		fprintf(stderr,"\nThis program works only on Ethernet networks.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	if(d->addresses != NULL)
		/* Retrieve the mask of the first address of the interface */
		netmask=((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* If the interface is without addresses we suppose to be in a C class network */
		netmask=0xffffff; 


	//compile the filter
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0 )
	{
		fprintf(stderr,"\nUnable to compile the packet filter. Check the syntax.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	//set the filter
	if (pcap_setfilter(adhandle, &fcode)<0)
	{
		fprintf(stderr,"\nError setting the filter.\n");
		/* Free the device list */
		pcap_freealldevs(alldevs);
		return -1;
	}
	
	printf("\nlistening on %s...\n", d->description);
	
	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);
	
	/* start the capture */
	pcap_loop(adhandle, 0, packet_handler, NULL);
	
	return 0;
}

/* Callback function invoked by libpcap for every incoming packet */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct tm *ltime;
	char timestr[16];
	ip_header *ih;
	udp_header *uh;
	u_int ip_len;
	u_short sport,dport;
	time_t local_tv_sec;

	/*
	 * unused parameter
	 */
	(VOID)(param);

	/* convert the timestamp to readable format */
	local_tv_sec = header->ts.tv_sec;
	ltime=localtime(&local_tv_sec);
	strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);

	/* print timestamp and length of the packet */
	printf("%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);

	/* retireve the position of the ip header */
	ih = (ip_header *) (pkt_data +
		14); //length of ethernet header

	/* retireve the position of the udp header */
	ip_len = (ih->ver_ihl & 0xf) * 4;
	uh = (udp_header *) ((u_char*)ih + ip_len);

	/* convert from network byte order to host byte order */
	sport = ntohs( uh->sport );
	dport = ntohs( uh->dport );

	/* print ip addresses and udp ports */
	printf("%d.%d.%d.%d.%d -> %d.%d.%d.%d.%d\n",
		ih->saddr.byte1,
		ih->saddr.byte2,
		ih->saddr.byte3,
		ih->saddr.byte4,
		sport,
		ih->daddr.byte1,
		ih->daddr.byte2,
		ih->daddr.byte3,
		ih->daddr.byte4,
		dport);
}
