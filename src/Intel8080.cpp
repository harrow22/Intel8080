#include "../include/Intel8080.h"

#include <vector>

constexpr int mnemonic[72] {
        3, // MOV r1, r2
        5, // MOV r, M
        8, // MOV M, r
        11, // SPHL
        13, // MVI r, data
        18, // MVI M, data
        25, // LXI rp, data
        32, // LDA addr
        42, // STA addr
        52, // LHLD addr
        66, // SHLD addr
        79, // LDAX rp
        83, // STAX rp
        87, // XCHG
        88, // ADD r
        89, // ADD M
        93, // ADI data
        97, // ADC r
        98, // ADC M
        103, // ACI data
        107, // SUB r
        108, // SUB M
        112, // SUI data
        116, // SBB r
        117, // SBB M
        121, // SBI data
        125, // INR r
        127, // INR M
        134, // DCR r
        136, // DCR M
        143, // INX rp
        145, // DCX rp
        147, // DAD rp
        154, // DAA
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
            fetch_();
            goto next;
        case 1:
            readT2_();
            ++pc_;
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
            reg_[dst_()] = reg_[src_()];
            goto done;

        // MOV r, M
        case 5:
            readT1_(getPair_(HL));
            goto next;
        case 6:
            readT2_();
            goto next;
        case 7:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[dst_()]);
                goto done;
            }

        // MOV M, r
        case 8:
            writeT1_(getPair_(HL));
            goto next;
        case 9:
            writeT2_(reg_[dst_()]);
            goto next;
        case 10:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto done;
            }

        // SPHL
        case 11: goto next;
        case 12:
            sp_ = getPair_(HL);
            goto done;

        // MVI r, data
        case 13: goto next;
        case 14:
            readT1_(pc_);
            goto next;
        case 16:
            readT2_();
            goto next;
        case 17:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[dst_()]);
                goto done;
            }

        // MVI M, data
        case 18: goto next;
        case 19:
            readT1_(pc_);
            goto next;
        case 20:
            readT2_();
            goto next;
        case 21:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                goto next;
            }
        case 22:
            writeT1_(getPair_(HL));
            goto next;
        case 23:
            writeT2_(tmp_);
            goto next;
        case 24:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto done;
            }

        // LXI rp, data
        case 25: goto next;
        case 26:
            readT1_(pc_);
            goto next;
        case 27:
            readT2_();
            ++pc_;
            goto next;
        case 28:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer16_(rp_() + 1U);
                goto next;
            }
        case 29:
            readT1_(pc_);
            goto next;
        case 30:
            readT2_();
            ++pc_;
            goto next;
        case 31:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer16_(rp_());
                goto done;
            }

        // LDA addr
        case 32: goto next;
        case 33:
            readT1_(pc_);
            goto next;
        case 34:
            readT2_();
            ++pc_;
            goto next;
        case 35:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[Z]);
                goto next;
            }
        case 36:
            readT1_(pc_);
            goto next;
        case 37:
            readT2_();
            ++pc_;
            goto next;
        case 38:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[W]);
                goto next;
            }
        case 39:
            readT1_(getPair_(WZ));
            goto next;
        case 40:
            readT2_();
            ++pc_;
            goto next;
        case 41:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[A]);
                goto done;
            }

        // STA addr
        case 42: goto next;
        case 43:
            readT1_(pc_);
            goto next;
        case 44:
            readT2_();
            ++pc_;
            goto next;
        case 45:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[Z]);
                goto next;
            }
        case 46:
            readT1_(pc_);
            goto next;
        case 47:
            readT2_();
            ++pc_;
            goto next;
        case 48:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[W]);
                goto next;
            }
        case 49:
            writeT1_(getPair_(W));
            goto next;
        case 50:
            writeT2_(reg_[A]);
            goto next;
        case 51:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto done;
            };

        // LHLD addr
        case 52: goto next;
        case 53:
            readT1_(pc_);
            goto next;
        case 54:
            readT2_();
            ++pc_;
            goto next;
        case 56:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[Z]);
                goto next;
            }
        case 57:
            readT1_(pc_);
            goto next;
        case 58:
            readT2_();
            ++pc_;
            goto next;
        case 59:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[W]);
                goto next;
            }
        case 60:
            readT1_(getPair_(WZ) + 1U);
            goto next;
        case 61:
            readT2_();
            goto next;
        case 62:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[L]);
                goto next;
            }
        case 63:
            readT1_(getPair_(WZ));
            goto next;
        case 64:
            readT2_();
            goto next;
        case 65:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[H]);
                goto done;
            }

        // SHLD addr
        case 66: goto next;
        case 67:
            readT1_(pc_);
            goto next;
        case 68:
            readT2_();
            ++pc_;
            goto next;
        case 69:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[Z]);
                goto next;
            }
        case 70:
            readT1_(pc_);
            goto next;
        case 71:
            readT2_();
            ++pc_;
            goto next;
        case 72:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[W]);
                goto next;
            }
        case 73:
            writeT1_(getPair_(WZ) + 1U);
            goto next;
        case 74:
            writeT2_(reg_[L]);
            goto next;
        case 75:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto next;
            }
        case 76:
            writeT1_(getPair_(WZ));
            goto next;
        case 77:
            writeT2_(reg_[H]);
            goto next;
        case 78:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto done;
            }

        // LDAX rp
        case 79: goto next;
        case 80:
            readT1_(getPair_(rp_()));
            goto next;
        case 81:
            readT2_();
            goto next;
        case 82:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(reg_[A]);
                goto done;
            }

        // STAX rp
        case 83: goto next;
        case 84:
            writeT1_(getPair_(rp_()));
            goto next;
        case 85:
            writeT2_(reg_[A]);
            goto next;
        case 86:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto done;
            }

        // XCHG
        case 87:
            std::swap(reg_[H], reg_[D]);
            std::swap(reg_[L], reg_[E]);
            goto done;

        // my emulator cheats in this group by not overlapping the arithmetic instructions with the next fetch/decode cycle
        // ADD r
        case 88:
            add(reg_[src_()]);
            goto done;

        // ADD M
        case 89: goto next;
        case 90:
            readT1_(getPair_(HL));
            goto next;
        case 91:
            readT2_();
            goto next;
        case 92:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                add(tmp_);
                goto done;
            }

        // ADI data
        case 93: goto next;
        case 94:
            readT1_(pc_);
            goto next;
        case 95:
            readT2_();
            ++pc_;
            goto next;
        case 96:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                add(tmp_);
                goto done;
            }

        // ADC r
        case 97:
            add(reg_[src_()] + reg_[F] & carryBit); // TODO: does this work or do i need to pass both?
            goto done;

        // ADC M
        case 98: goto next;
        case 99:
            readT1_(getPair_(HL));
            goto next;
        case 101:
            readT2_();
            goto next;
        case 102:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                add(tmp_ + reg_[F] & carryBit);
                goto done;
            }

        // ACI data
        case 103: goto next;
        case 104:
            readT1_(pc_);
            goto next;
        case 105:
            readT2_();
            ++pc_;
            goto next;
        case 106:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                add(tmp_ + reg_[F] & carryBit);
                goto done;
            }

        // SUB r
        case 107:
            sub(reg_[src_()]);
            goto done;

        // SUB M
        case 108: goto next;
        case 109:
            readT1_(getPair_(HL));
            goto next;
        case 110:
            readT2_();
            goto next;
        case 111:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                sub(tmp_);
                goto done;
            }

        // SUI data
        case 112: goto next;
        case 113:
            readT1_(pc_);
            goto next;
        case 114:
            readT2_();
            ++pc_;
            goto next;
        case 115:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                sub(tmp_);
                goto done;
            }

        // SBB r
        case 116:
            sub(reg_[src_()] - reg_[F] & carryBit);
            goto done;

        // SBB M
        case 117: goto next;
        case 118:
            readT1_(getPair_(HL));
            goto next;
        case 119:
            readT2_();
            goto next;
        case 120:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                add(tmp_ - reg_[F] & carryBit);
                goto done;
            }

        // SBI data
        case 121: goto next;
        case 122:
            readT1_(pc_);
            goto next;
        case 123:
            readT2_();
            ++pc_;
            goto next;
        case 124:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                add(tmp_ - reg_[F] & carryBit);
                goto done;
            }

        // INR r
        case 125: goto next;
        case 126:
            inr(reg_[dst_()]);
            goto done;

        // INR M
        case 127: goto next;
        case 128:
            readT1_(getPair_(HL));
            goto next;
        case 129:
            readT2_();
            goto next;
        case 130:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                inr(tmp_);
                goto next;
            }
        case 131:
            writeT1_(getPair_(HL));
            goto next;
        case 132:
            writeT2_(tmp_);
            goto next;
        case 133:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto done;
            }

        // DCR r
        case 134: goto next;
        case 135:
            dcr(reg_[dst_()]);
            goto done;

        // DCR M
        case 136: goto next;
        case 137:
            readT1_(getPair_(HL));
            goto next;
        case 138:
            readT2_();
            goto next;
        case 139:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                transfer8_(tmp_);
                dcr(tmp_);
                goto next;
            }
        case 140:
            writeT1_(getPair_(HL));
            goto next;
        case 141:
            writeT2_(tmp_);
            goto next;
        case 142:
            if (waiting_()) goto wait;
            else {
                stopDataIn();
                goto done;
            }

        // INX rp
        case 143: goto next;
        case 144:
            setPair_(rp_(), getPair_(rp_()) + 1U);
            goto done;

        // DCX rp
        case 145: goto next;
        case 146:
            setPair_(rp_(), getPair_(rp_()) - 1U);
            goto done;

        // DAD (cheating by using integers)
        case 147: case 148: case 149: case 150: case 151: case 152: goto next;
        case 153: {
            std::uint16_t hl = getPair_(HL);
            std::uint16_t rp = getPair_(rp_());
            setCarryFlag(hl + rp > 0xFFFF);
            setPair_(HL, hl + rp);
            goto done;
        }

        // DAA
        case 154:
            if ((reg_[A] & 0xF) > 9)
                add(6U);
            if ((reg_[A] & 0xF0) > 9 or reg_[F] & carryBit)
                add(6U << 2U);
            goto done;
    }

    wait:
    if (pins & READY)
        pins &= ~WAIT;
    next:
    ++step_;
    done:
    step_ = 0;
}