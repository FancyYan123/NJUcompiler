#ifndef _ASSEMBLE_H
#define _ASSEMBLE_H

#include "common.h"

extern int countReg;
extern int countOffset;
extern int countFormalPara;
extern int countRealPara;

typedef struct _var_t{
    char name[5];
    //int reg;    //check the index of Regs[] 
    int offset;
    struct _var_t* next;
}var_t;

typedef struct _reg_t{
    char name[5];
    var_t *var;
}reg_t;

extern var_t* interVarHead;

extern reg_t *Regs[];    //t0~t7 and a0~a3;
//extern reg_t *v0, *fp, *sp, *ra, *t8;    //control regs;

void initRegs();
void insertInterVar(var_t *var);
var_t* findInterVar(char *name);
void writeToMemory(reg_t *reg);
void loadFromMemory(var_t *srt, reg_t* dst);

int getReg(char *varName);

/*
void outputOp(Operand* t, FILE* file);
void outputInterCode(InterCode code, FILE* file);
void outputAssemble(InterCode IRhead, InterCode IRtail, char* path);
*/

int preparePrintOp(Operand t);
void printInterCode(InterCode code);
void printAssembleHeading();
void printAllAssemble(InterCode head, InterCode tail);

#endif
