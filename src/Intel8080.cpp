#include "../include/Intel8080.h"

#include <vector>

constexpr int mnemonic[72] {
    3, // MOV r1, r2
    5, // MOV r, M
    8, // MOV M, r
    0, // SPHL
    0, // MVI r, data
    0, // MVI M, data
    0, // LXI rp, data
    0, // LDA addr
    0, // STA addr
    0, // LHLD addr
    0, // SHLD addr
    0, // LDAX rp
    0, // STAX rp
    0, // XCHG
    0, // ADD r
    0, // ADD M
    0, // ADI data
    0, // ADC r
    0, // ADC M
    0, // ACI data
    0, // SUB r
    0, // SUB M
    0, // SUI data
    0, // SBB r
    0, // SBB M
    0, // SBI data
    0, // INR r
    0, // INR M
    0, // DCR r
    0, // DCR M
    0, // INX rp
    0, // DCX rp
    0, // DAD rp
    0, // DAA
    0, // ANA r
    0, // ANA M
    0, // ANI data
    0, // XRA r
    0, // XRA M
    0, // XRI data
    0, // ORA r
    0, // ORA M
    0, // ORI data
    0, // CMP r
    0, // CMP M
    0, // CPI data
    0, // RLC
    0, // RRC
    0, // RAL
    0, // RAR
    0, // CMA
    0, // CMC
    0, // STC
    0, // JMP addr
    0, // J cond addr
    0, // CALL addr
    0, // C cond addr
    0, // RET
    0, // R cond addr
    0, // RST n
    0, // PCHL
    0, // PUSH rp
    0, // PUSH PSW
    0, // POP rp
    0, // POP PSW
    0, // XTHL
    0, // IN port
    0, // OUT port
    0, // EI
    0, // DI
    0, // HLT
    0, // NOP
};

constexpr int opcode[256] {
    71, 6, 12, 30, 26, 28, 4, 46, 71, 32, 11, 31, 26, 28, 4, 47,
    71, 6, 12, 30, 26, 28, 4, 48, 71, 32, 11, 31, 26, 28, 4, 49,
    71, 6, 10, 30, 26, 28, 4, 33, 71, 32, 9, 31, 26, 28, 4, 50,
    71, 6, 8, 30, 27, 29, 5, 52, 71, 32, 7, 31, 26, 28, 4, 51,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
    2, 2, 2, 2, 2, 2, 70, 2, 0, 0, 0, 0, 0, 0, 1, 0,
    14, 14, 14, 14, 14, 14, 15, 14, 17, 17, 17, 17, 17, 17, 18, 17,
    20, 20, 20, 20, 20, 20, 21, 20, 23, 23, 23, 23, 23, 23, 24, 23,
    34, 34, 34, 34, 34, 34, 35, 34, 37, 37, 37, 37, 37, 37, 38, 37,
    40, 40, 40, 40, 40, 40, 41, 40, 43, 43, 43, 43, 43, 43, 44, 43,
    58, 63, 54, 53, 56, 61, 16, 59, 58, 57, 54, 53, 56, 55, 19, 59,
    58, 63, 54, 67, 56, 61, 22, 59, 58, 57, 54, 66, 56, 55, 25, 59,
    58, 63, 54, 65, 56, 61, 36, 59, 58, 60, 54, 13, 56, 55, 39, 59,
    58, 64, 54, 69, 56, 62, 42, 59, 58, 3, 54, 68, 56, 55, 45, 59,
};

/**
 * Loop Logic Credit: https://floooh.github.io/2021/12/17/cycle-stepped-z80.html#pin-timing-differences-to-a-real-z80
 */
void Intel8080::tick()
{
    if (pins & INT and pins & INTE) {
        return;
    }
    switch (step_) {
        // instruction fetch
        case 0:
            readT1_(pc_);
            goto next;
        case 1:
            readT2_();
            pc_ += 1;
            goto next;
        case 2:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(ir_);
                step_ = mnemonic[opcode[ir_]];
                break;
            }

        // MOV r1, r2
        case 3: goto next;
        case 4:
            dst_() = src_();
            goto done;

        // MOV r, M
        case 5:
            readT1_(pair_(HL));
            goto next;
        case 6:
            readT2_();
            goto next;
        case 7:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(dst_());
                goto done;
            }

        // MOV M, r
        case 8:
            writeT1_(pair_(HL));
            goto next;
        case 9:
            writeT2_(dst_());
            goto next;
        case 10:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto done;
            }

        // SPHL
        case 11:goto next;
        case 12:
            sp_ = pair_(HL);
            goto done;

        // MVI r, data

    }

    wait:
        if (pins & READY)
            pins &= ~WAIT;
    next:
        ++step_;
    done:
        step_ = 0;
}
