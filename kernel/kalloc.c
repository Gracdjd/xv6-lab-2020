// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};
struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

//mem_ref 记录某块物理内存引用计数，以及引用相关操作
struct memm_ref
{
  struct spinlock lock;
  int cnt;
};

struct memm_ref mem_ref[PHYSTOP/PGSIZE];
void
kinit()
{
  for(int i = 0; i < PHYSTOP/PGSIZE; ++i)
    initlock(&(mem_ref[i].lock), "kmem_ref");
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
  {
    mem_ref[(uint64)p/PGSIZE].cnt = 1;
    kfree(p);
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  

  r = (struct run*)pa;
  //引用计数为0时释放物理内存
  uint64 pi = (uint64)pa/PGSIZE;
  acquire(&(mem_ref[pi].lock));
    
  mem_ref[pi].cnt = mem_ref[pi].cnt - 1;
  if(mem_ref[pi].cnt > 0)
  {
    release(&(mem_ref[pi].lock));
    return;
  }
  release(&(mem_ref[pi].lock));
  memset(pa, 1, PGSIZE);
  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  //分配物理内存时初始化引用计数为1
  if(r)
  {
    uint64 pi = (uint64)r/PGSIZE;
    acquire(&(mem_ref[pi].lock));
    mem_ref[pi].cnt = 1;
    release(&(mem_ref[pi].lock));
    kmem.freelist = r->next;
  }
  // if(r)
  //   kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

int
get_mem_ref(uint64 pa)
{
  return mem_ref[(uint64)pa/PGSIZE].cnt;
}

int
add_ref(uint64 pa)
{
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    return -1;
  acquire(&(mem_ref[pa/PGSIZE].lock));
  mem_ref[(uint64)pa/PGSIZE].cnt = mem_ref[(uint64)pa/PGSIZE].cnt + 1;
  release(&(mem_ref[pa/PGSIZE].lock));
  return 1;
}