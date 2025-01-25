/*
    NAME: TMC
    INIT DATE: 14/01/2024
    LAST EDIT DATE: 14/01/2024
    DESC:
        This is the header file for all CPU opcodes, the instruction database, etc.
*/

#pragma once

#define FLAGS_ZERO (1 << 7)
#define FLAGS_NEGATIVE (1 << 6)
#define FLAGS_HALFCARRY (1 << 5)
#define FLAGS_CARRY (1 << 4)

#define FLAGS_ISZERO (registers.f & FLAGS_ZERO)
#define FLAGS_ISNEGATIVE (registers.f & FLAGS_NEGATIVE)
#define FLAGS_ISCARRY (registers.f & FLAGS_CARRY)
#define FLAGS_ISHALFCARRY (registers.f & FLAGS_HALFCARRY)

#define FLAGS_ISSET(x) (registers.f & (x))
#define FLAGS_SET(x) (registers.f |= (x))
#define FLAGS_CLEAR(x) (registers.f &= ~(x))

/*===================================================
    THE ABOVE WAS DIRECTLY TAKEN FROM CINOOP AS
    THEY ARE INCREDIBLY HELPFUL AND BASICALLY
    REPRESENT THE SAME WAY I WOULD IMPLEMENT IT!
===================================================*/

struct instruction
{
    char *disassembly;
    unsigned char operandLength;
    void *execute;
} extern const instructions[256];

extern const unsigned char instructionTicks[256];

extern unsigned long ticks;
extern unsigned char stopped;

void reset(void);
void stepCPU(void);

void undefined(void); // The function that runs if an opcode isn't defined!

void nop(void);
void jp_nn(unsigned short operand);
void xor_a(void);
void ld_hl_nn(unsigned short value);
void rst_18(void);
void ld_c_n(unsigned char value);
void ld_b_n(unsigned char value);
void ldd_hlp_a(void);
void dec_b(void);
void jr_nz_n(unsigned char value);
void dec_c(void);
void di(void);
void ld_b_a(void);
void ld_de_nn(unsigned short value);
void ldi_a_hlp(void);
void ld_bc_nn(unsigned short value);
void ld_bcp_a(void);
void ld_a_n(unsigned char value);
void ld_ff_n_ap(unsigned char value);
void ld_ff_ap_n(unsigned char value);
void cp_n(unsigned char value);
void ld_hlp_n(unsigned char value);
void ld_nnp_a(unsigned short value);
void ld_sp_nn(unsigned short value);
void ld_ff_c_a(void);
void inc_c(void);
void call_nn(unsigned short value);
void dec_bc(void);
void ld_a_b(void);
void or_c(void);
void ret(void);
void and_e(void);
void rst_38(void);
static unsigned char dec(unsigned char value);
static unsigned char inc(unsigned char value);
static void and(unsigned char value);
static void xor(unsigned char value);
static void or (unsigned char value);