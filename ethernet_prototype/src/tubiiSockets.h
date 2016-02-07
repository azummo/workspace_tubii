/*
 * tubiiSockets.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */

#ifndef TUBIISOCKETS_H_
#define TUBIISOCKETS_H_

#include "tubiiUtil.h"

int MakeSocket()
{
  printf("Making socket\n");
  int hServerSocket;  /* handle to socket */

  hServerSocket=socket(AF_INET,SOCK_STREAM,0);
  if(hServerSocket == SOCKET_ERROR)
  {
    printf("\nCould not make a socket\n");
    return -1;
  }
  else return hServerSocket;
}

int BindSocket(int hServerSocket, struct sockaddr_in Address)
{
  int nAddressSize=sizeof(struct sockaddr_in);

  printf("Binding to port %d\n",ntohs(Address.sin_port));
  if(bind(hServerSocket,(struct sockaddr*)&Address,sizeof(Address)) == SOCKET_ERROR)
  {
    printf("\nCould not connect to host\n");
    return -1;
  }

  ///  get port number
  getsockname( hServerSocket, (struct sockaddr *) &Address,(socklen_t *)&nAddressSize);
  //printf("Opened socket as fd (%d) on port (%d) for stream i/o\n",hServerSocket, ntohs(Address.sin_port) );

  printf("Making a listen queue of %d elements\n\n",QUEUE_SIZE);
  /// establish listen queue
  if(listen(hServerSocket,QUEUE_SIZE) == SOCKET_ERROR)
  {
    printf("\nCould not listen\n");
    return -2;
  }

  return 0;
}

struct sockaddr_in GetAddress(int nHostPort)
{
  struct sockaddr_in Address; /* Internet socket address stuct */
  int nAddressSize=sizeof(struct sockaddr_in);

  /// fill address struct
  Address.sin_addr.s_addr=INADDR_ANY;
  Address.sin_port=htons(nHostPort);
  Address.sin_family=AF_INET;
  return Address;
}

int ConnectSocket(int hServerSocket, struct sockaddr_in Address)
{
  int nAddressSize=sizeof(struct sockaddr_in);

  printf("Waiting for a connection\n\n");
  /// Get the connected socket
  int hSocket=accept(hServerSocket,(struct sockaddr*)&Address,(socklen_t *)&nAddressSize);
  return hSocket;
}

int HandShaking(int hSocket)
{
  char pBuffer[BUFFER_SIZE];

  /// Note: number returned by read() and write() is the number of bytes read or written, with -1 being that an error occured
  /// Read commands for tubii from socket into buffer
  printf("Receiving ");
  int readAmount= read(hSocket,pBuffer,BUFFER_SIZE);
  printf("\"%s\" from client\n",pBuffer);

  /// Send success message to client
  strcpy(pBuffer,"Connection Successful!");
  printf("Sending \"%s\" to client\n",pBuffer);
  int writeAmount= write(hSocket,pBuffer,strlen(pBuffer)+1);

  if( readAmount==-1 || writeAmount==-1 )
  {
  	printf("Problem read and writing from client\n");
  	return -1;
  }
  else return 0;
}

int RecieveCommand(int hSocket, char* tubBuffer, char** comm, char** arg)
{
  /// Read commands for tubii from socket into buffer
  printf("\nReading input ");
  read(hSocket,tubBuffer,BUFFER_SIZE);
  printf("\"%s\" from client\n",tubBuffer);

  /// Parse these into commands and arguments
  *comm = strtok(tubBuffer," -");
  *arg = strtok(NULL," -");

  printf("%s\t",*comm);
  printf("%s\n",*arg);

  return 0;
}

#endif /* TUBIISOCKETS_H_ */
