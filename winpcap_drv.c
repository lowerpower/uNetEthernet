/*! \file winpcap_drv.c \brief Winpcap "Ethernet" Interface Driver for windows. */


//#ifdef WIN32
#include <pcap.h>
//#else /* WIN32 */
//#include <stdlib.h>
//#include <stdio.h>
//#endif

extern char pcap_iface;

#define LINE_LEN 16

// Local iface variable
static pcap_t *iface;

void nicInit(void)
{
	pcap_if_t *alldevs, *d;
	char	errbuf[PCAP_ERRBUF_SIZE];
	int		inum, i=0;
	

	if(pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		fprintf(stderr,"Error in pcap_findalldevs: %s\n", errbuf);
		exit(1);
	}

	if(0==pcap_iface)
	{
		printf("\nNo adapter selected: printing the device list:\n");
	}
	else
		inum=pcap_iface;

	/* Print the list (linked list) */
	for(d=alldevs; d; d=d->next)
    {
		i++;
		if(0==pcap_iface)
		{
			printf("%d. %s\n    ", i, d->name);
			if (d->description)
				printf(" (%s)\n", d->description);
	        else
		        printf(" (No description available)\n");
		}
	}
        
	if (i==0)
    {
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		exit(0);
	}
	
	if(0==pcap_iface)
	{        
		printf("Enter the interface number (1-%d):",i);
		scanf("%d", &inum);
	}
    
	if (inum < 1 || inum > i)
	{
			printf("\nInterface number out of range.\n");

        /* Free the device list */
        pcap_freealldevs(alldevs);
		exit(0);
	}
        
    /* Jump to the selected adapter */
    for (d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
    

    iface =		pcap_open_live(d->name,	// name of the device
							 512,			// portion of the packet to capture. 
											// 65536 grants that the whole packet will be captured on all the MACs.
							 1,				// promiscuous mode (nonzero means promiscuous)
							 1,			// read timeout, do not use zero
							 errbuf			// error buffer
							 );
	// open an interface for pcap, device must contain a string identifying the interface/
	//iface= pcap_open(d->name/* ptr to device*/,512/*snaplen*/,0,0/*read timeout in ms*/,NULL /*auth*/, errbuf);

	if(NULL==iface)
	{
			// critical error, abort
			printf("\nError opening adapter\n");
			printf("  error string : %s\n",errbuf);
			exit(0);
	}
}
	

//
// Send packet buffer
//
void nicSend(unsigned int len, unsigned char* packet)
{
	// Write a raw packet to the selected windows adapter.
	if(-1==pcap_sendpacket (iface, packet, len))
		printf("winpcap send failed\n");
}

//
// Send multi chunked packet buffer
//
void nicSendMulti(unsigned char *header, unsigned int hlen, unsigned char *data, unsigned int dlen)
{
	unsigned char	tpacket[2048];
	unsigned int	i;

	// Aggregate header and data into temp buffer
	for(i=0;i<hlen;i++)
		tpacket[i]=header[i];
	for(i=0;i<dlen;i++)
		tpacket[i+hlen]=data[i];

	// Write a raw packet to the selected windows adapter.
	if(-1==pcap_sendpacket (iface, tpacket, hlen+dlen))
		printf("winpcap send failed\n");
}

unsigned char nicIsLinked(void)
{
	return(1);
}


//
// winpcap receive driver
//
unsigned int nicPoll(unsigned int maxlen, unsigned char *packet)
{
	int ret,i;
	struct pcap_pkthdr *header;
	const u_char *tpacket;

	// poll for packet
	ret = pcap_next_ex( iface, &header, &tpacket);

	if(ret>0)
	{
		// A packet has been returned, check size of returned part
		if((header->caplen+4)>maxlen)
			ret=maxlen;
		else
			ret=header->caplen+4;
		//
		// Copy over packet
		//
		for(i=0;i<ret;i++)
			packet[i]=tpacket[i];

	}
	else if (ret<0)
	{
		//printf("winpcap read < 0\n");
		printf("Error reading the packets: %s\n", pcap_geterr(iface));
		ret=0;
	}
	// return length if packet or zero otherwise
	return(ret);
}


void nicGetMacAddress(char * macaddr)
{
	macaddr[0]=0;
	macaddr[1]=0;
	macaddr[2]=0x14;
	macaddr[3]=0;
	macaddr[4]=0;
	macaddr[5]=0;

	return;
}


#if 0
//
// Initialize the PCAP psudo interface
//
void nicInit(void)
{
pcap_if_t *alldevs, *d;
pcap_t *fp;
u_int inum, i=0;
char errbuf[PCAP_ERRBUF_SIZE];
int res;
struct pcap_pkthdr *header;
u_char *pkt_data;

    printf("pktdump_ex: prints the packets of the network using WinPcap.\n");
    printf("   Usage: pktdump_ex [-s source]\n\n"
           "   Examples:\n"
           "      pktdump_ex -s file://c:/temp/file.acp\n"
           "      pktdump_ex -s rpcap://\\Device\\NPF_{C8736017-F3C3-4373-94AC-9A34B7DAD998}\n\n");

    if(argc < 3)
    {

        printf("\nNo adapter selected: printing the device list:\n");
        /* The user didn't provide a packet source: Retrieve the local device list */
        if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
        {
            fprintf(stderr,"Error in pcap_findalldevs_ex: %s\n", errbuf);
            exit(1);
        }
        
        /* Print the list */
        for(d=alldevs; d; d=d->next)
        {
            printf("%d. %s\n    ", ++i, d->name);

            if (d->description)
                printf(" (%s)\n", d->description);
            else
                printf(" (No description available)\n");
        }
        
        if (i==0)
        {
            printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
            return -1;
        }
        
        printf("Enter the interface number (1-%d):",i);
        scanf("%d", &inum);
        
        if (inum < 1 || inum > i)
        {
            printf("\nInterface number out of range.\n");

            /* Free the device list */
            pcap_freealldevs(alldevs);
            return -1;
        }
        
        /* Jump to the selected adapter */
        for (d=alldevs, i=0; i< inum-1 ;d=d->next, i++);
        
        /* Open the device */
        if ( (fp= pcap_open(d->name,
                            100 /*snaplen*/,
                            PCAP_OPENFLAG_PROMISCUOUS /*flags*/,
                            20 /*read timeout*/,
                            NULL /* remote authentication */,
                            errbuf)
                            ) == NULL)
        {
            fprintf(stderr,"\nError opening adapter\n");
            return -1;
        }
    }
    else 
    {
        // Do not check for the switch type ('-s')
        if ( (fp= pcap_open(argv[2],
                            100 /*snaplen*/,
                            PCAP_OPENFLAG_PROMISCUOUS /*flags*/,
                            20 /*read timeout*/,
                            NULL /* remote authentication */,
                            errbuf)
                            ) == NULL)
        {
            fprintf(stderr,"\nError opening source: %s\n", errbuf);
            return -1;
        }
    }
}
#endif

