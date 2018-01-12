#ifndef _INTERCODE_H
#define _INTERCODE_H

#include "common.h"

typedef struct Operand_ *Operand;
typedef struct InterCode_* InterCode;

typedef enum { VARIABLE, CONSTANT, ADDRESS, TEMP, LABEL, 
            VPOINTER, TPOINTER
            } OperandKind;
typedef enum { ASSIGN, ADD, SUB, MUL, DIVIDE, LABELOP, IFOP, 
            GOTO, RETURNOP, READ, WRITE, CALL, ARG, 
            FUNCTION, PARAM, DEC
            } IRKind;

struct Operand_ {
    OperandKind kind;
    union {
        int var_no;
        int value;
    }u;
    Operand next;
};

struct InterCode_ {
    IRKind kind;
    union {
        struct {Operand left, right; }assign; //assign
        struct {Operand result, op1, op2; }binop; //add, sub, mul, divide
		struct {Operand op; }sigop; //goto, return, read, write, arg, label, param
		struct {Operand op1, op2, label; char relop[50]; }ifop; //ifop
		struct {char name[50]; } funop; //function
		struct {Operand result; char name[50]; }callop; //call
		struct {Operand op; int size; }decop; //dec
    }u;

    InterCode pre, next;
};

// IRcode function
void insertIR(InterCode c);
void deleteIR(InterCode c);
void printIR(char *filename);
void printOperand(Operand op, FILE *fp);

void outIR(InterCode c);void outIR(InterCode c);
void outOp(Operand op);

Operand createOperand(OperandKind kind, int val);
Operand createTemp();
Operand createLable();

InterCode createAssign(IRKind kind, Operand left, Operand right);
InterCode createBinop(IRKind kind, Operand result, Operand op1, Operand op2);
InterCode createSigop(IRKind kind, Operand op);
InterCode createIfop(IRKind kind, Operand op1, Operand op2, Operand label, char *relop);
InterCode createFunop(IRKind kind, char *name);
InterCode createCallop(IRKind kind, Operand result, char *name);
InterCode createDecop(IRKind kind, Operand op, int size);

InterCode IRcodeConcat(int num, ...);

extern InterCode IRhead, IRtail;
// extern int varCount;
extern int labelCount;
extern int tempCount;



#endif