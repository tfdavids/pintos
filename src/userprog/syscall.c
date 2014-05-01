#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "lib/user/syscall.h"

#define MAX_ARGS 3
static uint8_t syscall_arg_num[] =
  {0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1, 2, 1, 1, 1, 2, 1, 1};

static void syscall_handler (struct intr_frame *f);
static void sys_halt (struct intr_frame *f) NO_RETURN;
static void sys_exit (struct intr_frame *f, int status) NO_RETURN;
static void sys_exec (struct intr_frame *f, const char *file);
static void sys_wait (struct intr_frame *f, pid_t pid);
static void sys_create (struct intr_frame *f, const char *file,
  unsigned initial_size);
static void sys_remove (struct intr_frame *f, const char *file);
static void sys_open (struct intr_frame *f, const char *file);
static void sys_filesize (struct intr_frame *f, int fd);
static void sys_read (struct intr_frame *f, int fd, void *buffer,
  unsigned length);
static void sys_write (struct intr_frame *f, int fd, const void *buffer,
  unsigned length);
static void sys_seek (struct intr_frame *f, int fd, unsigned position);
static void sys_tell (struct intr_frame *f, int fd);
static void sys_close (struct intr_frame *f, int fd);
static bool is_valid_ptr (void *ptr);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, &syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // printf ("system call!\n");
  uint32_t args[MAX_ARGS];
  void *intr_esp = f->esp;
  uint32_t syscall_num = *((uint32_t *)intr_esp);
  uint8_t arg_num = syscall_arg_num[syscall_num];
  
  int i;
  for (i = 0; i < arg_num; i++)
    {
      if (!is_valid_ptr (intr_esp))
	{
	  /* Terminate process and free resources */
	}
      intr_esp += sizeof(uint32_t);
      args[i] = *((uint32_t *)intr_esp);
    }

  switch (syscall_num)
    {
      case SYS_HALT:
        sys_halt (f);
        break;
      case SYS_EXIT:
        sys_exit (f, (int)args[0]);
        break;
/*
      case SYS_EXEC:
        sys_exec (f, (const char *)args[0]);
      case SYS_WAIT:
        sys_wait (f, (pid_t)args[0]);
        break;
      case SYS_CREATE:
        sys_create (f, (const char *)args[0], (unsigned)args[1]);
        break;
      case SYS_REMOVE:
        sys_remove (f, (const char *)args[0]);
        break;
      case SYS_OPEN:
        sys_open (f, (const char *)args[0]);
        break;
      case SYS_FILESIZE:
        sys_filesize (f, (int)args[0]);
        break;
      case SYS_READ:
        sys_read (f, (int)args[0], (void *)args[1], (unsigned)args[2]);
        break; 
*/
      case SYS_WRITE:
        sys_write (f, (int)args[0], (const void *)args[1], (unsigned)args[2]);
        break;
/*
      case SYS_SEEK:
        sys_seek (f, (int)args[0], (unsigned)args[1]);
        break;
      case SYS_TELL:
        sys_tell (f, (int)args[0]);
        break;
      case SYS_CLOSE:
      case SYS_MMAP:
      case SYS_MUNMAP:
      case SYS_CHDIR:
      case SYS_MKDIR:
      case SYS_READDIR:
      case SYS_ISDIR:
      case SYS_INUMBER:
      */
      default:
        /* TODO: Decide what to do in this case. */
        printf ("System call not implemented.\n");
    }
}

static bool is_valid_ptr (void *ptr)
{
  /* If null or invalid addr return false */
  if (ptr == NULL || !is_user_vaddr (ptr))
    return false;

  /* If virtual address is unmapped return false */
  if (pagedir_get_page (thread_current ()->pagedir, ptr) == NULL)
    return false;

  return true;
}

/* TODO: Validate user memory. */
static void sys_halt (struct intr_frame *f)
{
  shutdown_power_off ();
}

static void sys_exit (struct intr_frame *f, int status)
{
  f->eax = status;
  thread_exit ();
}

static void sys_exec (struct intr_frame *f, const char *file);
static void sys_wait (struct intr_frame *f, pid_t pid);
static void sys_create (struct intr_frame *f, const char *file,
  unsigned initial_size);
static void sys_remove (struct intr_frame *f, const char *file);
static void sys_open (struct intr_frame *f, const char *file);
static void sys_filesize (struct intr_frame *f, int fd);
static void sys_read (struct intr_frame *f, int fd, void *buffer,
  unsigned length);

static void sys_write (struct intr_frame *f, int fd, const void *buffer,
  unsigned length)
{
  if (fd != 1)
    {
      printf ("Writing to anything other than fd 1 not implemented.\n");
      return;
    }

  putbuf (buffer, length);
  f->eax = length; // TODO: return number of bytes actually written!
}

static void sys_seek (struct intr_frame *f, int fd, unsigned position);
static void sys_tell (struct intr_frame *f, int fd);
static void sys_close (struct intr_frame *f, int fd);
