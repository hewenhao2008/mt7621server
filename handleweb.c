#include "head.h"
#include "tool.h"
#include "table.h"

typedef struct{
	int moduleId;
	int req;
	char data[128];
}Request;

#define REQ_MODULE_EXIST	1
#define REQ_IF_EXIST		2
#define REQ_MAC				3
#define REQ_IP				4

int handleUnixdomainSocket( int fd)
{
	Request r;
	int id;
	client *pclient;
	int ret;

	ret=read(fd, &r, sizeof(Request));
	if(ret!= sizeof(Request))
		return -1;
	id = r.moduleId;
	ret = table_get_by_id(client_table, &pclient, id);
	printf("module %d request\n", r.moduleId);
	if(ret<0 || pclient==NULL){
		bzero(r.data, sizeof(r.data));
		printf("module %d not exist\n", id);
		strcpy(r.data, "0");
	}
	else{
		switch(r.req){
			case REQ_MODULE_EXIST:
				deb_print("Request: REQ_MODULE_EXIST\n");
				bzero(r.data, sizeof(r.data));
				strcpy(r.data, "1");
				break;

			case REQ_IF_EXIST:
				deb_print("Request: REQ_IF_EXIST\n");
				deb_print("r.data:%s\n", r.data);
				if( !strcmp(r.data, "ra0") ){
					bzero(r.data, sizeof(r.data));
					sprintf(r.data, "%d", pclient->mdInfo.state_24g);
				}
				else if(!strcmp(r.data, "rai0")){
					bzero(r.data, sizeof(r.data));
					sprintf(r.data, "%d", pclient->mdInfo.state_5g);			
				}else{
					bzero(r.data, sizeof(r.data));
					sprintf(r.data, "0");
				}				
				break;

			case REQ_MAC:
				deb_print("Request: REQ_MAC\n");
				if( !strcmp(r.data, "ra0") ){
					bzero(r.data, sizeof(r.data));
					memcpy(r.data, pclient->mdInfo.mac_24g, strlen(pclient->mdInfo.mac_24g)+1 );				
				}else{
					bzero(r.data, sizeof(r.data));
					memcpy(r.data, pclient->mdInfo.mac_5g, strlen(pclient->mdInfo.mac_5g)+1 );			
				}				
				break;
			case REQ_IP:
					deb_print("Request: REQ_IP\n");
					bzero(r.data, sizeof(r.data));
					memcpy(r.data, inet_ntoa(pclient->addr.sin_addr), 
								1+strlen(inet_ntoa(pclient->addr.sin_addr)));
					deb_print("ip:%s\n", inet_ntoa(pclient->addr.sin_addr) );
				break;

			default:
				bzero(r.data, sizeof(r.data));
				deb_print("unknow request\n");
				break;
		}
	}
	printf("response:%s\n",r.data);
	write(fd, &r, sizeof(Request));
	close(fd);

	return 0;		
}
