//
// Created by student on 1/11/21.
//


#include "utils.h"

//CodeBuffer &buffer = CodeBuffer::instance();


void printOP(string a, string b, string c, string op, CodeBuffer &buffer) {
    buffer.emit(a + " = " + op + " i32 " + b + ", " + c);
}

void retVoid(CodeBuffer &buffer) {
    buffer.emit("ret void\n");
}

void retVal(CodeBuffer &buffer, string ret_val) {
    buffer.emit("ret i32 " + ret_val);
}

void createIntersection(Exp *boolExp, CodeBuffer &buffer, VarGenerator &varGenerator) {
    boolExp->m_reg = varGenerator.freshVar();
    string trueLabel = buffer.genLabel();
    int trueAddress = buffer.emit("br label @");
    string falseLabel = buffer.genLabel();
    int falseAddress = buffer.emit("br label @");
    string endLabel = buffer.genLabel();
    buffer.emit(boolExp->m_reg + " = phi i32 [1, %" + trueLabel + "], [0, %" + falseLabel + "]");
    buffer.bpatch(buffer.makelist({trueAddress, FIRST}), endLabel);
    buffer.bpatch(buffer.makelist({falseAddress, FIRST}), endLabel);
    buffer.bpatch(boolExp->m_trueList, trueLabel);
    buffer.bpatch(boolExp->m_falseList, falseLabel);
}

void castSetToInt(Exp *setExp, Exp *intExp, CodeBuffer &buffer, VarGenerator &varGenerator) {
    intExp->m_reg = varGenerator.freshVar();
    string currCounter = varGenerator.freshVar();
    buffer.emit(currCounter + "= add i32 0, 0");

    string setPointer = varGenerator.freshVar();
    buffer.emit(setPointer + " = inttoptr i32 " + setExp->m_reg + " to %struct.Set*");

    string numOfElementPointer = varGenerator.freshVar();
    buffer.emit(numOfElementPointer + "= getelementptr %struct.Set, %struct.Set* " + setPointer + ", i32 0, i32 2");

    buffer.emit(intExp->m_reg + " = load i32, i32* " + numOfElementPointer);
}

void makeSet(Exp *setExp, int lowerBound, int upperBound, CodeBuffer &buffer, VarGenerator &varGenerator) {
    string setPointer = varGenerator.freshVar();
    string castPointer = varGenerator.freshVar();
    buffer.emit(castPointer + " = call i8* @malloc(i32 352)"); // (32 + 32 + 256 + 32)/8 = size of set in bytes
    buffer.emit(setPointer + " =  bitcast i8* " + castPointer + " to %struct.Set*");

    string lowerBoundPointer = varGenerator.freshVar();
    string upperBoundPointer = varGenerator.freshVar();
    buffer.emit(lowerBoundPointer + "= getelementptr %struct.Set, %struct.Set* " + setPointer + ", i32 0, i32 0");
    buffer.emit(upperBoundPointer + "= getelementptr %struct.Set, %struct.Set* " + setPointer + ", i32 0, i32 1");
    buffer.emit("store i32 " + to_string(lowerBound) + ", i32* " + lowerBoundPointer);
    buffer.emit("store i32 " + to_string(upperBound) + ", i32* " + upperBoundPointer);

    string numOfElementsPtr = varGenerator.freshVar();
    buffer.emit(numOfElementsPtr + "= getelementptr %struct.Set, %struct.Set* " + setPointer + ", i32 0, i32 2");
    buffer.emit("store i32 0, i32* " + numOfElementsPtr);

    string bitMapPtr = varGenerator.freshVar();
    string bitMapCast = varGenerator.freshVar();
    buffer.emit(bitMapPtr + "= getelementptr %struct.Set, %struct.Set* " + setPointer + ", i32 0, i32 3");
    buffer.emit(bitMapCast + " = bitcast [256 x i1]* " + bitMapPtr + " to i8*");
    buffer.emit("call void @llvm.memset.p0i8.i32(i8* " + bitMapCast + ", i8 0, i32 32, i1 0)"); //sets the bitmap to 0

    string i32Pointer = varGenerator.freshVar();
    buffer.emit(i32Pointer + " = ptrtoint %struct.Set* " + setPointer + " to i32");
    setExp->m_reg = i32Pointer;
}

void copySet(string dstSetReg, string srcSetReg, CodeBuffer &buffer, VarGenerator &varGenerator){
    string dstCastPointer = varGenerator.freshVar();
    string resReg = varGenerator.freshVar();

    buffer.emit(dstCastPointer + " = call i8* @malloc(i32 352)"); // (32 + 32 + 256 + 32)/8 = size of set in bytes

    string srcCastPointer = varGenerator.freshVar();
    buffer.emit(srcCastPointer + " = inttoptr i32 " + srcSetReg + " to i8*");

    buffer.emit("call void @llvm.memcpy.p0i8.p0i8.i32(i8* " + dstCastPointer +  ", i8* "+ srcCastPointer + ", i32 352, i1 0)");
    buffer.emit(dstSetReg + " =  ptrtoint i8* " + dstCastPointer + " to i32");
}

void setBinop(Exp* result, Exp* firstExp, Exp* secondExp, CodeBuffer &buffer, VarGenerator &varGenerator, string operation){
    Exp* setExp = firstExp->m_type == "SET" ? firstExp : secondExp;
    Exp* numExp = setExp == firstExp ? secondExp : firstExp;
    result->m_reg = varGenerator.freshVar();

    copySet(result->m_reg, setExp->m_reg, buffer, varGenerator);

    string resSetPointer = varGenerator.freshVar();
    buffer.emit(resSetPointer + " =  inttoptr i32 " + result->m_reg + " to %struct.Set*");

    string lowerBoundPointer = varGenerator.freshVar();
    string upperBoundPointer = varGenerator.freshVar();
    buffer.emit(lowerBoundPointer + "= getelementptr %struct.Set, %struct.Set* " + resSetPointer + ", i32 0, i32 0");
    buffer.emit(upperBoundPointer + "= getelementptr %struct.Set, %struct.Set* " + resSetPointer + ", i32 0, i32 1");

    string lowerBound = varGenerator.freshVar();
    string upperBound = varGenerator.freshVar();
    buffer.emit(lowerBound + "= load i32, i32* " + lowerBoundPointer);
    buffer.emit(upperBound + "= load i32, i32* " + upperBoundPointer);

    // Bounds check
    string smallerThanLower = varGenerator.freshVar();
    string largerThanUpper = varGenerator.freshVar();
    buffer.emit(smallerThanLower + " = icmp slt i32 "+ numExp->m_reg + ", " + lowerBound);
    buffer.emit(largerThanUpper + " = icmp sgt i32 " + numExp->m_reg + ", " + upperBound);
    string cond = varGenerator.freshVar();
    buffer.emit(cond + " = or i1 " + smallerThanLower + ", " + largerThanUpper);
    int address = buffer.emit("br i1 " + cond + ", label @, label @");
    string outOfBoundsLabel = buffer.genLabel();
    buffer.emit("call i32 (i8*, ...) @printf(i8* getelementptr ([31 x i8], [31 x i8]* @set_"+ operation + "_error, i32 0, i32 0))");
    buffer.emit("call void (i32) @exit(i32 1)");
    int exitAddress = buffer.emit("br label @");
    string okLabel = buffer.genLabel();

    string relativePosition = varGenerator.freshVar();
    buffer.emit(relativePosition + "= sub i32 " + numExp->m_reg + ", " + lowerBound);

    string bitMapPointer = varGenerator.freshVar();
    buffer.emit(bitMapPointer + "= getelementptr %struct.Set, %struct.Set* " + resSetPointer + ", i32 0, i32 3");

    string bitMapCellPointer = varGenerator.freshVar();
    buffer.emit(bitMapCellPointer + "= getelementptr [256 x i1], [256 x i1]* " + bitMapPointer + ", i32 0, i32 " + relativePosition);

    string prevValue = varGenerator.freshVar();
    buffer.emit(prevValue + " = load i1, i1* " + bitMapCellPointer);
    string newValue = operation == "add" ? "1" : "0";
    buffer.emit("store i1 " + newValue + ", i1* " + bitMapCellPointer);

    //update number of elements
    string numOfElementsPointer = varGenerator.freshVar();
    buffer.emit(numOfElementsPointer + "= getelementptr %struct.Set, %struct.Set* " + resSetPointer + ", i32 0, i32 2");
    string currNumOfElements = varGenerator.freshVar();
    buffer.emit(currNumOfElements + "= load i32, i32* " + numOfElementsPointer);

    string diff = varGenerator.freshVar();
    buffer.emit(diff + " = xor i1 " + prevValue + ", " + newValue);
    string i32Diff = varGenerator.freshVar();
    buffer.emit(i32Diff + " = zext i1 " + diff + " to i32");
    string newNumOfElements = varGenerator.freshVar();
    buffer.emit(newNumOfElements + " = " + operation + " i32 " + currNumOfElements + ", " + i32Diff);
    buffer.emit("store i32 " + newNumOfElements + ", i32* " + numOfElementsPointer);


    // bpatch for bound check
    buffer.bpatch(buffer.makelist({address, FIRST}), outOfBoundsLabel);
    buffer.bpatch(buffer.makelist({address, SECOND}), okLabel);
    buffer.bpatch(buffer.makelist({exitAddress, FIRST}), okLabel);
}


void setIn(Exp* result, Exp* numExp, Exp* setExp, CodeBuffer &buffer, VarGenerator &varGenerator){
    //loading set struct fields
    string setPointer = varGenerator.freshVar();
    string bitMapPointer = varGenerator.freshVar();
    string lowerBoundPointer = varGenerator.freshVar();
    string upperBoundPointer = varGenerator.freshVar();
    buffer.emit(setPointer + "= inttoptr i32 " + setExp->m_reg + " to %struct.Set*");
    buffer.emit(lowerBoundPointer + "= getelementptr %struct.Set, %struct.Set* " + setPointer + ", i32 0, i32 0");
    buffer.emit(upperBoundPointer + "= getelementptr %struct.Set, %struct.Set* " + setPointer + ", i32 0, i32 1");
    buffer.emit(bitMapPointer + "= getelementptr %struct.Set, %struct.Set* " + setPointer + ", i32 0, i32 3");

    string lowerBound = varGenerator.freshVar();
    string upperBound = varGenerator.freshVar();
    buffer.emit(lowerBound + "= load i32, i32* " + lowerBoundPointer);
    buffer.emit(upperBound + "= load i32, i32* " + upperBoundPointer);

    // Bounds check
    string smallerThanLower = varGenerator.freshVar();
    string largerThanUpper = varGenerator.freshVar();
    buffer.emit(smallerThanLower + " = icmp slt i32 "+ numExp->m_reg + ", " + lowerBound);
    buffer.emit(largerThanUpper + " = icmp sgt i32 " + numExp->m_reg + ", " + upperBound);
    string cond = varGenerator.freshVar();
    buffer.emit(cond + " = or i1 " + smallerThanLower + ", " + largerThanUpper);
    int boundsAddress = buffer.emit("br i1 " + cond + ", label @, label @");
    string outOfBoundsLabel = buffer.genLabel();
    buffer.emit("call i32 (i8*, ...) @printf(i8* getelementptr ([32 x i8], [32 x i8]* @set_in_error, i32 0, i32 0))");
    buffer.emit("call void (i32) @exit(i32 1)");
    int exitAddress = buffer.emit("br label @");
    string okLabel = buffer.genLabel();

    //accessing bit map
    string relativePosition = varGenerator.freshVar();
    buffer.emit(relativePosition + "= sub i32 " + numExp->m_reg + ", " + lowerBound);

    string bitMapCellPointer = varGenerator.freshVar();
    buffer.emit(bitMapCellPointer + "= getelementptr [256 x i1], [256 x i1]* " + bitMapPointer + ", i32 0, i32 " + relativePosition);
    string bitMapCellValue = varGenerator.freshVar();
    buffer.emit(bitMapCellValue + "= load i1, i1* " + bitMapCellPointer);

    // condition bpatch
    int address = buffer.emit("br i1 " + bitMapCellValue + ", label @, label @");
    result->m_trueList = buffer.merge(result->m_trueList, buffer.makelist({address, FIRST}));
    result->m_falseList = buffer.merge(result->m_falseList, buffer.makelist({address, SECOND}));

    // bpatch for bound check
    buffer.bpatch(buffer.makelist({boundsAddress, FIRST}), outOfBoundsLabel);
    buffer.bpatch(buffer.makelist({boundsAddress, SECOND}), okLabel);
    buffer.bpatch(buffer.makelist({exitAddress, FIRST}), okLabel);
}
