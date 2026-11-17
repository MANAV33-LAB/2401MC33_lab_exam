#include "types.h"
#include "stat.h"
#include "user.h"

#define N 5

void
child(int self, char letter)
{
  char buf[3];
  int i;

  buf[0] = letter;
  buf[1] = ' ';
  buf[2] = 0;

  for(i = 0; i < N; i++){
    sem_wait(self);
    printf(1, "%s", buf);
    sem_post((self + 1) % 3);
  }
  exit();
}

int
main(void)
{
  int i, pid;

  if(sem_init(0, 1) < 0 || sem_init(1, 0) < 0 || sem_init(2, 0) < 0){
    printf(1, "sem_init failed\n");
    exit();
  }

  for(i = 0; i < 3; i++){
    pid = fork();
    if(pid < 0){
      printf(1, "fork failed\n");
      exit();
    }
    if(pid == 0)
      child(i, 'A' + i);
  }

  for(i = 0; i < 3; i++)
    wait();

  printf(1, "\nDone\n");
  exit();
}
