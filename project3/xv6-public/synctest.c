#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void
no_sync_close(void)
{
  // 강제로 sync를 하여, 버퍼를 비웁니다.
  sync();

  printf(1, "[TEST] no-sync close test\n");

  int fd = open("no_sync_close", O_CREATE | O_RDWR);
  char buf[20];
  for(int i = 0; i < 20; ++i) buf[i] = 'a' + (i % 26);
  write(fd, buf, 20);
  close(fd);

  // 강제로 시스템을 종료시킵니다.
  // 시스템이 종료되었으므로, 버퍼의 내용이 반영되지 않았습니다.
  // 종료는 수동으로 합니다.
  exit();
}

void
check_no_sync_close(void)
{
  int fd = open("no_sync_close", O_RDONLY);
  char buf[20];
  memset(buf, 0, 20);
  read(fd, buf, 20);
  close(fd);

  int cnt = 0;
  for(int i = 0; i < 20; ++i) {
    if(buf[i] == 'a' + (i % 26)) cnt++;
  }

  // 버퍼의 내용이 반영이 되지 않았으므로, 20개 모두가 같으면 안됩니다.
  if(cnt != 20) printf(1, "[TEST] no-sync close test passed..\n");
  else {
    printf(1, "[TEST] no-sync close test failed..\n");
    exit();
  }

  unlink("no_sync_close");
}

void
sync_close(void)
{
  sync();
  printf(1, "[TEST] sync close test\n");

  int fd = open("sync_close", O_CREATE | O_RDWR);
  char buf[20];
  for(int i = 0; i < 20; ++i) buf[i] = 'a' + (i % 26);
  write(fd, buf, 20);
  sync();
  close(fd);

  fd = open("sync_close", O_RDONLY);
  memset(buf, 0, 20);
  read(fd, buf, 20);
  close(fd);

  int cnt = 0;
  for(int i = 0; i < 20; ++i) {
    if(buf[i] == 'a' + (i % 26)) cnt++;
  }

  // 버퍼의 내용이 반영이 되었으므로, 20개 모두가 같아야 합니다.
  if(cnt == 20) printf(1, "[TEST] sync close test passed..\n");
  else {
    printf(1, "[TEST] sync close test failed..\n");
    exit();
  }
}

int
main(int argc, char* argv[])
{
  int test_n = atoi(argv[1]);
  if(test_n == 0) no_sync_close();
  else if(test_n == 1) check_no_sync_close();
  else sync_close();
  exit();
}