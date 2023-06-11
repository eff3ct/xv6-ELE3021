#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int DELETE = 0;

void
single_indirect_test(void)
{
  const int MAX = 150;
  int fd = open("single_indirect_test", O_CREATE | O_RDWR);

  printf(1, "[TEST] Single indirect test\n");

  // 100 블록을 write합니다.
  char buf[512];
  int prev = 0;
  for(int i = 0; i < MAX; ++i) {
    memset(buf, 'a' + (i % 26), 512);
    int cnt = i / (MAX / 10);
    if(prev != cnt) {
      printf(1, "Writing... [");
      for(int j = 0; j < cnt; ++j) printf(1, "%c", '#');
      for(int j = cnt; j < 10; ++j) printf(1, "%c", ' ');
      printf(1, "]\n");
      prev = cnt;
    }
    write(fd, buf, 512);
  }
  sync();
  close(fd);

  // 해당 블록들을 읽어서 확인합니다.
  fd = open("single_indirect_test", O_RDONLY);
  char read_buf[512];
  for(int i = 0; i < MAX; ++i) {
    memset(read_buf, 0, 512);
    int chk = read(fd, read_buf, 512);
    if(chk != 512) {
      printf(1, "[ERROR] Invalid read count.\n");
      exit();
    }
    for(int j = 0; j < 512; ++j) {
      if(read_buf[j] != 'a' + (i % 26)) {
        printf(1, "i, j, read_buf[j], expected = %d, %d, %c, %c\n", i, j, read_buf[j], 'a' + (i % 26));
        printf(1, "[ERROR] Invalid read content.\n");
        exit();
      }
    }
  }

  close(fd);
  if(DELETE) unlink("single_indirect_test");

  printf(1, "[TEST] Single indirect test passed.\n");
}

void
double_indirect_test(void)
{
  const int MAX = 10000;
  int fd = open("double_indirect_test", O_CREATE | O_RDWR);

  printf(1, "[TEST] Double indirect test\n");

  // 10000 블록을 write합니다.
  char buf[512];
  int prev = 0;
  for(int i = 0; i < MAX; ++i) {
    memset(buf, 'a' + (i % 26), 512);
    int cnt = i / (MAX / 10);
    if(prev != cnt) {
      printf(1, "Writing... [");
      for(int j = 0; j < cnt; ++j) printf(1, "%c", '#');
      for(int j = cnt; j < 10; ++j) printf(1, "%c", ' ');
      printf(1, "]\n");
      prev = cnt;
    }
    write(fd, buf, 512);
  }
  sync();
  close(fd);

  // 해당 블록들을 읽어서 확인합니다.
  fd = open("double_indirect_test", O_RDONLY);
  char read_buf[512];
  for(int i = 0; i < MAX; ++i) {
    memset(read_buf, 0, 512);
    int chk = read(fd, read_buf, 512);
    if(chk != 512) {
      printf(1, "[ERROR] Invalid read count.\n");
      exit();
    }
    for(int j = 0; j < 512; ++j) {
      if(read_buf[j] != 'a' + (i % 26)) {
        printf(1, "i, j, read_buf[j], expected = %d, %d, %c, %c\n", i, j, read_buf[j], 'a' + (i % 26));
        printf(1, "[ERROR] Invalid read content.\n");
        exit();
      }
    }
  }

  close(fd);
  if(DELETE) unlink("double_indirect_test");

  printf(1, "[TEST] Double indirect test passed.\n");
}

void
tripple_indirect_test(void)
{
  const int MAX = 50000;
  int fd = open("tripple_indirect_test", O_CREATE | O_RDWR);

  printf(1, "[TEST] Tripple indirect test\n");

  // 50000 블록을 write합니다.
  char buf[512];
  int prev = 0;
  for(int i = 0; i < MAX; ++i) {
    memset(buf, 'a' + (i % 26), 512);
    int cnt = i / (MAX / 10);
    if(prev != cnt) {
      printf(1, "Writing... [");
      for(int j = 0; j < cnt; ++j) printf(1, "%c", '#');
      for(int j = cnt; j < 10; ++j) printf(1, "%c", ' ');
      printf(1, "]\n");
      prev = cnt;
    }
    write(fd, buf, 512);
  }
  sync();
  close(fd);

  // 해당 블록들을 읽어서 확인합니다.
  fd = open("tripple_indirect_test", O_RDONLY);
  char read_buf[512];
  for(int i = 0; i < MAX; ++i) {
    memset(read_buf, 0, 512);
    int chk = read(fd, read_buf, 512);
    if(chk != 512) {
      printf(1, "[ERROR] Invalid read count.\n");
      exit();
    }
    for(int j = 0; j < 512; ++j) {
      if(read_buf[j] != 'a' + (i % 26)) {
        printf(1, "[ERROR] Invalid read content.\n");
        exit();
      }
    }
  }

  close(fd);
  if(DELETE) unlink("tripple_indirect_test");

  printf(1, "[TEST] Tripple indirect test passed.\n");
}

int 
main(int argc, char* argv[])
{
  DELETE = atoi(argv[1]);
  single_indirect_test();
  double_indirect_test();
  tripple_indirect_test();
  exit();
}