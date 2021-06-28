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

void validate_assign(const string& var1, const string& var2) {

    if(var1 == "VOID" || !(var1 == var2 || (var1 == "INT" && var2 == "BYTE"))) {
        errorMismatch(yylineno);
        exit(1);
    }
}

string get_var_type(string& var_name, SymbolTable& symbolTable) {
    auto* var = (VarSymbol*)symbolTable.search_symbol(var_name, false);
    if(var == nullptr) {
        errorUndef(yylineno, var_name);
        exit(1);
    }
    return var->type;
}

static void emit_op(const string& a, const string& b, const string& c, const string& op, CodeBuffer &buff) {
    buff.emit(a + " = " + op + " i32 " + b + ", " + c);
}

static void div_handler(Exp* var1, Exp* var2, string& m_reg, CodeBuffer &buffer){
    // first check for a zero div error
    string cond = Generator::new_tmp();
    buffer.emit(cond + " = icmp eq i32 0, " + var2->m_reg);
    int address = buffer.emit("br i1 " + cond + ", label @, label @");
    string zeroDivLabel = buffer.genLabel();
    buffer.emit("call i32 (i8*, ...) @printf(i8* getelementptr ([24 x i8], [24 x i8]* @zero_div_error, i32 0, i32 0))");
    buffer.emit("call void (i32) @exit(i32 1)");
    int exitAddress = buffer.emit("br label @");
    string okLabel = buffer.genLabel();

    if (var1->m_type == "BYTE" && var2->m_type == "BYTE")
        emit_op(m_reg, var1->m_reg, var2->m_reg, "udiv", buffer);
    else
        emit_op(m_reg, var1->m_reg, var2->m_reg, "sdiv", buffer);

    // bpatch error handling
    buffer.bpatch(CodeBuffer::makelist({address, FIRST}), zeroDivLabel);
    buffer.bpatch(CodeBuffer::makelist({address, SECOND}), okLabel);
    buffer.bpatch(CodeBuffer::makelist({exitAddress, FIRST}), okLabel);

}

Exp* do_op(Exp* var1, Exp* var2, ops op, string specific_op, CodeBuffer& buffer) {
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
                ret->m_trueList = CodeBuffer::merge(ret->m_trueList, CodeBuffer::makelist({address, FIRST}));
                ret->m_falseList = CodeBuffer::merge(ret->m_falseList, CodeBuffer::makelist({address, SECOND}));
                return ret;
            }
            break;

        case(PAND):
            if (var1->m_type == "BOOL" && var2->m_type == "BOOL") {
                Exp *exp = new Exp("BOOL");
                exp->m_falseList = CodeBuffer::merge(var1->m_falseList, var2->m_falseList);
                exp->m_trueList = CodeBuffer::merge(exp->m_trueList, var2->m_trueList);
                return exp;
            }
            break;

        case(POR):
            if (var1->m_type == "BOOL" && var2->m_type == "BOOL") {
                Exp *exp = new Exp("BOOL");
                exp->m_trueList = CodeBuffer::merge(var1->m_trueList, var2->m_trueList);
                exp->m_falseList = CodeBuffer::merge(exp->m_falseList, var2->m_falseList);
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

void vaildate_type(const string& x, const string& y){
    if (x != y)
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

FuncSymbol* get_func(const string& name, const std::vector<string>& type_list, SymbolTable& symbolTable){
    auto* f = dynamic_cast<FuncSymbol*>(symbolTable.search_symbol(name, true));
    if (f == nullptr) {
        errorUndefFunc(yylineno, name);
        exit(0);
    }
    validate_func(f->args_type, type_list, f->name);
    return f;
}

void validate_func(std::vector<string> expected, const std::vector<string>& current, string& name) {
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

void exp_to_bool(Exp *exp, CodeBuffer& buffer) {
    exp->m_reg = Generator::new_tmp();
    string true_label = buffer.genLabel();
    int true_address = buffer.emit("br label @");
    string false_label = buffer.genLabel();
    int false_address = buffer.emit("br label @");
    string end_label = buffer.genLabel();
    buffer.emit(exp->m_reg + " = phi i32 [1, %" + true_label + "], [0, %" + false_label + "]");
    buffer.bpatch(CodeBuffer::makelist({true_address, FIRST}), end_label);
    buffer.bpatch(CodeBuffer::makelist({false_address, FIRST}), end_label);
    buffer.bpatch(exp->m_trueList, true_label);
    buffer.bpatch(exp->m_falseList, false_label);
}

void call_to_ir(Exp* call, Id* id, ExpList* expList, FuncSymbol* f, bool is_with_args, CodeBuffer& buffer) {
    string ir_ret_type = f->return_type == "VOID" ? "void" : "i32";
    string call_string;
    if(f->return_type != "VOID") {
        call->m_reg = Generator::new_tmp();
        call_string += call->m_reg + " = ";
    }

    if(is_with_args) {
        call_string += "call " + ir_ret_type + " " + f->ir_params() + " @" + id->m_name + "(";
        if (!expList->m_exp.empty()) {
            call_string += "i32 " + expList->m_exp.at(0)->m_reg;
            for (unsigned long i=1; i < expList->m_exp.size(); i++) {
                call_string += ", i32 " + expList->m_exp.at(i)->m_reg;
            }
        }
        call_string += ")";
    } else {
        call_string += "call " + ir_ret_type + " " + f->ir_params() + " @" + id->m_name + "()";
    }
    buffer.emit(call_string);

    if (f->return_type == "BOOL") {
        string cond = Generator::new_tmp();
        buffer.emit(cond + " = icmp ne i32 0, " + call->m_reg);
        int address = buffer.emit("br i1 " + cond + ", label @, label @");
        call->m_trueList = CodeBuffer::merge(call->m_trueList, CodeBuffer::makelist({address, FIRST}));
        call->m_falseList = CodeBuffer::merge(call->m_falseList, CodeBuffer::makelist({address, SECOND}));
    }
}