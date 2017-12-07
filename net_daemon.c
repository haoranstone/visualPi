/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : Main program body
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "oled.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include <time.h>

FILE* fp = NULL;

/*
*   Detect if visual Pi process is already running 
*
*/

int vpx_isRunning(void){
	FILE* fps = fopen("/tmp/visualPi.pid","r");
    char  cmd[50];
	char  pid[10];

	if (fps == NULL)
		return 0;
	
	memset(pid,0,sizeof(pid));
	fgets(pid,10,fps);
	fclose(fps);
	
	// read nothing...
	if(strlen(pid) == 0 )
		return 0;

	int pidnum = atoi(pid);


	int ret = kill((pid_t)pidnum,0);

	if (ret == 0)
		return 1;

	return 0;
	
}


void vpx_SavePID(void){

	int pid = 0;
	char buffer[10];

	pid = getpid();

	sprintf(buffer,"%d",pid);

	FILE* fps = fopen("/tmp/visualPi.pid","wt+");

	// open file failed
	if (fps == NULL)
		return;

	//write current pid
	fwrite(buffer,strlen(buffer),1,fps);

	fclose(fps);

}


void vpx_print_currentTime(void)
{
	struct timespec time;
	clock_gettime(CLOCK_REALTIME,&time); //seconds from 1970 
	struct tm nowtime;
	localtime_r(&time.tv_sec,&nowtime);
	char buffer[50];
	//2017-12-30 11:59:20
	sprintf(buffer,"%04d-%02d-%02d %02d:%02d:%02d",
				   nowtime.tm_year+1900,nowtime.tm_mon+1,nowtime.tm_mday,
				   nowtime.tm_hour, nowtime.tm_min,nowtime.tm_sec);
	LCD_P6x8Str(0, 58, buffer,NULL);
	
}


void vpx_singal_handler(int signum)
{
	LCD_CLS();
	exit(1);
}

/*
*  the Daemon showing IP address of eth0 and wlan0 
*  refresh every 5 seconds 
*/
int vpx_run_Daemon(void)
{
  struct ifaddrs * ifAddrStruct=NULL;
  void * tmpAddrPtr=NULL;
  char   displayBuffer[50];
  int    i = 0;
  u8     line = 0;
  char   eth0[20];
  char   wlan0[20];


  while(1){
  	  memset(eth0+1,0,sizeof(eth0)-1);
	  memset(wlan0+1,0,sizeof(wlan0)-1);
  	  eth0[0]='E';
	  wlan0[0] = 'W';
      getifaddrs(&ifAddrStruct);
      while(ifAddrStruct != NULL){
		 if (ifAddrStruct->ifa_addr->sa_family != AF_INET){
			ifAddrStruct=ifAddrStruct->ifa_next;
            continue;
		 }

		 tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
         char addressBuffer[INET_ADDRSTRLEN];
         inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
			

	  	 if (strcmp("eth0",ifAddrStruct->ifa_name)==0)
	  	 {
	  	 	memcpy(eth0+1,addressBuffer,strlen(addressBuffer));
	  	 }else if (strcmp("wlan0",ifAddrStruct->ifa_name)==0){
		 	memcpy(wlan0+1,addressBuffer,strlen(addressBuffer));
		 }
		 
          ifAddrStruct=ifAddrStruct->ifa_next;
      }
	  PrintLine(0, eth0);
	  PrintLine(1,wlan0);
	  vpx_print_currentTime();
      sleep(5);
      //fwrite(sts,strlen(sts),1,fp);
      //fflush(fp);
  }

}

int main (int argc, const char * argv[]) {
    int pid,i;
    //FILE* fp = NULL;

	if (vpx_isRunning()){
		printf("visual pi is already running..\r\n");
		exit(0);
	}

	LCD_Init();
	PrintLine(0, "START...");

    if(pid=fork()){
      exit(0); //parent process, exit
    }else if(pid < 0)
    {
      fp = fopen("visualPi.Log","a+");
      char* s = "pid less than 0 \r\n";
      fwrite(s,strlen(s),1,fp);
      fclose(fp);
      exit(1); //terminate process if it is abnormal
    }

	// set signal handler 
	signal(SIGINT,vpx_singal_handler);
	//signal(SIGHLD,vpx_singal_handler);
	//signal(SIGABR,vpx_singal_handler);
	signal(SIGKILL,vpx_singal_handler);
	signal(SIGTERM,vpx_singal_handler);
	
    PrintLine(0,"LAUNCH...");
    sleep(10);
    setsid();// re-set sid
    chdir("/tmp");
    //umask(0);

    fp = fopen("visualPi.Log","a");
    if (fp < 0)
    {
	  PrintLine(0,"LOG FAILED");
	  exit(1);
    }
    char* s = "go start to lunching.. \r\n";
    fwrite(s,strlen(s),1,fp);
    fclose(fp);
    //fflush(fp);
    vpx_SavePID();
    vpx_run_Daemon();// here is the real process
    fclose(fp);
    return 0;
}


/******************* (C) COPYRIGHT 2007 Tifnonee Tech *****END OF FILE****/
