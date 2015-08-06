/*
 * File    - utility_nw.c
 * Process - Infusion Pump Firmware
 * Module  - Core Process
 * Author  - Shahzeb Ihsan
 */

/*
 * Standard header files
 */
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/*
 * Local header files
 */
#define __USE_STDINT          (1)
#include "utility.h"

/*
 * Local scope variables
 */
static struct sockaddr_in udp_server;
static struct sockaddr_in udp_client;
static struct sockaddr_in tcp_server;
static struct sockaddr_in tcp_client;
static socklen_t len_client;
static bool_t udp_is_server = FALSE;
static bool_t tcp_is_server = FALSE;

/*
 * Function     - network_connect()
 *
 * Arguments    - <server_sock> Pointer to the server socket descriptor
 *                <client_sock> Pointer to client socket descriptor (NULL for TCP client)
 *                <server_ip> Server IP address
 *                <server_port> Server port number
 *                <server> Set to TRUE if this machine is the server
 *
 * Return Value - TRUE if successful, FALSE if otherwise
 */
bool_t network_connect(sint32_t * server_sock, sint32_t * client_sock, char * server_ip, uint16_t server_port, bool_t server)
{
   char error_buff[256];

   tcp_is_server = server;

   // Open server socket
   *server_sock = socket(AF_INET, SOCK_STREAM, 0);
   if(*server_sock < 0)
   {
      perror("\r\nOpening stream socket");
      return (FALSE);
   }

   // Setup server address
   tcp_server.sin_family = AF_INET;

   if((NULL == server_ip) && tcp_is_server)
      tcp_server.sin_addr.s_addr = htonl(INADDR_ANY);
   else
      inet_aton(server_ip, &tcp_server.sin_addr);

   tcp_server.sin_port = htons(server_port);

   // Connect to remote server if current application not a server
   if(!tcp_is_server)
   {
      if(connect(*server_sock, (struct sockaddr *)&tcp_server, sizeof(tcp_server)) == -1)
      {
         snprintf(error_buff, sizeof(error_buff), "\r\nConnecting to server %s:%d", server_ip, server_port);
         perror(error_buff);
         return (FALSE);
      }
   }
   // This is a server, listen on the specified IP:PORT
   else
   {
      // Bind server address
      if(bind(*server_sock, (struct sockaddr *)&tcp_server, sizeof(tcp_server)) < 0)
      {
         snprintf(error_buff, sizeof(error_buff), "\r\nBinding socket to %s:%d", server_ip, server_port);
         perror(error_buff);
         return (FALSE);
      }

      // Listen for an incoming connection
      if(listen(*server_sock, 0) == -1)
      {
         snprintf(error_buff, sizeof(error_buff), "\r\nListening on %s:%d", server_ip, server_port);
         perror(error_buff);
         return (FALSE);
      }

      printf("\r\nListening on %s %d\n", inet_ntoa(tcp_server.sin_addr), ntohs(tcp_server.sin_port));

      // Accept the connection
      len_client = sizeof(tcp_client);
      *client_sock = accept(*server_sock, (struct sockaddr *) &tcp_client, &len_client);
      if(*client_sock < 0)
      {
         snprintf(error_buff, sizeof(error_buff), "\r\nAccepting connection from %s", inet_ntoa(tcp_client.sin_addr));
         perror(error_buff);
         return (FALSE);
      }

      printf("Connection accepted\n");
   }

   return (TRUE);
}

/*
 * Function     - network_send()
 *
 * Arguments    - <sock_desc> Pointer to the socket descriptor
 *                <data_buff> Data buffer to send
 *                <data_len> Number of bytes to send
 *
 * Return Value - Number of bytes sent, -1 if operation failed
 */
sint32_t network_send(sint32_t * sock_desc, void * data_buff, uint32_t data_len)
{
   printf("Sending data\n");
   return send(*sock_desc, data_buff, data_len, 0);
}

/*
 * Function     - network_recv()
 *
 * Arguments    - <sock_desc> Pointer to the socket descriptor
 *                <data_buff> Pointer to buffer in which to store received data
 *                <data_len> Size of buffer (maximum number of bytes that can be received)
 *
 * Return Value - Number of bytes received, -1 if operation failed
 */
sint32_t network_recv(sint32_t * sock_desc, void * data_buff, uint32_t data_len)
{
   printf("Waiting for data\n");
   return recv(*sock_desc, data_buff, data_len, 0);
}

/*
 * Function     - network_close()
 *
 * Arguments    - <sock_desc> Pointer to the socket descriptor
 *
 * Return Value - 0 for success, -1 if operation failed
 */
sint32_t network_close(sint32_t * sock_desc)
{
   return close(*sock_desc);
}

/*
 * Function     - udp_setup()
 *
 * Arguments    - <sock_desc> Pointer to the socket descriptor
 *                <server_ip> Server IP address
 *                <server_port> Server port number
 *                <server> Set to TRUE if this machine is the server
 *
 * Return Value - TRUE if successful, FALSE if otherwise
 */
bool_t udp_setup(sint32_t * sock_desc, char * server_ip, uint16_t server_port, bool_t server)
{
   *sock_desc = socket(AF_INET, SOCK_DGRAM, 0);
   udp_is_server = server;

   if(*sock_desc < 0)
   {
      perror("\r\nCreating UDP socket");
      return (FALSE);
   }

   memset(&udp_server, 0, sizeof(udp_server));
   udp_server.sin_family = AF_INET;

   if((NULL == server_ip) && udp_is_server)
      udp_server.sin_addr.s_addr = htonl(INADDR_ANY);
   else
      inet_aton(server_ip, &udp_server.sin_addr);

   udp_server.sin_port = htons(server_port);

   if(udp_is_server)
   {
      if(bind(*sock_desc, (struct sockaddr *)&udp_server, sizeof(udp_server)) < 0)
      {
         perror("\r\nBinding socket");
         return (FALSE);
      }
   }

   return (TRUE);
}

/*
 * Function     - udp_send()
 *
 * Arguments    - <sock_desc> Pointer to the socket descriptor
 *                <data_buff> Data buffer to send
 *                <data_len> Number of bytes to send
 *
 * Return Value - Number of bytes received, -1 if operation failed
 */
sint32_t udp_send(sint32_t * sock_desc, void * data_buff, uint32_t data_len)
{
   return (sendto(*sock_desc, data_buff, data_len, 0,
                  (struct sockaddr *) &udp_server,
                  sizeof(udp_server)));

}

/*
 * Function     - udp_receive()
 *
 * Arguments    - <sock_desc> Pointer to the socket descriptor
 *                <data_buff> Buffer for receiving data
 *                <data_len> Number of bytes to receive
 *
 * Return Value - Number of bytes received, -1 if operation failed
 */
sint32_t udp_receive(sint32_t * sock_desc, void * data_buff, uint32_t data_len)
{
   sint32_t recv_len;
   sint32_t client_len = sizeof(udp_client);

   recv_len = recvfrom(*sock_desc, data_buff, data_len, 0,
                       (struct sockaddr *)&udp_client, &client_len);

   return recv_len;
}
