#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "net.h"

void makeInitPacket(char *fileName, int iid, Lot lot) {
  FILE *fp = fopen(fileName, "w");
	if(fp == NULL) {
		printf("error saving JSON!\n");
	}
  fprintf(fp, "{");
  fprintf(fp, "\"IID\":%d,", iid);
  fprintf(fp, "\"LOTID\":%d,", lot.id);
  fprintf(fp, "\"SPACES\":[");
  for(int j = 0; j < lot.spaceCount-1; j++) {
    fprintf(fp, "%d,", lot.space[j].isOccupied);
  }
  fprintf(fp, "%d", lot.space[lot.spaceCount-1].isOccupied);
  fprintf(fp, "]");
  fprintf(fp ,"}");

  fclose(fp);

}

void makeUpdatePacket(char *fileName, int iid, int isOcc, int lotID, int spotID, int popularity, unsigned long timetake) {
	FILE *fp = fopen(fileName, "w");
	if(fp == NULL) {
		printf("error saving JSON!\n");
	}
  uint64_t currentTime = time(NULL);
  fprintf(fp, "{");
  fprintf(fp, "\"IID\":%d,", iid);
  fprintf(fp, "\"TIME\":%ld,", currentTime);
  fprintf(fp, "\"STATUS\":%d,",isOcc);
  fprintf(fp, "\"LOTID\":%d,", lotID);
  fprintf(fp, "\"SPACEID\":%d,", spotID);
  fprintf(fp, "\"TIMETAKEN\":%ld,", timetake);
  fprintf(fp, "\"POPULARITY\":%d", popularity);
  fprintf(fp ,"}");

  fclose(fp);
  
} 

void error(const char *msg) { 
  perror(msg); 
  exit(0); 
}

void sendPacket(char *JSONFile, char *path, int iid) {
  
  FILE *fp = fopen(JSONFile, "r");
  if(fp == NULL) {
    printf("error reading JSON!\n");
  }

  char *content = malloc(sizeof(char)*1024);
  //printf("lol\n");
  while(fscanf(fp, "%s", content) != EOF)
    fclose(fp);
 // printf("%s\n", content);
  //char *authraw = "YCPAdmin:changeme7";
  char *auth;
  if(iid == 0)
    auth = "WUNQQWRtaW46Y2hhbmdlbWU3";
  else if(iid == 1)
    auth = "cm9vdH41NzpoYXNobWFuLSYmLTkw";
  else if(iid == 2)
    auth = "d2FsbWFydFByaXY6dGFyZ2V0RmtuU3Vja3M=";
  else if(iid == 3)
    auth = " eW9ya0NpdHlEVDpONio2NXIzSClsbmZlOTI=";
  else if(iid == 4)
    auth = "YXJkbm8tNzpBRE5PLXRlc3Q=";
   
  int portno = 80;
  char *host = "10.64.128.73";
  char *meathod = "POST";
  char *contentType = "Content-Type: application/json";
  struct hostent *server;
  struct sockaddr_in serv_addr;
  int sockfd, bytes, sent, received, total, message_size;
  char *message, response[4096];
  /* How big is the message? */
  message_size=0;
  message_size+=strlen("%s %s HTTP/1.1\r\n");
  message_size+=strlen(meathod);
  message_size+=strlen(path);
  message_size+=strlen(contentType)+strlen("\r\n");
  message_size+=strlen("Host: %s\r\n");
  message_size+=strlen(host);
  message_size+=strlen(auth);
  message_size+=strlen("Connection: close\r\n");
  message_size+=strlen("Content-Length: %lu\r\n")+21;
  message_size+=strlen("\r\n"); 
  message_size+=strlen(content);                        /* body              */
  /* allocate space for the message */
  message=malloc(message_size);
  /* fill in the parameters */
  sprintf(message,"%s %s HTTP/1.1\r\n",
  strlen(meathod)>0?meathod:"POST", strlen(path)>0?path:"/");                       /* path              */
  strcat(message,contentType);
  strcat(message,"\r\n");
  sprintf(message+strlen(message),"Host: %s\r\n", host);        /* host              */
  sprintf(message+strlen(message),"Authorization: basic %s\r\n", auth);        /* host              */
  sprintf(message+strlen(message),"Connection: close\r\n"); /* connection header */
  sprintf(message+strlen(message),"Content-Length: %lu\r\n",strlen(content));
  strcat(message,"\r\n");                                   /* blank line        */
  strcat(message,content);                              /* body              */
  /* What are we going to send? */
  printf("Request:\n%s\n",message);
  /* create the socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  /* lookup the ip address */
  server = gethostbyname(host);
  if (server == NULL) error("ERROR no such host");
  /* fill in the structure */
  memset(&serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);
  /* connect the socket */
  printf("1. here\n");
  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    error("ERROR connecting");

  printf("2. here\n");
  /* send the request */
  total = strlen(message);
  sent = 0;

  do {
    bytes = write(sockfd,message+sent,total-sent);
    if (bytes < 0)
      error("ERROR writing message to socket");
    if (bytes == 0)
      break;
    sent+=bytes;
  } while (sent < total);
  /* receive the response */
  memset(response,0,sizeof(response));
  total = sizeof(response)-1;
  received = 0;
  do {
    bytes = read(sockfd,response+received,total-received);
    if (bytes < 0)
      error("ERROR reading response from socket");
    if (bytes == 0)
      break;
    received+=bytes;
  } while (received < total);

  if (received == total)
    error("ERROR storing complete response from socket");

  close(sockfd);

  printf("Response:\n%s\n",response);

  free(content);
  free(message);
}
