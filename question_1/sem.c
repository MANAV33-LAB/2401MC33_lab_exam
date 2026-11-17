#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"

#define NSEM 8

struct sem {
  int value;
  int inuse;
};

struct {
  struct spinlock lock;
  struct sem s[NSEM];
} semtable;

void
seminit(void)
{
  int i;
  initlock(&semtable.lock, "semtable");
  for(i = 0; i < NSEM; i++){
    semtable.s[i].value = 0;
    semtable.s[i].inuse = 0;
  }
}

int
semcreate(int id, int value)
{
  if(id < 0 || id >= NSEM || value < 0)
    return -1;
  acquire(&semtable.lock);
  semtable.s[id].value = value;
  semtable.s[id].inuse = 1;
  release(&semtable.lock);
  return 0;
}

int
semwait(int id)
{
  if(id < 0 || id >= NSEM)
    return -1;
  acquire(&semtable.lock);
  if(!semtable.s[id].inuse){
    release(&semtable.lock);
    return -1;
  }
  while(semtable.s[id].value == 0){
    if(myproc()->killed){
      release(&semtable.lock);
      return -1;
    }
    sleep(&semtable.s[id], &semtable.lock);
  }
  semtable.s[id].value--;
  release(&semtable.lock);
  return 0;
}

int
sempost(int id)
{
  if(id < 0 || id >= NSEM)
    return -1;
  acquire(&semtable.lock);
  if(!semtable.s[id].inuse){
    release(&semtable.lock);
    return -1;
  }
  semtable.s[id].value++;
  wakeup(&semtable.s[id]);
  release(&semtable.lock);
  return 0;
}
