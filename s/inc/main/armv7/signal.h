typedef unsigned int size_t;
//arm-linux-androideabi/asm/signal.h
#define SIGSEGV 11
#define SA_SIGINFO 0x00000004
typedef unsigned long sigset_t;
//
typedef struct siginfo siginfo;
struct sigaction{
void(*sa_sigaction)(int,struct siginfo*,void*);
sigset_t sa_mask;
int sa_flags;
void*sa_handler;
//void*sa_restorer;
};
typedef struct{
void *ss_sp;
size_t ss_size;
int ss_flags;
}stack_t;
//./arm-linux-androideabi/asm/sigcontext.h
typedef struct{
  unsigned long trap_no;
  unsigned long error_code;
  unsigned long oldmask;
  unsigned long arm_r0;
  unsigned long arm_r1;
  unsigned long arm_r2;
  unsigned long arm_r3;
  unsigned long arm_r4;
  unsigned long arm_r5;
  unsigned long arm_r6;
  unsigned long arm_r7;
  unsigned long arm_r8;
  unsigned long arm_r9;
  unsigned long arm_r10;
  unsigned long arm_fp;
  unsigned long arm_ip;
  unsigned long arm_sp;
  unsigned long arm_lr;
  unsigned long arm_pc;
  unsigned long arm_cpsr;
  unsigned long fault_address;
}mcontext_t;
//
typedef struct{
void *uc_link;
sigset_t    uc_sigmask;
stack_t     uc_stack;
mcontext_t  uc_mcontext;
}ucontext_t;

#ifdef __cplusplus
extern "C" {
#endif
int sigaction(int sig,const struct sigaction*,struct sigaction*);
#ifdef __cplusplus
}
#endif
