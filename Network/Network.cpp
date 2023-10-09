
#ifdef _MSC_VER // ����� Visual Studio ������
/*
 * we do not want the warnings about the old deprecated and unsecure CRT functions
 * since these examples can be compiled under *nix as well
 */
#define _CRT_SECURE_NO_WARNINGS // ���ñ���������
#endif

#include <pcap.h> // �����������ݰ��Ŀ�
#include <time.h> // ��ȡ�Ͳ���ϵͳʱ���йص���Ϣ

#ifdef _WIN32 // ����� Windows �����±���
#include <tchar.h> // ��������Ҫ�� Windows ������ͬʱ֧�� ANSI �� Unicode �ַ���������·ǳ�����

/*
��ϵͳĿ¼�е�npcapĿ¼���õ�dll����·���С�
*/
BOOL LoadNpcapDlls()
{
	_TCHAR npcap_dir[512]; // ��ʵ����һ�� Char������Visual Studio�趨���ַ����ͣ�Unicode������Ӧ
	UINT len; // unsigned int ��д
	len = GetSystemDirectory(npcap_dir, 480); // npcap_dir ָ��Ŀռ���� C:\\Windows\\system32 ���� ����
	if (!len) {
		fprintf(stderr, "Error in GetSystemDirectory: %x", GetLastError()); // ��ʱ����ӡ�� console �� GetLastError ���ڻ�ȡ��������Ĵ������
		return FALSE;
	}
	_tcscat_s(npcap_dir, 512, _T("\\Npcap")); // �ַ������ӣ�512��ʾ���ܳ������С
	if (SetDllDirectory(npcap_dir) == 0) { // ���� SetDllDirectory �������� npcap_dir ��ӵ� DLL ����·���С��������ʧ�ܣ�false����ʧ����
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
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits) ����: 0x45�����а汾���� 4 (IPv4)��ͷ�������� 5��5 * 4 �ֽ� = 20 �ֽڣ�
	u_char	tos;			// Type of service									  ʾ����0x00��ͨ������������ķ�������
	u_short tlen;			// Total length										  ʾ����0x0014���ܳ���Ϊ 20 �ֽڣ����������ݲ��֣�
	u_short identification; // Identification 								      ʾ����0x1234���������ݰ���Ψһ��ʶ
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)         ʾ����0x4000�����б�־λΪ 010����Ƭƫ��Ϊ 0
	u_char	ttl;			// Time to live									      ʾ����0x40������ʱ��Ϊ 64 ��
	u_char	proto;			// Protocol											  ʾ����0x06����ʾ�ϲ�Э���� TCP
	u_short crc;			// Header checksum									  ʾ����0x7A42������У��ͷ�����ݵ�������
	ip_address	saddr;		// Source address									  ʾ����0xC0A80101����ʾԴ IP ��ַΪ 192.168.1.1
	ip_address	daddr;		// Destination address								  ʾ����0xC0A80102����ʾĿ�� IP ��ַΪ 192.168.1.2
	u_int	op_pad;			// Option + Padding									  ʾ����ͨ��Ϊ�գ�������ѡ����������
}ip_header;

/* UDP header*/
typedef struct udp_header
{
	u_short sport;			// Source port			ʾ����0x1234����ʾԴ�˿ں�Ϊ 4660��
	u_short dport;			// Destination port		ʾ����0x80����ʾĿ��˿ں�Ϊ 128��
	u_short len;			// Datagram length		ʾ����0x0040����ʾ���ݱ����ܳ���Ϊ 64 �ֽڡ�
	u_short crc;			// Checksum				ʾ����ͨ��Ϊ�գ�0x0000������ΪУ����� UDP ͷ���в��Ǳ���ģ�����ѡ����ʹ�á�
}udp_header;

/* TCP header */
typedef struct tcp_header
{
	u_short sport;         // Դ�˿�(Source Port)						ʾ����0x1234����ʾԴ�˿ں�Ϊ 4660��
	u_short dport;         // Ŀ��˿�(Destination Port)					ʾ����0x80����ʾĿ��˿ں�Ϊ 128��
	u_int seq_num;         // ���к�(Sequence Number)					ʾ����0xabcdef01����ʾ���к�Ϊ 0xabcdef01��
	u_int ack_num;         // ȷ�Ϻ�(Acknowledgment Number)				ʾ����0x12345678����ʾȷ�Ϻ�Ϊ 0x12345678��
	u_short offset_flags;  // ����ƫ��(Data Offset) + ���Ʊ�־(Flags)		ʾ����0x5002����������ƫ��Ϊ 5��5 * 4 �ֽ� = 20 �ֽڣ������Ʊ�־�е� SYN ��־λ�����á�
	u_short window_size;   // ���ڴ�С(Window Size)						ʾ����0x7fff����ʾ���ڴ�СΪ 32767 �ֽڡ�
	u_short checksum;      // У���(Checksum)							ʾ����ͨ��ΪһЩУ���ֵ��������֤ TCP ͷ�������ݵ������ԡ�
	u_short urgent_ptr;    // ����ָ��(Urgent Pointer)					ʾ����0x0000��ͨ�������ý���ָ�롣
} tcp_header;

/* prototype of the packet handler */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data); // ��������


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
