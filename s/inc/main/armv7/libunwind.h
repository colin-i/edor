
//https://github.com/llvm-mirror/libunwind
enum {
 UNW_ARM_R0  = 0,
 UNW_ARM_R1  = 1,
 UNW_ARM_R2  = 2,
 UNW_ARM_R3  = 3,
 UNW_ARM_R4  = 4,
 UNW_ARM_R5  = 5,
 UNW_ARM_R6  = 6,
 UNW_ARM_R7  = 7,
 UNW_ARM_R8  = 8,
 UNW_ARM_R9  = 9,
 UNW_ARM_R10 = 10,
 UNW_ARM_R11 = 11,
 UNW_ARM_R12 = 12,
 UNW_ARM_R13 = 13,
 UNW_ARM_R14 = 14,
 UNW_ARM_R15 = 15
};
enum {
 UNW_REG_IP = -1,// instruction pointer
 UNW_REG_SP = -2 // stack pointer
};

//don't know on termux nowadays but on raspberry os is not unw_set_reg anymore, and unw_getcontext, ...

//# define LIBUNWIND_CONTEXT_SIZE 167
//# define LIBUNWIND_CURSOR_SIZE 179
////'long long' is incompatible with C++98
//typedef unsigned long long uint64_t;
//struct unw_context_t {
// uint64_t data[LIBUNWIND_CONTEXT_SIZE];
//};
//typedef struct unw_context_t unw_context_t;
//struct unw_cursor_t {
// uint64_t data[LIBUNWIND_CURSOR_SIZE];
//};
//typedef struct unw_cursor_t unw_cursor_t;
//typedef unsigned int unw_word_t;
typedef struct unw_tdep_context
  {
    unsigned long regs[16];
  }
unw_tdep_context_t;
#define unw_context_t unw_tdep_context_t
typedef unsigned int __uint32_t;
typedef __uint32_t uint32_t;
typedef uint32_t unw_word_t;
typedef struct unw_cursor
  {
    unw_word_t opaque[4096];
  }
unw_cursor_t;

typedef int unw_regnum_t;

#ifdef __cplusplus
extern "C" {
#endif

//int unw_getcontext(unw_context_t *);
#define unw_tdep_getcontext(uc) ({ unw_tdep_context_t *unw_ctx = (uc); register unsigned long *r0 __asm__ ("r0"); unsigned long *unw_base = unw_ctx->regs; __asm__ __volatile__ ( ".align 2\n" "bx pc\n" "nop\n" ".code 32\n" "mov r0, #0\n" "stmia %[base], {r0-r14}\n" "adr r0, ret%=+1\n" "str r0, [%[base], #60]\n" "orr r0, pc, #1\n" "bx r0\n" ".code 16\n" "mov r0, #0\n" "ret%=:\n" : [r0] "=r" (r0) : [base] "r" (unw_base) : "memory", "cc"); (int)r0; })
#define unw_getcontext unw_tdep_getcontext

int _Uarm_init_local(unw_cursor_t *, unw_context_t *);
#define unw_init_local _Uarm_init_local
int _Uarm_step(unw_cursor_t *);
#define unw_step _Uarm_step
int _Uarm_get_reg(unw_cursor_t *, unw_regnum_t, unw_word_t *);
#define unw_get_reg _Uarm_get_reg
int _Uarm_set_reg(unw_cursor_t *, unw_regnum_t, unw_word_t);
#define unw_set_reg _Uarm_set_reg
#ifdef __cplusplus
}
#endif
