
#ifndef RISCV_SIM_CPU_H
#define RISCV_SIM_CPU_H

#include "Memory.h"
#include "Decoder.h"
#include "RegisterFile.h"
#include "CsrFile.h"
#include "Executor.h"

class Cpu {
public:
    Cpu(Memory &mem)
            : _mem(mem) {

    }

    void ProcessInstruction() {
        Word instr_word = _mem.Request(_ip);
        InstructionPtr instructionPtr = _decoder.Decode(instr_word);
        _rf.Read(instructionPtr);
        _csrf.Read(instructionPtr);
        _exe.Execute(instructionPtr, _ip);
        _mem.Request(instructionPtr);
        _rf.Write(instructionPtr);
        _csrf.Write(instructionPtr);
        _csrf.InstructionExecuted();
        _ip = instructionPtr->_nextIp;
    }

    void Reset(Word ip) {
        _csrf.Reset();
        _ip = ip;
    }

    std::optional<CpuToHostData> GetMessage() {
        return _csrf.GetMessage();
    }

private:
    Reg32 _ip;
    Decoder _decoder;
    RegisterFile _rf;
    CsrFile _csrf;
    Executor _exe;
    Memory &_mem;
};


#endif //RISCV_SIM_CPU_H
