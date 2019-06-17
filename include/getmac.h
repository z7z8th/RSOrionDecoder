#ifndef __GETMAC_H__
#define __GETMAC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __unix__
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <linux/if.h>

int getmac(char* mac, int len) {
    char macaddr[128];

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return -10;
    }

    struct ifconf ifc;
    char buf[1024];
    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    int iRet = ioctl(sock, SIOCGIFCONF, &ifc);
    if (iRet != 0) {
        return iRet;
    }

    int cnt = ifc.ifc_len / sizeof(struct ifreq);
    //printf("ifc_cnt=%d\n", cnt);
    if (cnt == 0) {
        return -20;
    }

    struct ifreq ifr;
    for (int i = 0; i < cnt; ++i) {
        strcpy(ifr.ifr_name, ifc.ifc_req[i].ifr_name);
        //printf("ifr_name: %s\n", ifr.ifr_name);
        iRet = ioctl(sock, SIOCGIFFLAGS, &ifr);
        if (iRet != 0) {
            return iRet;
        }
        if (ifr.ifr_flags & IFF_LOOPBACK)   continue;
        iRet = ioctl(sock, SIOCGIFHWADDR, &ifr);
        if (iRet != 0) {
            return iRet;
        }
        snprintf(macaddr, sizeof(macaddr), "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
            (unsigned char)ifr.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
        //printf("macaddr: %s\n", macaddr);
        strncpy(mac, macaddr, len);
        return 0;
    }

    return -30;
}
#elif defined(_WIN32)
#include <Winsock2.h>
#include <IphlpApi.h>
#ifdef _MSC_VER
#pragma comment(lib,"Iphlpapi")
#endif
int getmac(char* mac, int len) {
    PIP_ADAPTER_ADDRESSES pAddrs = NULL;
    ULONG family = AF_INET;
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG buflen = 0;
    GetAdaptersAddresses(family, flags, NULL, pAddrs, &buflen);
    pAddrs = (PIP_ADAPTER_ADDRESSES)malloc(buflen);
    if (pAddrs == NULL) {
        return -10;
    }
    if (GetAdaptersAddresses(family, flags, NULL, pAddrs, &buflen) != 0) {
        free(pAddrs);
        return -20;
    }
    PIP_ADAPTER_ADDRESSES pCur = pAddrs;
    while (pCur) {
        snprintf(mac, len, "%02x:%02x:%02x:%02x:%02x:%02x",
            pCur->PhysicalAddress[0],
            pCur->PhysicalAddress[1],
            pCur->PhysicalAddress[2],
            pCur->PhysicalAddress[3],
            pCur->PhysicalAddress[4],
            pCur->PhysicalAddress[5]);

        //printf("mac:%s\n", mac);
        //printf(pCur->AdapterName);
        //printf("\n");
        //wprintf(pCur->Description);
        //printf("\n-----------------------\n\n"); 
        break;
        pCur = pCur->Next;
    }

    free(pAddrs);
    return 0;
}
#else
int getmac(char* mac, int len) {
    return -10;  // unimplemented
}
#endif

#endif // __GETMAC_H__

