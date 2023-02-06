#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#include "TCPServer.h"
#include "LinkList.h"

#define _BACKLOG_ 10

/**
 * @brief     tcp服务端数据处理函数
 * @details
 * @param	  void* arg		参数
 * @return    int 返回初始化结果
 *     - 0  		成功
 *     - ohter      失败
 */
static void *fun(void* arg)
{
	struct NetParaType* pclient_sock = (struct NetParaType*)arg;
	struct clientInfoType* node = FindNode(pclient_sock->ClientInfo, 1);

	while(1)
	{
		char buf[1024];
		memset(buf, '\0', sizeof(buf));
		int len = read(node->accFd, buf, sizeof(buf));
		if (len < 0)
		{
			printf("socket[%d] port: %d read date from client failure: %s\n", node->accFd, pclient_sock->localPort, strerror(errno));
			break;
		}
		else if (len == 0)
		{
			if(NULL != pclient_sock->iCBFunc->tcpDisconnected)
			{
				pclient_sock->iCBFunc->tcpDisconnected(pclient_sock, node);
			}
			printf("socket[%d] port: %d get Disconnected\n", node->accFd, pclient_sock->localPort);
			break;
		}
		else if (len > 0)
		{
			printf("socket[%d] port: %d read %d Byte data from client\n", node->accFd, pclient_sock->localPort, len);

			if(NULL != pclient_sock->iCBFunc->tcpRecvDisposal)
			{
				pclient_sock->iCBFunc->tcpRecvDisposal(pclient_sock, node, (UINT8*)buf, len);
			}
		}

	}

    close(node->accFd);
    return NULL;
}
#if 0
/**
 * @brief     TCP服务器启动侦听
 * @details
 * @param	  UINT16 port, 			侦听端口号
 * 			  RecvCBType recvCB		数据接收回调函数
 * @return    int 返回初始化结果
 *     - 0  		成功
 *     - ohter      失败
 */
int TCPServerRun1(UINT16 port, struct cbFuncType* iCBFunc)
{
	struct NetParaType netPara;
	int on = 1;
	/*新建客户端信息链表*/
	netPara.ClientInfo = CreatLink(0, sizeof(struct clientInfoType));

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock<0)
    {
        printf("socket()\n");
    }
    //避免上次结束程序时，端口未被及时释放的问题
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    struct sockaddr_in server_socket;
    struct sockaddr_in socket;
    pthread_t thread_id;
    bzero(&server_socket, sizeof(server_socket));
    server_socket.sin_family = AF_INET;
    server_socket.sin_addr.s_addr = htonl(INADDR_ANY);
    server_socket.sin_port = htons(port);
    if(bind(sock, (struct sockaddr*)&server_socket, sizeof(struct sockaddr_in)) < 0)
    {
    	perror("bind");
        close(sock);
        return 1;
    }
    if(listen(sock, _BACKLOG_)<0)
    {
    	perror("listen()\n");
        close(sock);
        return 2;
    }
    netPara.localPort = port;
    printf("TCP Server start success, port: %d\n", port);
    for(;;)
    {
        socklen_t len = sizeof(struct sockaddr);
        bzero(&socket, sizeof(socket));
        int client_sock = accept(sock, (struct sockaddr *)&socket, &len);
        if(client_sock < 0)
        {
            printf("accept()\n");
            return 3;
        }

        InsertNode(netPara.ClientInfo, 0, sizeof(struct clientInfoType));
        struct clientInfoType* node = FindNode(netPara.ClientInfo, 1);
        if(NULL != node)
        {
        	memset(node->ip,'\0',sizeof(node->ip));
        	inet_ntop(AF_INET, &socket.sin_addr.s_addr, node->ip, sizeof(node->ip));;
        	node->port = ntohs(socket.sin_port);
        	node->accFd = client_sock;
        	node->iCBFunc = iCBFunc;
            printf("get connect, ip is %s\n", node->ip);
            printf("port = %d\n", node->port);
        }

        pthread_create(&thread_id, NULL, (void *)fun, &netPara);
        pthread_detach(thread_id);
    }

    close(sock);
    return 0;
}
#endif

/**
 * @brief     TCP服务器启动侦听
 * @details
 * @param	  void* argv struct NetParaType类型指针
 * @return
 *     - 0  		成功
 *     - ohter      失败
 */
void* TCPServerRun(void* argv)
{
	int					listen_fd = -1;
	int					client_fd = -1;
	int					on = 1;
	struct sockaddr_in	servaddr;
	struct sockaddr_in	cliaddr;
	socklen_t			cliaddr_len;
	int					server_port;
	int					backlog = 10;
	struct NetParaType* pNet = argv;
//	int					rv = -1;
//	char				buf[1024];

//	//用来确认程序执行的格式是否正确，不正确则退出并提醒用户
//	if (argc < 2)
//	{
//		printf("Program usage: %s [Port]\n", argv[0]);
//		return -1;
//	}

	//将端口参数赋给参数变量
	//由于命令行传参进来是字符串类型，所以需要atoi转换为整型
	server_port = pNet->localPort;

	/*
	 * socket()，创建一个新的sockfd
	 * 指定协议族为IPv4
	 * socket类型为SOCK_STREAM（TCP）
	 */
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
		printf("create socket failure: %s\n", strerror(errno));
		return -2;
	}
	printf("create socket[%d] success\n", listen_fd);

	//避免上次结束程序时，端口未被及时释放的问题
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	/*
	 * bind()，将服务器的协议地址绑定到listen_fd
	 */
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(server_port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(listen_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		printf("socket[%d] bind port[%d] failure: %s\n", listen_fd, server_port, strerror(errno));
		close(listen_fd);
		return -3;
	}
	printf("socket[%d] bind port[%d] success\n", listen_fd, server_port);

	/*
	 * listen()
	 * 监听listen_fd的端口，并设置最大排队连接个数
	 */
	listen(listen_fd, backlog);
	printf("Start listening port[%d]\n", server_port);

	/*
	 * accept()
	 * 等待并接受来自客户端的连接请求
	 * 如果没有客户端连接服务器的话该程序将一直阻塞着不会返回，直到有一个客户端连过来为止
	 * 返回一个client_fd与客户通信
	 */
    for(;;)
    {
    	pthread_t thread_id;
    	printf("\nStart waitting and accept new client to connect...\n");
    	cliaddr_len = sizeof(struct sockaddr);
		client_fd = accept(listen_fd, (struct sockaddr *)&cliaddr, &cliaddr_len);
		if (client_fd < 0)
		{
			printf("accept new client failure: %s\n", strerror(errno));
			continue;
		}
		else
		{
			printf("accept new client [%s:%d] with fd[%d] success\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), client_fd);
		}

        InsertNode(pNet->ClientInfo, 0, sizeof(struct clientInfoType));
        struct clientInfoType* node = FindNode(pNet->ClientInfo, 1);
        if(NULL != node)
        {
        	memset(node->ip,'\0',sizeof(node->ip));
        	inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, node->ip, sizeof(node->ip));;
        	node->port = ntohs(cliaddr.sin_port);
        	node->accFd = client_fd;
			if(NULL != pNet->iCBFunc->tcpConnected)
			{
				pNet->iCBFunc->tcpConnected(pNet, node);
			}
            printf("get connect, ip is %s\n", node->ip);
            printf("port = %d\n", node->port);
        }

        pthread_create(&thread_id, NULL, (void *)fun, pNet);
        pthread_detach(thread_id);
    }

    close(listen_fd);
    return 0;
}

/**
 * @brief     TCP服务器初始化函数
 * @details
 * @param	  struct NetParaType* ptr操作句柄
 * @return    成功后返回handler,失败返回0
 *     - 0  		成功
 *     - ohter      失败
 */
struct NetParaType* TCPServerInit(struct NetParaType* ptr)
{
	pthread_t pid;
	struct NetParaType* pnetPara = ptr;

	/*新建客户端信息链表*/
	pnetPara->ClientInfo = CreatLink(0, sizeof(struct clientInfoType));
	if(NULL == pnetPara->ClientInfo)
	{
		return NULL;
	}

	pthread_create(&pid, NULL, TCPServerRun, pnetPara);
	pthread_detach(pid);

	return pnetPara;
}

/**
 * @brief      匹配IP
 * @details
 * @param
 * @return     STATUS_T
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct clientInfoType* ISItAClient(struct NetParaType* phandler, char* pcheckip)
{
	int nodeCount = GetLinkNodeCount(phandler->ClientInfo);
	struct clientInfoType* pnode;
	for(int i = 1; i < nodeCount + 1; i++)
	{
		pnode = (struct clientInfoType*)FindNode(phandler->ClientInfo, i);
		if(0 == strcmp(pnode->ip, pcheckip))
			return pnode;
	}

	return NULL;
}

/**
 * @brief     TCP服务器发送数据
 * @details
 * @param	  struct NetParaType* pNet	操作句柄
 * 			  char* pdestIP, 			目的IP
 * 			  UINT8* pdata, 			发送数据
 * 			  UINT16 len				发送数据长度
 * @return    int 返回初始化结果
 *     - 0  		成功
 *     - ohter      失败
 */
int TCPServerSend(struct NetParaType* pNet, char* pdestIP, UINT8* pdata, UINT16 len)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct clientInfoType* ptr;

	if(NULL != pdestIP)
	{
		/*检查发送目标合法性*/
		ptr = ISItAClient(pNet, pdestIP);
		if(NULL != ptr)
		{
			write(ptr->accFd, pdata, len);
		}
		else
		{
			printf("Send package to : %s is error, no that client", pdestIP);
		}
	}
	else
	{
		int nodeCount = GetLinkNodeCount(pNet->ClientInfo);
		struct clientInfoType* pnode;
		for(int i = 1; i < nodeCount + 1; i++)
		{
			pnode = (struct clientInfoType*)FindNode(pNet->ClientInfo, i);
			if(NULL != pnode)
			{
				write(pnode->accFd, pdata, len);
			}
		}
	}


	return ret;
}
