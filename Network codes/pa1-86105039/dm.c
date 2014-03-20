/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int getFileSize(FILE *input);

#define PORT 80 // the port client will be connecting to
long int file_size;
#define MAXDATASIZE 100000 // max number of bytes we can get at once

int getFileSize(FILE *input)
{
    long int fileSizeBytes;
    fseek(input, 0, SEEK_END);
    fileSizeBytes = ftell(input);
    fseek(input, 0, SEEK_SET);
    return fileSizeBytes;
}

int main(int argc, char *argv[])
{
	long int start = 0;
	long int end;
	int part;
	int ch_num = 4;
	int forked = 0;
	if (argc == 3) {
	   ch_num = atoi(argv[2]);
	}
	char* url = argv[1]+7;
	char *tmp = strstr(url,"/");
	char* file_Address = tmp+1;
	char* t=tmp;
	char* t2;
	while((t2=strstr(t+1,"/")) != NULL){
		t = t2;
	}
	char* file_name = t+1;
	*tmp = '\0';
	char* serverName = url;

	int resume = 0;
	char dmf_name[100];
	strcpy(dmf_name,file_name);
	FILE* exist = fopen(file_name,"r");
	FILE* dm_exist = fopen(strcat(dmf_name,".dm"),"r");
	if(exist != NULL){
		fclose(exist);
		printf("File already exists.\n");
		return 0;
	}
	else if(dm_exist != NULL){
		printf("Resuming File download ....\n");
		resume = 1;
		fclose(dm_exist);
	}
	pid_t pid;
	if(resume == 0){
	printf("addr : %s, host: %s file_name: %s \n",file_Address,serverName,file_name);

	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("client: socket");
		return -1;
	}
	struct hostent* serverHost = gethostbyname(serverName);
	if (serverHost == NULL) {
		herror("client: gethostbyname");
		return -1;
	}
	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	memcpy(&(serverAddr.sin_addr), serverHost->h_addr, serverHost->h_length);
	serverAddr.sin_port = htons(PORT);
	printf("client: host name '%s' resolved to '%s'\n", serverName, inet_ntoa(serverAddr.sin_addr));
	int ret = connect(sockfd, ((struct sockaddr *)&serverAddr), sizeof(struct sockaddr));
	if (ret != 0) {
		close(sockfd);
		perror("client: connect");
		return -1;
	}
	printf("client %d :successfully connected to %s:%d\n",getpid(), serverName, PORT);
	char get_mes[100]="HEAD /";
	strcat(get_mes,file_Address);
	strcat(get_mes," http/1.1\r\nHost: ");
	strcat(get_mes,serverName);
	strcat(get_mes,"\r\n\r\n");
	char *sendBuf = get_mes;
	do {
		ret = write(sockfd, sendBuf, strlen(sendBuf));
		if (ret > 0) {
			sendBuf += ret;
		}
	} while (ret > 0 && strlen(sendBuf) > 0);
	if (ret < 0) {
		perror("client: write");
		close(sockfd);
		return -1;
	}
	char message2[MAXDATASIZE];
	while ((ret = read(sockfd, message2, MAXDATASIZE-1)) > 0) {
		char* length_ptr = strstr(message2,"Content-Length: ");
		char* end = strstr(length_ptr,"\r\n");
		*end = '\0';
		file_size = atol(length_ptr + 16);
		printf("file_size is :%ld: \n\n",file_size);
	}
	if (ret < 0) {
		perror("server: read");
	}
	char dmfile[100];
	strcpy(dmfile,file_name);
	FILE* dm = fopen(strcat(dmfile,".dm"),"w");
	fprintf(dm,"%d\n",ch_num);
	fflush(dm);
	long chunk = file_size/ch_num;
	int i;
	for(i=0;i<ch_num;i=i+1){
		pid = fork();
		printf("I forked.\n");
		forked = 1;
		if(i== (ch_num-1))
			end = file_size - 1;
		else
			end = start + chunk -1;
		if(pid<0){
			printf("ERROR !!");
			exit(1);
		}
		else if(pid==0){
			part = i;
			break;
		}
		fprintf(dm,"%s.part%d %ld %ld\n",file_name,i,start,end);
		fflush(dm);
		if(i != (ch_num -1))
			start = start + chunk;
	}
	fclose(dm);
	}else{	//Resume ...
		char dmfile[100];
		strcpy(dmfile,file_name);
		FILE* dm = fopen(strcat(dmfile,".dm"),"r");
		int ch_n;
		fscanf(dm,"%d\n",&ch_n);
		int i;
		for(i=0;i<ch_n;i=i+1){
			char fle_name[100];
			long int start2,end2;
			fscanf(dm,"%s %ld %ld\n",fle_name,&start2,&end2);
			long int size = 0;
			FILE* f = fopen(fle_name,"rb");
			if(f!=NULL)
				size = getFileSize(f);
			if((end2-start2+1) > size){
				pid = fork();
				forked = 1;
				printf("I forked in resume.\n");
				if(pid<0){
					printf("ERROR !!");
					exit(1);
				}
				else if(pid==0){
					start = start2 + size;
					end = end2;
					printf("start is: %ld , end is: %ld .",start,end);
					part = i;
					break;
				}
			}
		}
		fclose(dm);
	}

	////////////////////////##################################/////////////////////////
	////////////////////////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$////////////////////////
	if((pid == 0) && forked){	//child goes to download.
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		perror("client: socket");
		return -1;
	}

	struct hostent* serverHost = gethostbyname(serverName);

	if (serverHost == NULL) {
		herror("client: gethostbyname");
		return -1;
	}

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	memcpy(&(serverAddr.sin_addr), serverHost->h_addr, serverHost->h_length);
	serverAddr.sin_port = htons(PORT);

	printf("client: host name '%s' resolved to '%s'\n", serverName, inet_ntoa(serverAddr.sin_addr));

	int ret = connect(sockfd, ((struct sockaddr *)&serverAddr), sizeof(struct sockaddr));

	if (ret != 0) {
		close(sockfd);
		perror("client: connect");
		return -1;
	}

	printf("client %d :successfully connected to %s:%d\n",getpid(), serverName, PORT);
	char get_mes[100]="GET /";
	strcat(get_mes,file_Address);
	strcat(get_mes," http/1.1\r\nHost: ");
	strcat(get_mes,serverName);
	strcat(get_mes,"\r\nRange: bytes=");
	char start_str[20];
	sprintf(start_str,"%ld",start);
	char end_str[20];
	sprintf(end_str,"%ld",end);
	printf("Range for %d is : %s-%s\n",getpid(),start_str,end_str);
	strcat(get_mes,start_str);
	strcat(get_mes,"-");
	strcat(get_mes,end_str);
	strcat(get_mes,"\r\n\r\n");

	char *sendBuf = get_mes;

	do {
		ret = write(sockfd, sendBuf, strlen(sendBuf));
		if (ret > 0) {
			sendBuf += ret;
		}
	} while (ret > 0 && strlen(sendBuf) > 0);
	if (ret < 0) {
		perror("client: write");
		close(sockfd);
		return -1;
	}
	FILE * fp;
	char part_str[10];
	sprintf(part_str,"%d",part);
	if((fp = fopen(strcat(strcat(file_name,".part"),part_str),"ab")) == NULL){
		printf("Can not open the file.\n");
		exit(1);
	}
	int done = 0;
	char message2[MAXDATASIZE];
	int counter = 0;
	while ((ret = read(sockfd, message2, MAXDATASIZE-1)) > 0) {
		if(!done){
			char* body = strstr(message2,"\r\n\r\n") + 4;
			fwrite(body,sizeof(char),(ret - (body - message2)) ,fp);
			done = 1;
		}
		else{
			fwrite(message2,sizeof(char),ret,fp);
		}
		fflush(fp);
		counter = counter + 1;
//		if(counter == 10)
//			break;
	}
	if (ret < 0) {
		perror("server: read");
	}
	fclose(fp);
	}	//parent goes to wait and finalize if download completed.
	else{
		char dmfile[100];
		strcpy(dmfile,file_name);
		FILE* dm = fopen(strcat(dmfile,".dm"),"r");
		int ch_n;
		fscanf(dm,"%d\n",&ch_n);
		int w;
		for(w=0;w<ch_n;w=w+1){
			printf("\nwaiting !!!\n");
			wait(NULL);
		}
		int complete = 1;
		int i;
		for(i=0;i<ch_n;i=i+1){
			char fle_name[100];
			long int start2,end2;
			fscanf(dm,"%s %ld %ld\n",fle_name,&start2,&end2);
			long int sz = 0;
			FILE* f = fopen(fle_name,"rb");
			if(f!=NULL)
				sz = getFileSize(f);
			if((end2-start2+1) != sz){
				complete = 0;
				break;
			}
		}
		fclose(dm);
		if(complete){ //goes to finalize output file.
			FILE* final = fopen(file_name,"wb");
			if(final!=NULL)
				printf("it is complete and ready to finalize.\n");
			char dmfile[100];
			strcpy(dmfile,file_name);
			FILE* dm = fopen(strcat(dmfile,".dm"),"r");
			int ch_n;
			fscanf(dm,"%d\n",&ch_n);
			int i;
			for(i=0;i<ch_n;i=i+1){
				char fle_name[100];
				long int start2,end2;
				fscanf(dm,"%s %ld %ld\n",fle_name,&start2,&end2);
//				printf("read data from dm file for finalize.\n");
				FILE* r = fopen(fle_name,"rb");
				int ret;
				char data[MAXDATASIZE];
				while ((ret = fread(data, sizeof(char), MAXDATASIZE-1,r)) > 0) {
					fwrite(data,sizeof(char),ret,final);
					fflush(final);
				}
				if (ret < 0) {
					perror("file: read");
				}
				fclose(r);
				remove(fle_name);
			}
			fclose(dm);
			remove(dmfile);
			fclose(final);
		}
	}
		/////////////////////////////////////////////////////
		////////////////////////////////////////////////////
	printf("client %d : connection successfully finished.\n",getpid());
	return 0;
}
