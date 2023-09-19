#ifndef INTEL8080_INTEL8080_H
#define INTEL8080_INTEL8080_H

#include <cstdint>
#include <vector>
#include "../src/Registers.h"

/*
 * Intel 8080 Emulated Pinout:
 *             ┌───────────────────┐
 *    A0 ○<————│  1             34 │<————○ READY
 *    A1 ○<————│  2             33 │<————○ INT
 *    A2 ○<————│  3             32 │<————○ HOLD
 *    A3 ○<————│  4             31 │<————○ RESET
 *    A4 ○<————│  5             30 │————>○ WAIT
 *    A5 ○<————│  6    INTEL    29 │————>○ HLDA
 *    A6 ○<————│  7     8080    28 │————>○ SYNC
 *    A7 ○<————│  8             27 │————>○ WR
 *    A8 ○<————│  9             26 │————>○ DBIN
 *    A9 ○<————│ 10             25 │————>○ INTE
 *   A10 ○<————│ 11             24 │<———>○ D7
 *   A11 ○<————│ 12             23 │<———>○ D6
 *   A12 ○<————│ 13             22 │<———>○ D5
 *   A13 ○<————│ 14             21 │<———>○ D4
 *   A14 ○<————│ 15             20 │<———>○ D3
 *   A15 ○<————│ 16             19 │<———>○ D2
 *    D0 ○<———>│ 17             18 │<———>○ D1
 *             └───────────────────┘
 *
 * Status Information Definition:
 * --------------------------------------------------------------------------------------------------------------
 *          Data Bus
 * Symbols     Bit                                      Definition
 * INTA        D0  -  Acknowledge signal for INTERRUPT request. Signal should be used to gate a restart
 *                    instruction onto the data bus when DBIN is active.
 * WO          D1  -  Indicates that the operation in the current machine cycle will be a WRITE memory or OUTPUT
 *                    function (WO = 0). Otherwise a READ memory or INPUT operation will be executed.
 * STACK       D2  -  Indicates that the address bus hold the pushdown stack address from the Stack Pointer.
 * HLTA        D3  -  Acknowledge signal for HALT instruction.
 * OUT         D4  -  Indicates that the address bus contains the address of an output device and the data bus
 *                    will contain the output data when WR is active.
 * M1          D5  -  Provides a signal to indicate that the CPU is in the fetch cycle for the first byte of an
 *                    instruction.
 * INP         D6  -  Indicates that the address bus contains the address of an input device and the input data
 *                    should be placed on the data bus when DBIN is active.
 * MEMR        D7  -  Designates that the data bus will be used for memory read data.
 * --------------------------------------------------------------------------------------------------------------
 * *Data bus bit constants are named as their respective symbols (instead of D0-D7) for readability.
 *
 * Control Output Definition:
 * --------------------------------------------------------------------------------------------------------------
 * SYNC  -  Marks the beginning of every machine cycle. Active from T1 until T2.
 *
 *
 */

class Intel8080 {
public:
    void tick();

    // pin bit constants
    static constexpr std::uint_fast64_t A0 {1ULL << 0};
    static constexpr std::uint_fast64_t A1 {1ULL << 1};
    static constexpr std::uint_fast64_t A2 {1ULL << 2};
    static constexpr std::uint_fast64_t A3 {1ULL << 4};
    static constexpr std::uint_fast64_t A4 {1ULL << 5};
    static constexpr std::uint_fast64_t A5 {1ULL << 6};
    static constexpr std::uint_fast64_t A6 {1ULL << 7};
    static constexpr std::uint_fast64_t A7 {1ULL << 8};
    static constexpr std::uint_fast64_t A8 {1ULL << 9};
    static constexpr std::uint_fast64_t A9 {1ULL << 10};
    static constexpr std::uint_fast64_t A10 {1ULL << 11};
    static constexpr std::uint_fast64_t A11 {1ULL << 12};
    static constexpr std::uint_fast64_t A12 {1ULL << 13};
    static constexpr std::uint_fast64_t A13 {1ULL << 14};
    static constexpr std::uint_fast64_t A14 {1ULL << 15};
    static constexpr std::uint_fast64_t A15 {1ULL << 16};
    static constexpr std::uint_fast64_t INTA {1ULL << 17};
    static constexpr std::uint_fast64_t WO {1ULL << 18};
    static constexpr std::uint_fast64_t STACK {1ULL << 19};
    static constexpr std::uint_fast64_t HLTA {1ULL << 20};
    static constexpr std::uint_fast64_t OUT {1ULL << 21};
    static constexpr std::uint_fast64_t M1 {1ULL << 22};
    static constexpr std::uint_fast64_t INP {1ULL << 23};
    static constexpr std::uint_fast64_t MEMR {1ULL << 24};
    static constexpr std::uint_fast64_t INTE {1ULL << 25};
    static constexpr std::uint_fast64_t DBIN {1ULL << 26};
    static constexpr std::uint_fast64_t WR {1ULL << 27};
    static constexpr std::uint_fast64_t SYNC {1ULL << 28};
    static constexpr std::uint_fast64_t HLDA {1ULL << 29};
    static constexpr std::uint_fast64_t WAIT {1ULL << 30};
    static constexpr std::uint_fast64_t RESET {1ULL << 31};
    static constexpr std::uint_fast64_t HOLD {1ULL << 32};
    static constexpr std::uint_fast64_t INT {1ULL << 33};
    static constexpr std::uint_fast64_t READY {1ULL << 34};

    // bus constants
    static constexpr std::uint_fast64_t abus {0xFFFFULL};
    static constexpr std::uint_fast64_t dbus {0xFF0000ULL};

    std::uint8_t status {0};
    std::uint_fast64_t pins {0};

    [[nodiscard]] auto getABus()->std::uint_fast64_t { return pins & abus; }
    [[nodiscard]] auto getDBus()->std::uint_fast64_t { return pins & dbus; }
private:
    static constexpr std::uint8_t signBit {0x80};
    static constexpr std::uint8_t zeroBit {0x40};
    static constexpr std::uint8_t auxiliaryBit {0x10};
    static constexpr std::uint8_t parityBit {0x04};
    static constexpr std::uint8_t carryBit {0x01};

    static constexpr std::uint8_t BC {0};
    static constexpr std::uint8_t DE {2};
    static constexpr std::uint8_t HL {4};
    static constexpr std::uint8_t A {7};
    static constexpr std::uint8_t F {8};


    void setDBus(std::uint_fast64_t val) { pins = pins & ~0xFF0000ULL | val<<16ULL; }
    void setABus(std::uint_fast64_t val) { pins = pins & ~0xFF | val; }

    void t2_();
    void readT1_(std::uint16_t addr) { pins |= SYNC; setDBus(WO|M1|MEMR); setABus(addr); }
    void readT2_() { t2_(); pins |= DBIN; }
    void writeT1_(std::uint16_t addr) { pins |= SYNC; setDBus(0); setABus(addr); }
    void writeT2_(std::uint8_t r) { t2_(); pins |= DBIN; setDBus(r); }
    void stopDataIn() { pins &= ~DBIN; }

    [[nodiscard]] bool waiting_() const { return pins & WAIT; }
    std::uint16_t pair_(std::uint8_t rp) { return reg_[rp] << 8 | reg_[rp+1]; }
    void transfer8_(std::uint8_t& dst) { dst = getDBus(); }
    [[nodiscard]] std::uint8_t& dst_() { return reg_[ir_ >> 3 & 7]; }
    [[nodiscard]] std::uint8_t& src_() { return reg_[ir_ & 7]; }

    void setSignFlag(bool enabled) { enabled ? reg_[F] |= signBit : reg_[F] &= ~signBit; }
    void setZeroFlag(bool enabled) { enabled ? reg_[F] |= zeroBit : reg_[F] &= ~zeroBit; }
    void setAuxCarryFlag(bool enabled) { enabled ? reg_[F] |= auxiliaryBit : reg_[F] &= ~auxiliaryBit; }
    void setParityFlag(bool enabled) { enabled ? reg_[F] |= parityBit : reg_[F] &= ~parityBit; }
    void setCarryFlag(bool enabled) { enabled ? reg_[F] |= carryBit : reg_[F] &= ~carryBit; }

    std::uint16_t pc_ {0}, sp_ {0};
    std::uint8_t ir_ {};
    std::uint8_t reg_[9] {};
    std::uint16_t step_ {0};
};

void Intel8080::t2_()
{
    status = getDBus();
    pins &= ~SYNC;
    if (!(pins & READY))
        pins |= WAIT;
}

#endif //INTEL8080_INTEL8080_H
