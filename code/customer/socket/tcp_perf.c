

#include "sys/ZG_system.h"

static int server_socket = -1;
static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;

static unsigned int socklen = sizeof(client_addr);
static int client_socket[TCP_SERVER_MAX_CONN]={0};
static int conn_amount;

int socket_keepalive(int fd, int keep_idle, int keep_interval, int keep_count)
{
    if(fd==-1){
     return -1;
    }
    int ret = 0;
    int keep_alive = true;

    ret = setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive));
    if(ret<0){
        return ret;
    }

    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle));
    if(ret<0){
        return ret;
    }

    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &keep_interval, sizeof(keep_interval));
    if(ret<0){
        return ret;
    }

    ret = setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count));
    if(ret<0){
        return ret;
    }
    return 0;
}

/**
 * @brief 叠包解析器
 * @author hings
 * @note 
 * @param[in]  data  : 输入数据，
 * @param[in] len  : length of data
 * @param[in] preset_len  : 设置解析的包长 1~64
 * @return none
 */
static void overlap_data_parser(uint8_t * data,int len,int preset_len)
{
   
    if(len > preset_len && len % preset_len == 0 && len <= 4 * preset_len){

        uint8_t parser_buf[64] = {0};
        int len_t = len;
        while(len_t > 0){
        
            memcpy(parser_buf,data + len - len_t,preset_len);
            len_t -= preset_len;
            protocol_data_parser(parser_buf,preset_len,LOCAL_DATA_EVENT);
        }
        
    }else{
        
        protocol_data_parser(data + len - preset_len,preset_len,LOCAL_DATA_EVENT);
    }
}


void socket_client_destroy(int index)
{

  close(client_socket[index]);
  printf("rev failed:clean fd: %d\n",client_socket[index]);

  int DeleteIndex = -1;
  for(int i = 0;i < conn_amount;i++){

      if(client_socket[index] == client_socket[i]){

          DeleteIndex = i;
          break;
      }   
  }
  if(DeleteIndex == -1){

      printf("list find failed！\n");
  }else{
      /*delete date in list*/
      for(int i = DeleteIndex;i < conn_amount;i++){

          client_socket[i] = client_socket[i + 1];
      }
  }

  conn_amount--;

  printf("client list %d:\n",conn_amount);
  for(int listi = 0;listi < conn_amount;listi++){

      printf("[%d:%d]\n",listi,client_socket[listi]);
  }
  printf("\n");  
}

static int create_tcp_server(int port)
{
    uint8_t *ip;
    int max_fd   =-1;
    int opt_val  = 1;
    int fd;
    int len = 0;
    int new_cli_sock = 0;
    uint8_t *recv_data = NULL ;
    uint8_t *send_data = NULL;

    send_data = (uint8_t*)malloc(data_PKTSIZE);
    recv_data =  (uint8_t*)malloc(data_PKTSIZE);

    fd_set readset;
    fd_set writeset;

    struct timeval select_timeout = {0, 50000};

    server_socket = socket(AF_INET, SOCK_STREAM, 0);//初始化socket
    if (server_socket < 0) {
        
      return -1;
    }
    server_addr.sin_family = AF_INET;//地址结构的协议族
    server_addr.sin_port = htons(port);//
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//机器运行的ip
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
       
        close(server_socket);
        return -1;
    }
    if (listen(server_socket, 5) < 0) {
       
        close(server_socket);
        return -1;
    }

    while(1){
#if S907X_BUILD_FOR_ZG
        if (s907x_zg_ota_process_is_start() == pdTRUE){
            printf("ota process begin, exit tcp server task.\n\n");
            break;
        }
#endif
        new_cli_sock = -1;
        FD_ZERO(&readset);
        FD_ZERO(&writeset);
        FD_SET(server_socket, &readset);
        max_fd = server_socket;

        for(fd = 0;fd < TCP_SERVER_MAX_CONN;fd++){

            if(client_socket[fd] > 0){
                FD_SET(client_socket[fd], &readset);
                max_fd = MAX(max_fd, client_socket[fd]);
            }
        }
        if (select(max_fd + 1, &readset, (fd_set *)0, NULL, &select_timeout) <= 0) {//无文件可读或无文件可写，则执行continue
            continue;
        }

        if(FD_ISSET(server_socket,&readset)){

            new_cli_sock = accept(server_socket, (struct sockaddr*)&client_addr, &socklen);

            if (new_cli_sock < 0) {
                
                close(new_cli_sock);
                printf("tcp new chil fail\n");
                continue;
            }

            if (S907X_MAX_TCP_SERVER_ACCEPT_NUM <= conn_amount) {
                close(new_cli_sock);
                USER_DBG("New TCP connection not allowed, current cnt[%d].", conn_amount);
            }
            else {
                client_socket[conn_amount] = new_cli_sock;                
                socket_keepalive(new_cli_sock, 50, 5, 3);
                printf("tcp socket count:%d\n",conn_amount);
                conn_amount++;
                printf("adding client on fd %d\n", new_cli_sock);
                ip = (uint8_t*)&client_addr.sin_addr;
                printf("insert sockfd: %d, ip: %d:%d:%d:%d, port: %d", new_cli_sock, ip[0], ip[1], ip[2], ip[3], client_addr.sin_port);
                printf("client list %d:\n",conn_amount);
                for(int listi = 0;listi < conn_amount;listi++){

                    printf("[%d:%d]\n",listi,client_socket[listi]);
                }
                printf("\n");
            }
        }

        for(fd = 0;fd < conn_amount;fd++){

            if(FD_ISSET(client_socket[fd],&readset)){

                memset(recv_data,0,data_PKTSIZE);
                memset(send_data,0,data_PKTSIZE);
                /*receiver data here*/
                len = recv(client_socket[fd], recv_data, data_PKTSIZE, 0);
                printf("tcp sever:rev data length:%d\n",len);
                if(len > 0){

                 if(recv_data[0] == 0xA0 && recv_data[1] == 0xA1 && recv_data[2] == 0xA2 && recv_data[3] == 0xA3 && recv_data[4] == 0xA4){

                    send_debug_info(0);
                 }
                  else if(recv_data[0] == 0x42 || recv_data[0] == 0x41 || recv_data[0] == 0x31){
                    
                    overlap_data_parser(recv_data,len,9);
                  }else{
                    
                    protocol_data_parser(recv_data,len,LOCAL_DATA_EVENT);
                  }
                }else{

                   socket_client_destroy(fd);
                }
            
            }
        }

    }
    free(send_data);
    free(recv_data);
    return 0;
}
int tcp_server_multi_send(unsigned char *buf,int len)
{
    int slen;

    for(int socket_dix = 0 ; socket_dix < conn_amount; socket_dix++){ //send to all tcp client

        
        slen = send(client_socket[socket_dix], buf, len, 0);
        if(slen > 0){
          printf("tcp sever:send success\n");

        }else{
          printf("send failed\n");
          socket_client_destroy(socket_dix);
        }
    }
    return 0;
}

uint8_t get_tcp_amaount()
{
  return conn_amount;
}

void tcp_conn(void *arg){

    create_tcp_server( TCP_SEVER_PORT);
    vTaskDelete(NULL);
}

void tcp_server_task_init()
{
   xTaskCreate(&tcp_conn, "tcp_conn", TCP_SERVER_TASK_STACK_SZ, NULL, TCP_SERVER_TASK_PRIO, NULL);
}
