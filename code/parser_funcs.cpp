//
// Created by yaniv on 19/06/2021.
//

#include "parser_funcs.hpp"

bool is_number(Exp* var) {
    if (var->m_type == "INT" || var->m_type == "BYTE")
        return true;
    else
        return false;
}

void validate_assign(string var1, string var2) {

    if(var1 == "VOID" || !(var1 == var2 || (var1 == "INT" && var2 == "BYTE"))) {
        errorMismatch(yylineno);
        exit(1);
    }
}

string get_var_type(string var_name) {
    VarSymbol* var = (VarSymbol*)symbolTable.search_symbol(var_name, false);
    if(var == nullptr) {
        errorUndef(yylineno, var_name);
        exit(1);
    }
    return var->type;
}

static void emit_op(const string& a, const string& b, const string& c, const string& op, CodeBuffer &buff) {
    buff.emit(a + " = " + op + " i32 " + b + ", " + c);
}

static void div_handler(Exp* var1, Exp* var2, string m_reg, CodeBuffer &buff){
    // first check for a zero div error
    string cond = Generator::new_tmp();
    buff.emit(cond + " = icmp eq i32 0, " + var2->m_reg);
    int address = buff.emit("br i1 " + cond + ", label @, label @");
    string zeroDivLabel = buff.genLabel();
    buff.emit("call i32 (i8*, ...) @printf(i8* getelementptr ([24 x i8], [24 x i8]* @zero_div_error, i32 0, i32 0))");
    buff.emit("call void (i32) @exit(i32 1)");
    int exitAddress = buff.emit("br label @");
    string okLabel = buff.genLabel();

    if (var1->m_type == "BYTE" && var2->m_type == "BYTE")
        emit_op(m_reg, var1->m_reg, var2->m_reg, "udiv", buff);
    else
        emit_op(m_reg, var1->m_reg, var2->m_reg, "sdiv", buff);

    // bpatch error handling
    buff.bpatch(buff.makelist({address, FIRST}), zeroDivLabel);
    buff.bpatch(buff.makelist({address, SECOND}), okLabel);
    buff.bpatch(buff.makelist({exitAddress, FIRST}), okLabel);

}

Exp* do_op(Exp* var1, Exp* var2, ops op, string specific_op="") {
    if(var1->m_type == "STRING" || var2->m_type == "STRING") {
        goto err;
    }
    switch(op) {
        case(PASSIGN):
            validate_assign(var1->m_type, var2->m_type);
            return var1;

        case(PRELOP):
            if (is_number(var1) && is_number(var2)){
                Exp* ret = new Exp("BOOL");
                string cond = Generator::new_tmp();
                if (var1->m_type == "BYTE" && var2->m_type == "BYTE" && specific_op != "eq" && specific_op != "ne") // unsigned ops for lt,gt,le,ge
                    buffer.emit(cond + " = icmp " + "u" + specific_op.substr(1) + " i32 " + var1->m_reg + ", " + var2->m_reg);
                else
                    buffer.emit(cond + " = icmp " + specific_op + " i32 " + var1->m_reg + ", " + var2->m_reg);

                int address = buffer.emit("br i1 " + cond + ", label @, label @");
                ret->m_trueList = buffer.merge(ret->m_trueList, buffer.makelist({address, FIRST}));
                ret->m_falseList = buffer.merge(ret->m_falseList, buffer.makelist({address, SECOND}));
                return ret;
            }
            break;

        case(PAND):
            if (var1->m_type == "BOOL" && var2->m_type == "BOOL") {
                Exp *exp = new Exp("BOOL");
                exp->m_falseList = buffer.merge(var1->m_falseList, var2->m_falseList);
                exp->m_trueList = buffer.merge(exp->m_trueList, var2->m_trueList);
                return exp;
            }
            break;

        case(POR):
            if (var1->m_type == "BOOL" && var2->m_type == "BOOL") {
                Exp *exp = new Exp("BOOL");
                exp->m_trueList = buffer.merge(var1->m_trueList, var2->m_trueList);
                exp->m_falseList = buffer.merge(exp->m_falseList, var2->m_falseList);
                return exp;
            }
            break;

        case(PNOT):
            if (var1->m_type == "BOOL") {
                Exp *exp = new Exp("BOOL");
                exp->m_falseList = var1->m_trueList;
                exp->m_trueList = var1->m_falseList;
                return exp;
            }
            break;

        case(PBINOP):
            if (is_number(var1) && is_number(var2)){
                string m_reg = Generator::new_tmp();
                if (specific_op == "sdiv") // special handling
                    div_handler(var1, var2, m_reg, buffer);
                else
                    emit_op(m_reg, var1->m_reg, var2->m_reg, specific_op, buffer);

                if(var1->m_type == "INT" || var2->m_type == "INT"){
                    return new Exp("INT", m_reg);
                }
                else{
                    string temp = m_reg;
                    m_reg = Generator::new_tmp();
                    buffer.emit(m_reg + " = and i32 " + temp + ", 255");
                    return new Exp("BYTE", m_reg);
                }
            }
            break;
    }
    err:
    errorMismatch(yylineno);
    exit(1);
}

void vaildate_type(string x, string y){
    if (x != y)
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

FuncSymbol* get_func(string name, std::vector<string> type_list){
    FuncSymbol* f = (FuncSymbol*)(symbolTable.search_symbol(name, true));
    if (f == nullptr) {
        errorUndefFunc(yylineno, name);
        exit(0);
    }
    validate_func(f->args_type, type_list, f->name);
    return f;
}

void validate_func(std::vector<string> expected, std::vector<string> current, string name) {
    if (expected.size() != current.size())
    {
        errorPrototypeMismatch(yylineno, name, expected);
        exit(1);
    }
    int size = expected.size();
    for(int i=0; i < size; i++){
        if(expected[i] != current[size-i-1] && !(expected[i] == "INT" && current[size-i-1] == "BYTE")){
            errorPrototypeMismatch(yylineno, name, expected);
            exit(1);
        }
    }
}

void bool_intersection(Exp *bool_exp) {
    bool_exp->m_reg = Generator::new_tmp();
    string truelist = buffer.genLabel();
    int truelist_address = buffer.emit("br label @");
    string falselist = buffer.genLabel();
    int falselist_address = buffer.emit("br label @");
    string end = buffer.genLabel();
    buffer.emit(bool_exp->m_reg + " = phi i32 [1, %" + truelist + "], [0, %" + falselist + "]");
    buffer.bpatch(buffer.makelist({truelist_address, FIRST}), end);
    buffer.bpatch(buffer.makelist({falselist_address, FIRST}), end);
    buffer.bpatch(bool_exp->m_trueList, truelist);
    buffer.bpatch(bool_exp->m_falseList, falselist);
}
