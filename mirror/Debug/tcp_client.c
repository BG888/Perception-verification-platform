#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "errno.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        /* code */
        return -1;
    }
    
    int sockfd = socket(PF_INET,SOCK_STREAM,0);
    if (sockfd == -1) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    if(connect(sockfd,(struct sockaddr *)&addr,sizeof(addr)) == -1)
    {
        perror("connect");
        return -1;
    }

    for (;;)
    {
        /* code */
          printf("> ");
          char buf[1024];
          fgets(buf, sizeof(buf) / sizeof(buf[0]),stdin);
           if(!strcmp(buf,"!\n")) 
           {
               break;
           }

           send(sockfd,buf,strlen(buf) * sizeof(buf[0]), 0);

           ssize_t rb = recv(sockfd,buf,sizeof(buf) - sizeof(buf[0]),0);

           if(rb == 0)
           {
                printf("客户机：服务器已关闭\n");
                break;
           }    

          buf[rb / sizeof(buf[0])] = '\0';
          printf("< %s", buf);
    }
     printf("客户机：关闭套接字\n");

     close(sockfd);

    close(sockfd);

}

