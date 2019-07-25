#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "portsc.h"

// Fungsi Utama
int main(int argc, char *argv[]){
  char host[MAX_SERV][BUFFER_SIZE];
  int  port[MAX_SERV];
  int  status[MAX_SERV];
  int  timeout;
  char *path = "config.xml";
  FILE *fp;
  char server_address[BUFFER_SIZE];
  int  port_count;
  int  debug = 0;

  if(argc == 2){
    if(strcmp(argv[1], "DEBUG") == 0){
      debug = 1;
    }
  }

  // Ekstraksi data
  fp = fopen(path, "r");
  port_count = extract_xml(fp, server_address, host, port, &timeout);

  while(1){
    for(int i = 0; i < port_count; i++){
      // Melakukan scanning port
      status[i] = scanning(host[i], port[i], 1, debug);

      // Untuk keperluan Debugging port
      // printf("%s %d : ", host[i], port[i]);
      // if(status[i] == 1) printf("Active\n");
      // else if(status[i] == 0) printf("Inactive\n");
      // else printf("Error\n");

      // Menyiapkan variable untuk tempat JSON dengan format string
      char message[BUFFER_SIZE] = "\0";

      // Merubah struct JSON menjadi format string
      // {"host":"host_addr","port":"port_no","status":"status_port"}
      jsonstr(host[i], port[i], status[i], message, debug);

      // Mengirim data ke server
      int ss = sending(server_address, 10000, message, debug);
      // if(ss == -1){
      //     printf("Data gagal dikirim\n");
      // }else{
      //     printf("Data berhasil dikirim\n");
      // }
    }

    // Memberikan delay
    sleep(timeout); // dalam detik
  }
  return 0;
}
