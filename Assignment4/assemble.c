#include "assemble.h"

int countReg = 0;
int countOffset = 0;
int countFormalPara = 0;
int countRealPara = 0;
const int stackSize = 400;
var_t* interVarHead = NULL;
reg_t *Regs[12];
//reg_t v0, fp, sp, ra, t8;

void initRegs(){
    int i;
    //Regs = malloc(sizeof(reg_t)*12);
    for(i = 0; i<8; i++){
        Regs[i] = malloc(sizeof(reg_t));
        sprintf(Regs[i]->name, "t%d", i);
        Regs[i]->var = NULL;
    }
    for(; i<12; i++){
        Regs[i] = malloc(sizeof(reg_t));
        sprintf(Regs[i]->name, "a%d", i-8);
        Regs[i]->var = NULL;
    }
    /*strcpy(t8.name, "t8"); t8.var = NULL;
    strcpy(v0.name, "v0"); v0.var = NULL;
    strcpy(fp.name, "fp"); fp.var = NULL;
    strcpy(sp.name, "sp"); sp.var = NULL;
    strcpy(ra.name, "ra"); ra.var = NULL;*/
}

void insertInterVar(var_t *var){
    if(interVarHead==NULL){
        interVarHead = var;
        interVarHead->next = NULL;
        return;
    }
    var_t* temp = interVarHead;
    while(temp->next!=NULL)
        temp = temp->next;
    temp->next = var;
    var->next = NULL;
}

var_t* findInterVar(char* name){
    var_t* temp = interVarHead;
    while(temp!=NULL){
        if(strcmp(temp->name, name)==0){
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void writeToMemory(reg_t* reg){
    if(reg==NULL || reg->var == NULL)
        return;

    var_t* temp = reg->var;
    printf("sw $%s, %d($fp)\n", reg->name, -temp->offset);
    //temp->reg = -1;
    reg->var = NULL;
}

void loadFromMemory(var_t *var, reg_t* reg){
    if(reg==NULL)
        return;
    assert(reg->var==NULL || var->offset!=-1);
    printf("lw $%s, %d($fp)\n", reg->name, -var->offset);
    reg->var = var;
}


/*
 * insert variables to the list
 * load the variable into registers and transfer some
 * variables to memory. return the index of register 
 * for var named varName
 */
int getReg(char *varName){
    /*int i, regIndex=-1;
    for(i=0; i<8; i++){
        if(Regs[i]->var==NULL)
            regIndex=i;
    }
    assert(regIndex!=-1);*/
    int regIndex = countReg;
    countReg++;

    var_t* var = findInterVar(varName);
    if(var == NULL){
        var = malloc(sizeof(var_t));
        strcpy(var->name, varName);
        countOffset+=4;
        var->offset = countOffset;
        //var->reg = regIndex;
        insertInterVar(var);
        
        Regs[regIndex]->var = var;
        return regIndex;
    }
    else{
        loadFromMemory(var, Regs[regIndex]);
        //var->reg = regIndex;
        Regs[regIndex]->var = var;
        return regIndex;
    }
    
}

void printInterCode(InterCode code){
    if(code!=NULL){
        InterCode p = code;
        int op1, op2, op3;
        countReg = 0;
        switch(p->kind) {
            case ASSIGN:
                op1 = preparePrintOp(p->u.assign.left);
                op2 = preparePrintOp(p->u.assign.right);
                if(p->u.assign.right->kind == CONSTANT){
                    printf("li $t%d, %d\n", op1, p->u.assign.right->u.value);
                }
                else if(p->u.assign.left->kind==TPOINTER || p->u.assign.left->kind==VPOINTER){
                    printf("sw $t%d, 0($t%d)\n", op2, op1);
                }

                else if(p->u.assign.right->kind==TPOINTER || p->u.assign.right->kind==TPOINTER){
                    printf("lw $t%d, 0($t%d)\n", op1, op2);
                }
                else if(p->u.assign.right->kind == VARIABLE || p->u.assign.right->kind == TEMP){
                    printf("move $t%d, $t%d\n", op1, op2);
                }

                writeToMemory(Regs[op1]);
                //writeToMemory(Regs[op2]);
                break;

            case ADD:
                //assume that no imm in expr
                op1 = preparePrintOp(p->u.binop.result);
                op2 = preparePrintOp(p->u.binop.op1);
                op3 = preparePrintOp(p->u.binop.op2);
                if(op3!=-1)
                    printf("add $t%d, $t%d, $t%d\n", op1, op2, op3);
                else
                    printf("addi $t%d, $t%d, %d\n", op1, op2, p->u.binop.op2->u.value);
                writeToMemory(Regs[op1]);
                //writeToMemory(Regs[op2]);
                //writeToMemory(Regs[op3]);
                break;
 
            case SUB:
                op1 = preparePrintOp(p->u.binop.result);
                op2 = preparePrintOp(p->u.binop.op1);
                op3 = preparePrintOp(p->u.binop.op2);
                if(op3!=-1)
                    printf("sub $t%d, $t%d, $t%d\n", op1, op2, op3);
                else
                    printf("addi $t%d, $t%d, %d\n", op1, op2, p->u.binop.op2->u.value);
                writeToMemory(Regs[op1]);
                break;
 
            case MUL:
                op1 = preparePrintOp(p->u.binop.result);
                op2 = preparePrintOp(p->u.binop.op1);
                op3 = preparePrintOp(p->u.binop.op2);
                printf("mul $t%d, $t%d, $t%d\n", op1, op2, op3);
                writeToMemory(Regs[op1]);
                break;

            case DIVIDE:
                op1 = preparePrintOp(p->u.binop.result);
                op2 = preparePrintOp(p->u.binop.op1);
                op3 = preparePrintOp(p->u.binop.op2);
                printf("div $t%d, $t%d\n", op2, op3);
                printf("mflo $t%d\n", op1);
                writeToMemory(Regs[op1]);
                break;

            case LABELOP:
                printf("label%d: \n", p->u.sigop.op->u.var_no);
                break;

            case GOTO:
                printf("j label%d\n", p->u.sigop.op->u.var_no);
                break;

            case CALL:
                printf("addi $sp, $sp, -4\n");
                printf("sw $ra, 0($sp)\n");
                op1 = preparePrintOp(p->u.callop.result);
                printf("jal %s\n", p->u.callop.name);
                printf("move $t%d, $v0\n", op1);
                writeToMemory(Regs[op1]);
                printf("lw $ra, 0($sp)\n");
                printf("addi $sp, $sp, 4\n");
                break;

            case IFOP:
                op1 = preparePrintOp(p->u.ifop.op1);
                op2 = preparePrintOp(p->u.ifop.op2);
                if(strcmp(p->u.ifop.relop, "==")==0)
                    printf("beq ");
                else if(strcmp(p->u.ifop.relop, "!=")==0)
                    printf("bne ");
                else if(strcmp(p->u.ifop.relop, ">")==0)
                    printf("bgt ");
                else if(strcmp(p->u.ifop.relop, "<")==0)
                    printf("blt ");
                else if(strcmp(p->u.ifop.relop, ">=")==0)
                    printf("bge ");
                else if(strcmp(p->u.ifop.relop, "<=")==0)
                    printf("ble ");
                else
                    printf("error in 'if' compression.\n");
                printf("$t%d, $t%d, label%d\n", op1, op2, p->u.ifop.label->u.var_no);
                Regs[op1]->var=NULL; Regs[op2]->var=NULL;
                //writeToMemory(Regs[op1]);
                //writeToMemory(Regs[op2]);
                break;

            case FUNCTION:
                printf("%s:\n", p->u.funop.name);
                if(strcmp(p->u.funop.name, "main")==0){
                    //add code to initialization, if needed.
                }
                
                printf("addi $sp, $sp, -4\n");
                printf("sw $fp, 0($sp)\n");
                printf("move $fp, $sp\n");
                printf("addi $sp, $sp, %d\n", -stackSize);
                countOffset = 0;
                countFormalPara = 0;
                break;

            case RETURNOP:
                op1 = preparePrintOp(p->u.sigop.op);
                printf("move $v0, $t%d\n", op1);
                printf("addi $sp, $sp, %d\n", stackSize);
                printf("lw $fp, 0($sp)\n");
                printf("addi $sp, $sp, 4\n");
                printf("jr $ra\n");
                break;

            case WRITE:
                op1 = preparePrintOp(p->u.sigop.op);
                printf("move $a0, $t%d\n", op1);
                writeToMemory(Regs[op1]);
                printf("addi $sp, $sp, -4\n");
                printf("sw $ra, 0($sp)\n");
                printf("jal write\n");
                printf("lw $ra, 0($sp)\n");
                printf("addi $sp, $sp, 4\n");
                break;

            case READ:
                printf("addi $sp, $sp, -4\n");
                printf("sw $ra, 0($sp)\n");
                printf("jal read\n");
                printf("lw $ra, 0($sp)\n");
                printf("addi $sp, $sp, 4\n");
                op1 = preparePrintOp(p->u.sigop.op);
                printf("move $t%d, $v0\n", op1);
                writeToMemory(Regs[op1]);
                break;

            case ARG:
            {
                char varName[5];
                sprintf(varName, "t%d", p->u.sigop.op->u.var_no);
                var_t* temp = findInterVar(varName);
                assert(temp!=NULL);
				printf("lw $t8, %d($fp)\n", -temp->offset);
				countOffset += 4;
				printf("sw $t8, %d($sp)\n", 4*(countRealPara));
                countRealPara++;
                if(p->next->kind!=ARG)
                    countRealPara = 0;
                break;
            }
            case PARAM:
            {
                //p->u.sigop.op->u.var_no
                var_t* temp = malloc(sizeof(var_t));
                sprintf(temp->name, "v%d", p->u.sigop.op->u.var_no);
                countOffset += 4;
                temp->offset = countOffset;
                insertInterVar(temp);
				printf("lw $t8, %d($fp)\n",  countFormalPara*4+8);
				printf("sw $t8, %d($fp)\n", -temp->offset);
                countFormalPara++;
                break;
            }
            case DEC:
            {
                var_t* arrayHead = malloc(sizeof(var_t));
                countOffset += 4;
                arrayHead->offset = countOffset;                
                sprintf(arrayHead->name, "v%d", p->u.decop.op->u.var_no);
                insertInterVar(arrayHead);
                countOffset += p->u.decop.size;
                //save start position of array to memory 
                printf("addi $t8, $fp, %d\n", -countOffset);
                printf("sw $t8, %d($fp)\n", -arrayHead->offset);
                break;
            }
        }
    }
}

//get the register of variables;
int preparePrintOp(Operand op){
    if(op==NULL)
        return -1;
    int rtn = -1;
    char varName[5];
    switch(op->kind) {
        case VPOINTER:
        case VARIABLE: 
            sprintf(varName, "v%d", op->u.var_no);
            rtn = getReg(varName);
            break;
        case TPOINTER:
        case TEMP:
            sprintf(varName, "t%d", op->u.var_no);
            rtn = getReg(varName);
            break;
        case ADDRESS:
            break;
    }
    return rtn;
}

void printAssembleHeading(){
    printf(".data\n_prompt: .asciiz \"Enter an integer:\"\n");
    printf("_ret: .asciiz \"\\n\"\n.globl main\n");
    printf(".text\nread:\n");
    printf("    li $v0, 4\n");
    printf("    la $a0, _prompt\n");
    printf("    syscall\n");
    printf("    li $v0, 5\n");
    printf("    syscall\n");
    printf("    jr $ra\n");
    printf("write:\n");
    printf("    li $v0, 1\n");
    printf("    syscall\n");
    printf("    li $v0, 4\n");
    printf("    la $a0, _ret\n");
    printf("    syscall\n");
    printf("    move $v0, $0\n");
    printf("    jr $ra\n");
}

void printAllAssemble(InterCode head, InterCode tail){
    initRegs();
    printAssembleHeading();
    InterCode temp = head;
    while(temp!=tail){
        printInterCode(temp);
        temp = temp->next;
    }
}
/*
void printOp(Operand* op){
    if(op==NULL){
        return;
    }
    switch(op->kind) {
        case CONSTANT: 
            printf("#%d", op->u.value); 
            break;
        case VARIABLE:
        case VPOINTER: 
            char varName[5];
            sprintf(varName, "v%d\0", op->u.var_no);
            printf("$t%d", getReg(varName));
            break;
        case TEMP:
        case TPOINTER:
            char varName[5];
            sprintf(varName, "t%d\0", op->u.var_no);
            printf("$t%d", getReg(varName));
            break;
        case LABEL:
            printf("label%d: ", op->var_no);
            break;
        case ADDRESS:
            break;
        default: printf("unknown command.\n");
    }
}*/
