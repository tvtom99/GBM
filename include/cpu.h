/*
    NAME: TMC
    INIT DATE: 14/01/2024
    LAST EDIT DATE: 14/01/2024
    DESC:
        This is the header file for all CPU opcodes, the instruction database, etc.
*/

struct instruction {
	char *disassembly;
	unsigned char operandLength;
	void *execute;
} extern const instructions[256];

void reset(void);
void stepCPU(void);

void undefined(void);   //The function that runs if an opcode isn't defined!
void nop(void);