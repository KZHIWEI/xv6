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

int pageref[PHYSTOP / PGSIZE];
struct spinlock ref_c_lock;

void kinit() {
  initlock(&kmem.lock, "kmem");
  initlock(&ref_c_lock, "ref_c");
  // uint64 pl = PHYSTOP - PGROUNDUP((uint64)end);
  // printf("pl %d\n", pl);
  // int pn = PGROUNDUP(pl) / PGSIZE;
  // printf("pn %d\n", pn);

  // refend = PGROUNDUP(pn * sizeof(int));
  // printf("sz %d\n", sizeof(int));
  // printf("PS  %p\n", PHYSTOP);
  // printf("PHS %p\n", PHYSTOP - refend);
  // pageref = (int *)(PHYSTOP - refend);

  // memset(pageref, 0, refend);
  // printf("PR  %p\n", pageref);

  freerange(end, (void *)(PHYSTOP));
}

uint pa_index(uint64 pa) { return pa / PGSIZE; }
void _kfree(void *);
int pa_ref_inc(uint64 pa, int inc) {
  int pgn = pa_index(pa);
  acquire(&ref_c_lock);
  int previous = pageref[pgn];
  if (inc > 0) {
    pageref[pgn] += 1;
    release(&ref_c_lock);
  } else if (inc < 0) {
    pageref[pgn] -= 1;
    if (pageref[pgn] <= 0) {
      release(&ref_c_lock);
      _kfree((char *)pa);
      return previous;
    }
    release(&ref_c_lock);
  }
  return previous;
}

void freerange(void *pa_start, void *pa_end) {
  char *p;
  p = (char *)PGROUNDUP((uint64)pa_start);
  for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE) _kfree(p);
}

void _kfree(void *pa) {
  struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= (PHYSTOP))
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa) {
  // struct run *r;

  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= (PHYSTOP))
    panic("kfree");

  pa_ref_inc((uint64)pa, -1);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *kalloc(void) {
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if (r) kmem.freelist = r->next;
  release(&kmem.lock);
  // printf("%d\n", pa_ref_inc((uint64)r, 0));
  if (r) {
    memset((char *)r, 5, PGSIZE);  // fill with junk
    pa_ref_inc((uint64)r, 1);
  }
  // printf("%d\n", pa_ref_inc((uint64)r, 0));

  return (void *)r;
}