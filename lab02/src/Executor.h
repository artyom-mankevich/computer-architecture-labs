#ifndef RISCV_SIM_EXECUTOR_H
#define RISCV_SIM_EXECUTOR_H

#include "Instruction.h"

class Executor {
public:
    void Execute(InstructionPtr &instr, Word ip) {
        Word al_operation_result = AL_operation(instr);
        write_data(instr, ip, al_operation_result);
        bool jump = get_jump_condition(instr);
        calculate_nextIp(instr, ip, jump);
    }


private:
    static Word AL_operation(InstructionPtr &instr) {
        Word result;

        Word first_operand;
        if (instr->_src1) {
            first_operand = instr->_src1Val;
        } else return result;
        Word second_operand = instr->_imm ? instr->_imm.value() : instr->_src2Val;

        switch (instr->_aluFunc) {
            case AluFunc::Add:
                result = first_operand + second_operand;
                break;
            case AluFunc::Sll:
                result = first_operand << (second_operand % 32);
                break;
            case AluFunc::Slt:
                result = (int) first_operand < (int) second_operand;
                break;
            case AluFunc::Sltu:
                result = first_operand < second_operand;
                break;
            case AluFunc::Xor:
                result = first_operand ^ second_operand;
                break;
            case AluFunc::And:
                result = first_operand & second_operand;
                break;
            case AluFunc::Or:
                result = first_operand | second_operand;
                break;
            case AluFunc::Sub:
                result = first_operand - second_operand;
                break;
            case AluFunc::Sra:
                result = (int) first_operand >> (second_operand % 32);
                break;
            case AluFunc::Srl:
                result = first_operand >> (second_operand % 32);
                break;
            case AluFunc::Sr:
            case AluFunc::None:
                break;
        }
        return result;
    }

    static void write_data(InstructionPtr &instr, Word ip, Word result) {
        switch (instr->_type) {
            case IType::Ld:
                instr->_addr = result;
                break;
            case IType::St:
                instr->_data = instr->_src2Val;
                instr->_addr = result;
                break;
            case IType::J:
            case IType::Jr:
                instr->_data = ip + 4;
                break;
            case IType::Csrr:
                instr->_data = instr->_csrVal;
                break;
            case IType::Csrw:
                instr->_data = instr->_src1Val;
                break;
            case IType::Auipc:
                instr->_data = instr->_imm.value() + ip;
                break;
            default:
                instr->_data = result;
                break;
        }
    }

    static bool get_jump_condition(const InstructionPtr &instr) {
        bool branch = false;
        switch (instr->_brFunc) {
            case BrFunc::Eq:
                branch = instr->_src1Val == instr->_src2Val;
                break;
            case BrFunc::Neq:
                branch = instr->_src1Val != instr->_src2Val;
                break;
            case BrFunc::Lt:
                branch = (int) instr->_src1Val < (int) instr->_src2Val;
                break;
            case BrFunc::Ltu:
                branch = instr->_src1Val < instr->_src2Val;
                break;
            case BrFunc::Ge:
                branch = (int) instr->_src1Val >= (int) instr->_src2Val;
                break;
            case BrFunc::Geu:
                branch = instr->_src1Val >= instr->_src2Val;
                break;
            case BrFunc::AT:
                branch = true;
                break;
            case BrFunc::NT:
                branch = false;
                break;
        }
        return branch;
    }

    static void calculate_nextIp(InstructionPtr &instr, Word ip, bool jump) {
        if (!jump) {
            instr->_nextIp = ip + 4;
        } else {
            Word next_address;
            switch (instr->_type) {
                case IType::Jr:
                    next_address = instr->_src1Val + instr->_imm.value();
                    break;
                case IType::Br:
                case IType::J:
                    next_address = ip + instr->_imm.value();
                    break;
            }
            instr->_nextIp = next_address;
        }
    }
};

#endif // RISCV_SIM_EXECUTOR_H
