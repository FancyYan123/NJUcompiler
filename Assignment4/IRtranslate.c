// #include "IRtranslate.h"
// #include <assert.h>
// #include <string.h>
// #include <malloc.h>

#include "common.h"

Operand constant_0;
Operand constant_1;

InterCode translate_Program(Node *root)
{
#ifdef DEBUG_BEGIN
    printf("Program\n");
#endif
    constant_0 = createOperand(CONSTANT, 0);
    constant_1 = createOperand(CONSTANT, 1);
    
    if(root != NULL) {
        InterCode code = translate_ExtDefList(root->Child);
#ifdef DEBUG
    printf("at Program\n");
#endif
        return code;
    }
    else 
        return NULL;
}

InterCode translate_ExtDefList(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("ExtDefList\n");
#endif
    if(node == NULL || node->Child == NULL)
        return NULL;
    Node *node_exdef = node->Child;
    Node *node_exdeflist = node_exdef->Sibling;
    InterCode code1 = translate_ExtDef(node_exdef);
    InterCode code2 = translate_ExtDefList(node_exdeflist);
    InterCode code = IRcodeConcat(2, code1, code2);

#ifdef DEBUG
    printf("translate_ExtDefList\n");
#endif

    return code;
}

InterCode translate_ExtDef(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("ExtDef\n");
#endif
    if(node == NULL)
        return NULL;
    
    Node *child2 = node->Child->Sibling;
    
    // ExtDef__Specifier_ExtDecList_SEMI
    if(strcmp(child2->type, "ExtDecList") == 0) {

#ifdef DEBUG
    printf("ExtDef__Specifier_ExtDecList_SEMI\n");
#endif

        return translate_ExtDecList(child2);
    }

    // ExtDef__Specifier_FunDec_Compst
    else if(strcmp(child2->type, "FunDec")==0) {
        InterCode code1 = translate_FunDec(child2);
        InterCode code2 = translate_CompSt(child2->Sibling);

#ifdef DEBUG
    printf("ExtDef__Specifier_FunDec_Compst\n");
#endif
        
        return IRcodeConcat(2, code1, code2);
    }

    else 
        return NULL;
}


InterCode translate_FunDec(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("FunDec\n");
#endif
    if(node == NULL)
        return NULL;

    Node *Idnode = node->Child;
    InterCode code1 = createFunop(FUNCTION, Idnode->value);

    Node *child3 = Idnode->Sibling->Sibling;
    if(strcmp(child3->type, "VarList") == 0) {
        InterCode code2 = translate_VarList(child3);
        code1 = IRcodeConcat(2, code1, code2);
    }

#ifdef DEBUG

    printf("translate_FunDec\n");

#endif

    return code1;
}

InterCode translate_VarList(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("VarList\n");
#endif
    if(node == NULL)
        return NULL;
    
    Node *ParamNode = node->Child;
    InterCode code = translate_ParamDec(ParamNode);

    if(ParamNode->Sibling != NULL) {
        Node *child3 = ParamNode->Sibling->Sibling;
        InterCode code2 = translate_VarList(child3);
        code = IRcodeConcat(2, code, code2);
    }

#ifdef DEBUG

    printf("translate_VarList\n");

#endif

    return code;
}

InterCode translate_ParamDec(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("ParamDec\n");
#endif
    Node *varNode = node->Child->Sibling;
    InterCode code1 = translate_VarDec(varNode);

    // get ID from VarDec
    FieldList *myid = NULL;
    if(strcmp(varNode->Child->type, "ID") == 0) 
        myid = findVar(varNode->Child->value);
    else if(strcmp(varNode->Child->Child->type, "ID") == 0)
        myid = findVar(varNode->Child->Child->value);

    Operand op_param = createOperand(VARIABLE, myid->val_no);
    InterCode code = createSigop(PARAM, op_param);

#ifdef DEBUG

    printf("translate_ParamDec\n");

#endif

    return IRcodeConcat(2, code1, code);
}


// TODO: check if always return NULL
InterCode translate_ExtDecList(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("ExtDecList\n");
#endif
    if(node == NULL)
        return NULL;
    
    Node *varChild = node->Child;
    InterCode code1 = translate_VarDec(varChild);

    Node *child2 = varChild->Sibling;
    if(child2 != NULL) {
        InterCode code2 = translate_ExtDecList(child2->Sibling);
        code1 = IRcodeConcat(2, code1, code2);
    }

#ifdef DEBUG

    printf("translate_ExtDecList\n");

#endif

    return code1;
}

InterCode translate_VarDec(Node *node)
{
    if(node == NULL)
        return NULL;
#ifdef DEBUG_BEGIN
    printf("VarDec\n");
#endif
    Node *child = node->Child;
    
    if(strcmp(child->type, "VarDec") == 0) {
        Node *node_array = child->Child;
        FieldList *var = findVar(node_array->value);
        Node *node_int = child->Sibling->Sibling;

        int size = 0;
        sscanf(node_int->value, "%d", &size);
        size = size * 4;
#ifdef DEBUG_BEGIN
    printf("size: %d\n", size);
#endif
        Operand arrayAddr = createOperand(VARIABLE, var->val_no);
		InterCode code = createDecop(DEC, arrayAddr, size);

        return code;
    }

    return NULL;
}

InterCode translate_CompSt(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("CompSt\n");
#endif
    if(node == NULL)
        return NULL;

    Node *node_deflist = node->Child->Sibling;
    Node *node_stmtlist = node_deflist->Sibling;
    InterCode code1 = translate_DefList(node_deflist);
    InterCode code2 = translate_StmtList(node_stmtlist);

#ifdef DEBUG

    printf("translate_CompSt\n");

#endif

    return IRcodeConcat(2, code1, code2);
}

InterCode translate_DefList(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("DefList\n");
#endif
    if(node == NULL)
        return NULL;
    if(node->Child == NULL)
        return NULL;

    Node *node_def = node->Child;
    Node *node_deflist = node_def->Sibling;

    InterCode code1 = translate_Def(node_def);

#ifdef DEBUG
    printf("translate_DefList step1: def\n");
#endif

    InterCode code2 = translate_DefList(node_deflist);

#ifdef DEBUG
    printf("translate_DefList\n");
#endif

    return IRcodeConcat(2, code1, code2);
}

InterCode translate_Def(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("Def at\n");
#endif

    if(node == NULL)
        return NULL;
    
    Node *node_declist = node->Child->Sibling;
    InterCode code = translate_DecList(node_declist);

#ifdef DEBUG

    printf("translate_Def\n");

#endif

    return code;
}

InterCode translate_DecList(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("DecList\n");
#endif
    
    if(node == NULL)
        return NULL;

    Node *node_dec = node->Child;
    InterCode code1 = translate_Dec(node_dec);

    if(node_dec->Sibling != NULL) {
        Node *node_declist = node_dec->Sibling->Sibling;
        InterCode code2 = translate_DecList(node_declist);
        code1 = IRcodeConcat(2, code1, code2); 
    }

#ifdef DEBUG

    printf("translate_DecList\n");

#endif

    return code1;
}


// TODO: need check, vardec if is struct type
InterCode translate_Dec(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("Dec\n");
#endif
    
    if(node == NULL)
        return NULL;
    
    Node *node_vardec = node->Child;
    InterCode code = translate_VarDec(node_vardec);

    if(node_vardec->Sibling != NULL) {
        Node *node_vardec = node->Child;
        Node *node_exp = node_vardec->Sibling->Sibling;

        // get ID from VarDec
        FieldList *myid = NULL;
        if(strcmp(node_vardec->Child->type, "ID") == 0) 
            myid = findVar(node_vardec->Child->value);
        else if(strcmp(node_vardec->Child->Child->type, "ID") == 0)
            myid = findVar(node_vardec->Child->Child->value);

        Operand right = createTemp();
        InterCode code1 = translate_Exp(node_exp, right);
        Operand left = createOperand(VARIABLE, myid->val_no);
        InterCode code2 = createAssign(ASSIGN, left, right);
        code = IRcodeConcat(3, code, code1, code2);

#ifdef DEBUG
    printf("translate_Dec done concat\n");
#endif

    }

#ifdef DEBUG
    printf("translate_Dec\n");
#endif

    return code;
}

InterCode translate_StmtList(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("StmtList\n");
#endif
    if(node == NULL || node->Child == NULL)
        return NULL;
    
    Node *node_stmt = node->Child;
    Node *node_stmtlist = node_stmt->Sibling;
    InterCode code1 = translate_Stmt(node_stmt);
    InterCode code2 = translate_StmtList(node_stmtlist);

#ifdef DEBUG

    printf("translate_StmtList\n");

#endif

    return IRcodeConcat(2, code1, code2);
}

InterCode translate_Stmt(Node *node)
{
#ifdef DEBUG_BEGIN
    printf("Stmt\n");
#endif
    if(node == NULL)
        return NULL;

    Node *child1 = node->Child;
    
    // Stmt__Exp_SEMI
    if(strcmp(child1->type, "Exp") == 0) {

#ifdef DEBUG

    printf("Stmt__Exp_SEMI\n");

#endif

        return translate_Exp(child1, NULL);
    }

    // Stmt__Compst
    else if(strcmp(child1->type, "CompSt") == 0) {

#ifdef DEBUG

    printf("Stmt__Compst\n");

#endif

        return translate_CompSt(child1);
    }

    // Stmt__RETURN_Exp_SEMI
    else if(strcmp(child1->type, "RETURN") == 0) {
        Operand t1 = createTemp();
        Node *node_exp = child1->Sibling;
        InterCode code1 = translate_Exp(node_exp, t1);
        InterCode code2 = createSigop(RETURNOP, t1);

#ifdef DEBUG

    printf("Stmt__RETURN_Exp_SEMI\n");

#endif

        return IRcodeConcat(2, code1, code2);
    }

    else if(strcmp(child1->type, "IF") == 0) {
        Node *child5 = child1->Sibling->Sibling->Sibling->Sibling;
        
        Operand label1 = createLable();
        Operand label2 = createLable();
        Node *node_exp = child1->Sibling->Sibling;
        Node *node_stmt = child5;
        InterCode code1 = translate_Cond(node_exp, label1, label2);
        InterCode code2 = translate_Stmt(node_stmt);
        InterCode lablecode1 = createSigop(LABELOP, label1);
        InterCode lablecode2 = createSigop(LABELOP, label2);
        
        // Stmt__IF_LP_Exp_RP_Stmt
        if(child5->Sibling == NULL) {

#ifdef DEBUG

    printf("Stmt__IF_LP_Exp_RP_Stmt\n");

#endif

            return IRcodeConcat(4, code1, lablecode1, code2, lablecode2);
        }
        
        // Stmt__IF_LP_Exp_RP_Stmt_ELSE_Stmt
        else {
            Node *node_stmt2 = child5->Sibling->Sibling;
            Operand label3 = createLable();
            InterCode code3 = translate_Stmt(node_stmt2);
            InterCode labelcode3 = createSigop(LABELOP, label3);
            InterCode gotocode = createSigop(GOTO, label3);

#ifdef DEBUG

    printf("Stmt__IF_LP_Exp_RP_Stmt_ELSE_Stmt\n");

#endif

            return IRcodeConcat(7, code1, lablecode1, code2, gotocode, lablecode2, code3, labelcode3);
        }
    }
    
    // Stmt__WHILE_LP_Exp_RP_Stmt
    else {
        Node *node_exp = child1->Sibling->Sibling;
        Node *node_stmt = node_exp->Sibling->Sibling;

        Operand label1 = createLable();
        Operand label2 = createLable();
        Operand label3 = createLable();
        
        InterCode code1 = translate_Cond(node_exp, label2, label3);
        InterCode code2 = translate_Stmt(node_stmt);
        InterCode labelcode1 = createSigop(LABELOP, label1);
        InterCode labelcode2 = createSigop(LABELOP, label2);
        InterCode labelcode3 = createSigop(LABELOP, label3);
        InterCode gotocode = createSigop(GOTO, label1);

#ifdef DEBUG

    printf("Stmt__WHILE_LP_Exp_RP_Stmt\n");

#endif

        return IRcodeConcat(6, labelcode1, code1, labelcode2, code2, gotocode, labelcode3);
    }
}

InterCode translate_Exp(Node *node, Operand place)
{
#ifdef DEBUG_BEGIN
    printf("Exp\n");
#endif
    if(node == NULL)
        return NULL;

#ifdef DEBUG
    printf("Exp at %s", node->Child->type);
    if(node->Child->Sibling != NULL) {
        printf(" %s", node->Child->Sibling->type);
        if(node->Child->Sibling->Sibling != NULL) 
            printf(" %s", node->Child->Sibling->Sibling->type);
    } 
    printf("\n");
#endif

    Node *child = node->Child;

    if(strcmp(child->type, "LP") == 0) 
        return translate_Exp(child->Sibling, place);

    // Exp__INT
    if(strcmp(child->type, "INT") == 0) {
        if(place == NULL)
            return NULL;
        int val;
        sscanf(child->value, "%d", &val);
        Operand right = createOperand(CONSTANT, val);
        InterCode code = createAssign(ASSIGN, place, right);

#ifdef DEBUG
    printf("Exp__INT\n");
#endif

        return code;
    }

    // Exp__ID
    else if(strcmp(child->type, "ID") == 0 && child->Sibling == NULL) {
#ifdef DEBUG_BEGIN
    printf("   Exp ID\n");
#endif
        if(place == NULL)
            return NULL;
        FieldList *myid = findVar(child->value); 
        Operand right = createOperand(VARIABLE, myid->val_no);
        InterCode code = createAssign(ASSIGN, place, right);

#ifdef DEBUG
    printf("Exp__ID");
#endif

        return code;
    }

    // Exp__Exp_ASSIGNOP_Exp
    else if(strcmp(child->type, "Exp") == 0 &&
            strcmp(child->Sibling->type, "ASSIGNOP") == 0) {
#ifdef DEBUG_BEGIN
    printf("   Exp_ASSIGNOP_Exp\n");
#endif
             
                Node *node_exp = child;
                Node *node_exp2 = node_exp->Sibling->Sibling;

                // Exp1 is ID 
                if(node_exp->Child->Sibling == NULL) {
                    FieldList *myid = findVar(node_exp->Child->value);
                    Operand var = createOperand(VARIABLE, myid->val_no);

                    Operand t1 = createTemp();
                    InterCode code1 = translate_Exp(node_exp2, t1);
                    InterCode tempcode1 = createAssign(ASSIGN, var, t1);
                    InterCode tempcode2 = NULL;
                    if(place != NULL)
                        tempcode2 = createAssign(ASSIGN, place, var);
                    InterCode code2 = IRcodeConcat(2, tempcode1, tempcode2);

#ifdef DEBUG
    printf("Exp__Exp_ASSIGNOP_Exp\n");
#endif
                    
                    return IRcodeConcat(2, code1, code2);
                }

                // TODO: Exp1 is array or struct type
                else if(strcmp(node_exp->Child->type, "Exp") == 0 
                        && strcmp(node_exp->Child->Sibling->type, "LB") == 0) {
                        Node *node_id = node_exp->Child->Child;
                        Node *node_int = node_exp->Child->Sibling->Sibling->Child;

                        FieldList *myid = findVar(node_id->value);
                        int size = 0;
                        sscanf(node_int->value, "%d", &size);
                        size = size * 4;
                        Operand const_4 = createOperand(CONSTANT, size); 

                        Operand var = createOperand(VARIABLE, myid->val_no);
                        //Operand t1 = createOperand(TPOINTER, tempCount);
                        Operand t1 = createTemp();
						InterCode code1 = createBinop(ADD, t1, var, const_4);
                        Operand t8=malloc(sizeof(struct Operand_));
						memcpy(t8, t1, sizeof(struct Operand_));
						t8->kind = TPOINTER;

                        Operand t2 = createTemp();
                        InterCode code2 = translate_Exp(node_exp2, t2);
                        InterCode code3 = createAssign(ASSIGN, t8, t2);
                        InterCode code4 = NULL;
                        if(place != NULL)
                            code4 = createAssign(ASSIGN, place, t8);

                        return IRcodeConcat(4, code1, code2, code3, code4);
                    }
                    
            }
    
    // Exp__Exp_PLUS_Exp, 
    // Exp__Exp_MINUS_Exp, 
    // Exp__Exp_STAR_Exp, 
    // Exp__Exp_DIV_Exp
    else if(strcmp(child->type, "Exp") == 0 && 
            ( strcmp(child->Sibling->type, "PLUS") == 0 ||
              strcmp(child->Sibling->type, "MINUS") == 0 ||
              strcmp(child->Sibling->type, "STAR") == 0 ||
              strcmp(child->Sibling->type, "DIV") == 0 )) {

#ifdef DEBUG_BEGIN
    printf("   Exp ADD\n");
#endif
                if(place == NULL)
                    return NULL;
                Node *node_exp1 = child;
                Node *node_exp2 = node_exp1->Sibling->Sibling;
                  
                Operand t1 = createTemp();
                Operand t2 = createTemp();
                InterCode code1 = translate_Exp(node_exp1, t1);
                InterCode code2 = translate_Exp(node_exp2, t2);

                InterCode code3 = NULL;
                if(strcmp(child->Sibling->type, "PLUS") == 0) {
                    code3 = createBinop(ADD, place, t1, t2);
                }

                else if(strcmp(child->Sibling->type, "MINUS") == 0)
                    code3 = createBinop(SUB, place, t1, t2);
                else if(strcmp(child->Sibling->type, "STAR") == 0)
                    code3 = createBinop(MUL, place, t1, t2);
                else if(strcmp(child->Sibling->type, "DIV") == 0)
                    code3 = createBinop(DIVIDE, place, t1, t2);
             
#ifdef DEBUG
    printf("Exp__Exp_PLUS_Exp");
#endif
                return IRcodeConcat(3, code1, code2, code3);
            }
    
    // Exp__MINUS_Exp
    else if(strcmp(child->type, "MINUS") == 0) {
        Node *node_exp = child->Sibling;

        Operand t1 = createTemp();
        InterCode code1 = translate_Exp(node_exp, t1);
        InterCode code2 = createBinop(SUB, place, constant_0, t1);

#ifdef DEBUG

    printf("Exp__MINUS_Exp\n");

#endif

        return IRcodeConcat(2, code1, code2);
    }

    // Exp__Exp_RELOP_Exp,
    // Exp__NOT_Exp,
    // Exp__Exp_AND_Exp,
    // Exp__Exp_OR_Exp
    else if( strcmp(child->type, "NOT") == 0 || 
            ( strcmp(child->type, "Exp") == 0 && 
             ( strcmp(child->Sibling->type, "AND") == 0 ||
               strcmp(child->Sibling->type, "OR") == 0 ||
               strcmp(child->Sibling->type, "RELOP") == 0 ) )) {
                if(place == NULL)
                    return NULL;
                Operand label1 = createLable();
                Operand label2 = createLable();
                InterCode code0 = createAssign(ASSIGN, place, constant_0);
                InterCode code1 = translate_Cond(node, label1, label2);
                InterCode tempcode1 = createSigop(LABELOP, label1);
                InterCode tempcode2 = createAssign(ASSIGN, place, constant_1);
                InterCode code2 = IRcodeConcat(2, tempcode1, tempcode2);
                InterCode label2_code = createSigop(LABELOP, label2);

#ifdef DEBUG

    printf("Exp__Exp_RELOP_Exp\n");

#endif

                return IRcodeConcat(4, code0, code1, code2, label2_code);
            }
    
    // Exp__ID_LP_RP
    else if( strcmp(child->type, "ID") == 0  &&
             strcmp( child->Sibling->Sibling->type, "RP") == 0) {
#ifdef DEBUG
    printf("at Exp__ID_LP_RP\n");
#endif
                if(place == NULL)
                    place = createTemp();
                char * myfunc = child->value;
                // read()
                if(strcmp(myfunc, "read") == 0) {
#ifdef DEBUG
    printf("at Exp__ID_LP_RP : read\n");
#endif
                    return createSigop(READ, place);
                }

#ifdef DEBUG
    printf("Exp__ID_LP_RP\n");
#endif

                return createCallop(CALL, place, myfunc);
            }

    // Exp__ID_LP_Args_RP
    else if( strcmp(child->type, "ID") == 0  && 
             strcmp(child->Sibling->Sibling->type, "Args") == 0) {

#ifdef DEBUG
    printf("at Exp__ID_LP_Args_RP\n");
#endif

                if(place == NULL)
                    place = createTemp();
                char * myfunc = child->value;
                Node *node_arg = child->Sibling->Sibling;

                Operand *arglist = (Operand *)malloc(sizeof(Operand));
                InterCode code1 = translate_Args(node_arg, arglist);
#ifdef DEBUG
    if(*arglist == NULL) 
        printf("arglist null1\n");
#endif

                // write()
                if(strcmp(myfunc, "write") == 0) {
#ifdef DEBUG
    printf("at write\n");
    if(*arglist == NULL) 
        printf("arglist null2\n");
#endif
                    InterCode tempcode1 = createSigop(WRITE, (*arglist));
#ifdef DEBUG
    printf("at write end\n");
#endif


                    return IRcodeConcat(2, code1, tempcode1);
                }

                InterCode code2 = NULL;
                Operand p = (*arglist);
                for(; p != NULL; p = p->next){
                    InterCode tempcode = createSigop(ARG, p);
                    code2 = IRcodeConcat(2, code2, tempcode);
                }
                InterCode callcode = createCallop(CALL, place, myfunc);

#ifdef DEBUG

    printf("Exp__ID_LP_Args_RP\n");

#endif

                return IRcodeConcat(3, code1, code2, callcode);
            }
    
    // Exp__Exp_LB_Exp_RB
    else if(strcmp(child->type, "Exp") == 0 && strcmp(child->Sibling->type, "LB") == 0) {
        Node *node_id = child->Child;
        Node *node_int = child->Sibling->Sibling->Child;

        FieldList *myid = findVar(node_id->value);
        int size = 0;
        sscanf(node_int->value, "%d", &size);
        size = size * 4;
        Operand const_4 = createOperand(CONSTANT, size); 

        Operand var = createOperand(VARIABLE, myid->val_no);
		Operand t1 = createTemp();
        //Operand t1 = createOperand(TPOINTER, tempCount);
        InterCode code1 = createBinop(ADD, t1, var, const_4);
		Operand t2 = malloc(sizeof(struct Operand_));
		memcpy(t2, t1, sizeof(struct Operand_));
		t2->kind = TPOINTER;
        InterCode code2 = createAssign(ASSIGN, place, t2);

        return IRcodeConcat(2, code1, code2);
    }


    else {
#ifdef DEBUG_BEGIN
    printf("   Exp_Else\n");
#endif
        return NULL;
    }         
}

InterCode translate_Args(Node *node, Operand *arg_list)
{
#ifdef DEBUG_BEGIN
    printf("Args\n");
#endif
    if(node == NULL)
        return NULL;

    Node *child = node->Child;

    // Args__Exp
    if(child->Sibling == NULL) {
        Node *node_exp = child;
        
        Operand t1 = createTemp();
        InterCode code1 = translate_Exp(node_exp, t1);

#ifdef DEBUG
    printf("at args\n");
#endif

        // Arg concat
        if(*arg_list == NULL) {
            *arg_list = t1;
            (*arg_list)->next = NULL;
#ifdef DEBUG
    if(*arg_list == NULL)
        printf("arglist NULL\n");
#endif
        }
        else {
            t1->next = *arg_list;
            *arg_list = t1;
        }

#ifdef DEBUG

    printf("Args__Exp\n");

#endif

        return code1;
    }

    // Args__Exp_COMMA_Args
    else {
        Node *node_exp = child;
        Node *node_arg = node_exp->Sibling->Sibling;

        Operand t1 = createTemp();
        InterCode code1 = translate_Exp(node_exp, t1);

        // Arg concat
        if(*arg_list == NULL) {
            *arg_list = t1;
            (*arg_list)->next = NULL;
        }
        else {
            t1->next = *arg_list;
            *arg_list = t1;
        }

        InterCode code2 = translate_Args(node_arg, arg_list);

#ifdef DEBUG

    printf("Args__Exp_COMMA_Args\n");

#endif

        return IRcodeConcat(2, code1, code2);
    }
}

InterCode translate_Cond(Node *node, Operand label_true, Operand label_false)
{
#ifdef DEBUG_BEGIN
    printf("Cond\n");
#endif
    if(node == NULL)
        return NULL;

    Node *child = node->Child;
    Node *child2 = child->Sibling;

    // Exp__NOT_Exp
    if(strcmp(child->type, "NOT") == 0) {
        Node *node_exp = child2;

#ifdef DEBUG

    printf("Exp__NOT_Exp\n");

#endif

        return translate_Cond(node_exp, label_false, label_true);
    }

    // Exp__Exp_RELOP_Exp
    else if(strcmp(child2->type, "RELOP") == 0) {
        Node *node_exp1 = child;
        Node *node_relop = child2;
        Node *node_exp2 = child2->Sibling;
        
        Operand t1 = createTemp();
        Operand t2 = createTemp();
        InterCode code1 = translate_Exp(node_exp1, t1);
        InterCode code2 = translate_Exp(node_exp2, t2);

        char *op = node_relop->value;
        InterCode code3 = createIfop(IFOP, t1, t2, label_true, op);
        InterCode gotocode = createSigop(GOTO, label_false);

#ifdef DEBUG

    printf("Exp__Exp_RELOP_Exp\n");

#endif

        return IRcodeConcat(4, code1, code2, code3, gotocode);
    }

    // Exp__Exp_AND_Exp
    else if(strcmp(child2->type, "AND") == 0) {
        Node *node_exp1 = child;
        Node *node_exp2 = child2->Sibling;

        Operand label1 = createLable();
        InterCode code1 = translate_Cond(node_exp1, label1, label_false);
        InterCode code2 = translate_Cond(node_exp2, label_true, label_false);
        InterCode labelcode = createSigop(LABELOP, label1);

#ifdef DEBUG

    printf("Exp__Exp_AND_Exp\n");

#endif

        return IRcodeConcat(3, code1, labelcode, code2);
    }

    // Exp__Exp_OR_Exp
    else if(strcmp(child2->type, "OR") == 0) {
        Node *node_exp1 = child;
        Node *node_exp2 = child2->Sibling;

        Operand label1 = createLable();
        InterCode code1 = translate_Cond(node_exp1, label_true, label1);
        InterCode code2 = translate_Cond(node_exp2, label_true, label_false);
        InterCode labelcode = createSigop(LABELOP, label1);

#ifdef DEBUG

    printf("Exp__Exp_OR_Exp\n");

#endif

        return IRcodeConcat(3, code1, labelcode, code2);
    }

    // others
    else {
        Operand t1 = createTemp();
        InterCode code1 = translate_Exp(node, t1);
        InterCode code2 = createIfop(IFOP, t1, constant_0, label_true, "!=");
        InterCode gotocode = createSigop(GOTO, label_false);

        return IRcodeConcat(3, code1, code2, gotocode);
    }
}


