
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
# define LIBUNWIND_CONTEXT_SIZE 167
# define LIBUNWIND_CURSOR_SIZE 179
//'long long' is incompatible with C++98
typedef unsigned long long uint64_t;
struct unw_context_t {
 uint64_t data[LIBUNWIND_CONTEXT_SIZE];
};
typedef struct unw_context_t unw_context_t;
struct unw_cursor_t {
 uint64_t data[LIBUNWIND_CURSOR_SIZE];
};
typedef struct unw_cursor_t unw_cursor_t;
typedef unsigned int unw_word_t;
typedef int unw_regnum_t;
#ifdef __cplusplus
extern "C" {
#endif
int unw_getcontext(unw_context_t *);
int unw_init_local(unw_cursor_t *, unw_context_t *);
int unw_step(unw_cursor_t *);
int unw_get_reg(unw_cursor_t *, unw_regnum_t, unw_word_t *);
int unw_set_reg(unw_cursor_t *, unw_regnum_t, unw_word_t);
#ifdef __cplusplus
}
#endif
