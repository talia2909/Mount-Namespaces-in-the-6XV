/*Talia Yehuda 316471978
Fixing mount_nsleave function so that the command : mount test would work properly*/ 

#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "mount.h"
#include "namespace.h"
#include "mnt_ns.h"

struct {
  struct spinlock lock;
  struct mount_ns mount_ns[NNAMESPACE];
} mountnstable;

struct mount_list {
  struct mount mnt;
  struct mount_list *next;
};

void mount_nsinit()
{
  initlock(&mountnstable.lock, "mountns");
  for (int i = 0; i < NNAMESPACE; i++) {
    initlock(&mountnstable.mount_ns[i].lock, "mount_ns");
  }
}

struct mount_ns* mount_nsdup(struct mount_ns* mount_ns)
{
  acquire(&mountnstable.lock);
  mount_ns->ref++;
  release(&mountnstable.lock);

  return mount_ns;
}

//Only for information. Commented because decraration here is not needed
/*struct mount_list {
  struct mount mnt;
  struct mount_list *next;
};*/
void
umountlist(struct mount_list* mounts)
{
  while (mounts != 0) {
    struct mount_list* next = mounts->next;
    if (mounts->mnt.parent == 0) {
      // No need to unmount root - 
      mounts->mnt.ref = 0;
    } else if (umount(&mounts->mnt) != 0) {
      panic("failed to umount upon namespace close");
    }
    mounts = next;
  }
}

void mount_nsleave(struct mount_ns* mount_ns)
{
/* Fix this function:
   Take care of 2 cases:
     1) The last process leaves mount ns
     2) Not the last process leaves mount ns

   You also need to use: acquire(&mountnstable.lock); - to lock access when needed
   and: release(&mountnstable.lock); - to unlock as soon as possible when not needed.
   The functions are defined in other place, nothing have to be changed in their code.
*/
    acquire(&mountnstable.lock);  // Lock access to the mount namespace table

    if (mount_ns->ref == 1) { /* The last process leaves mount ns  */
        umountlist(mount_ns->list_mounts);
        mount_ns->list_mounts = 0;
        mount_ns->ref = 0;
    }
    else { /* Not the last process leaves mount ns */
        
        mount_ns->ref--;
    }

    release(&mountnstable.lock);  
}

static struct mount_ns* allocmount_ns()
{
  acquire(&mountnstable.lock);
  for (int i = 0; i < NNAMESPACE; i++) {
    if (mountnstable.mount_ns[i].ref == 0 && mountnstable.mount_ns[i].list_mounts == 0)
     {
      struct mount_ns* mount_ns = &mountnstable.mount_ns[i];
      mount_ns->ref = 1;
      release(&mountnstable.lock);
      return mount_ns;
     }
  }
  release(&mountnstable.lock);

  panic("out of mount_ns objects");
}

struct mount_ns* copymount_ns()
{
  struct mount_ns* mount_ns = allocmount_ns();
  mount_ns->list_mounts = copyactivemounts();
  mount_ns->root = getroot(mount_ns->list_mounts);
  return mount_ns;
}

struct mount_ns* newmount_ns()
{
  struct mount_ns* mount_ns = allocmount_ns();
  return mount_ns;
}
