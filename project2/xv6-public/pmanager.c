#include "types.h"
#include "user.h"

#define BUF_SZ 512
#define UPPER_BOUND 1000000000

int 
get_cmd(char *buf, int sz)
{
  printf(1, "pmanager> ");
  memset(buf, 0, sz);
  gets(buf, sz);
  return strlen(buf) > 0;
}

void
err(char *msg)
{
  printf(2, "[ERROR] %s\n", msg);
  exit();
}

int 
_fork(void)
{
  int pid = fork();
  if(pid == -1) err("fork");
  return pid;
}

int check_str(char* str) {
  for(int i = 0; i < strlen(str); ++i) {
    if(
      !(('0' < str[i] && str[i] < '9') 
      || ('a' < str[i] && str[i] < 'z') 
      || ('A' < str[i] && str[i] < 'Z'))
    ) return 0;
  }
  return 1;
}

// 명령어를 실행하는 함수입니다.
void
run_cmd(char* buf)
{
  char tmp[BUF_SZ];
  char cmd[BUF_SZ];
  char arg0[BUF_SZ];
  char arg1[BUF_SZ];

  memset(tmp, 0, BUF_SZ);
  memset(cmd, 0, BUF_SZ);
  memset(arg0, 0, BUF_SZ);
  memset(arg1, 0, BUF_SZ);

  // 명령어를 파싱합니다.
  // printf(1, "[DEBUG]: buf: %s\n", buf);
  char* p = buf;
  char* e = p + strlen(p);
  for(int i = 0, j = 0; p < e; ++p) {
    if(*p != ' ' && *p != 10) {
      // printf(1, "[DEBUG]: *p: %d\n", *p);
      tmp[i++] = *p;
    }
    else {
      tmp[i] = 0;
      if(j == 0) strcpy(cmd, tmp);
      else if(j == 1) strcpy(arg0, tmp);
      else if(j == 2) strcpy(arg1, tmp);
      j++; i = 0;
    }
  }

  // printf(1, "[DEBUG]: cmd: %s, arg0: %s, arg1: %s\n", cmd, arg0, arg1);

  // 명령어를 실행합니다.
  // 명령어가 없으면 아무것도 하지 않습니다.
  if(strlen(cmd) == 0 && strlen(arg0) == 0 && strlen(arg1) == 0) {
    exit();
    return;
  }
  // 명령어가 공백으로 시작하면 에러입니다.
  if(strlen(cmd) == 0) err("A command shouldn't start with space.");
  // 프로그램을 탈출합니다.
  if(!strcmp(cmd, "exit")) {
    exit();
    return;
  }
  // pid를 가진 프로세스를 kill합니다.
  else if(!strcmp(cmd, "kill")) {
    if(strlen(arg0) == 0 || strlen(arg0) > 10) err("kill: invalid pid");
    int pid = atoi(arg0);
    if(pid < 0 || pid > UPPER_BOUND) err("kill: invalid pid");
    if(kill(pid) == -1) printf(2, "[ERROR] kill: kill %d failed.\n", pid);
    else printf(1, "kill %d success.\n", pid);
  }
  // exec2 시스템콜을 이용해 path의 프로그램을 실행합니다.
  else if(!strcmp(cmd, "exec")) {
    if(strlen(arg0) == 0 || strlen(arg0) > 50) err("exec: invalid path");
    if(!check_str(arg0)) err("exec: invalid path");
    if(strlen(arg1) == 0 || strlen(arg1) > 10) err("exec: invalid stacksize");
    if(atoi(arg1) < 0 || atoi(arg1) > UPPER_BOUND) err("exec: invalid stacksize");

    char* argv[] = { arg0, 0 };
    if(exec2(arg0, argv, atoi(arg1)) != 0) printf(2, "[ERROR] exec: %s failed.\n", arg0);
  }
  // 실행중인 프로세스들의 정보를 출력합니다.
  else if(!strcmp(cmd, "list")) get_pinfo();
  // 메모리 제한을 설정합니다.
  else if(!strcmp(cmd, "memlim")) {
    int pid = atoi(arg0);
    int limit = atoi(arg1);

    if(strlen(arg0) == 0 || strlen(arg0) > 10) err("memlim: invalid pid.");
    if(pid < 0 || pid > UPPER_BOUND) err("memlim: invalid pid");
    if(strlen(arg1) == 0 || strlen(arg1) > 10) err("memlim: invalid limit.");
    if(limit < 0 || limit > UPPER_BOUND) err("memlim: invalid limit.");

    if(setmemorylimit(pid, limit) == 0) printf(1, "memlim %d %d success.\n", pid, limit);
    else printf(2, "[ERROR] memlim: limit on %d process with %d memory limit failed.\n", pid, limit);
  }
  // 유효하지 않은 명령어에 대한 처리입니다.
  else printf(2, "[ERROR] Unknown command: %s\n", cmd);

  exit();
}

// 해당 구현은 sh.c의 영향을 강하게 받았음을 밝힙니다.
int
main(void)
{
  static char buf[BUF_SZ];

  while(get_cmd(buf, BUF_SZ) >= 0) {
    if(buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'i' && buf[3] == 't' && buf[4] == 10 && buf[5] == 0)
      exit();
    // 명령어는 자식 프로세스에서 실행됩니다.
    if(_fork() == 0)
      run_cmd(buf);
    wait();
  }

  exit();
}