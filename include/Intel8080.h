#ifndef INTEL8080_INTEL8080_H
#define INTEL8080_INTEL8080_H

#include <cstdint>

/*
 * Intel 8080 Emulated Pinout:
 *             ┌───────────────────┐
 *    A0 ○<————│  1             34 │<————○ READY
 *    A1 ○<————│  2             33 │<————○ INT
 *    A2 ○<————│  3             32 │<————○ HOLD ** REMOVE
 *    A3 ○<————│  4             31 │<————○ RESET ** REMOVE implemented as a function
 *    A4 ○<————│  5             30 │————>○ WAIT
 *    A5 ○<————│  6    INTEL    29 │————>○ HLDA ** REMOVE
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
 * ## Member Functions:
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * void tick();
 *   - Steps the processor one state forward. Each instruction consists of 1-5 machine cycles and 3-5 states (T1-T5)
 *   - constitute a machine cycle. A full instruction cycle requires anywhere from 4-18 states for its completion.
 *
 * void reset();
 *   - Restores the processor's internal program counter to zero, and the cpu will begin the next cycle from T1. Note,
 *   - however, that this has no effect on status flags, or on any of the processor's working registers
 *
 * const std::uint_fast64_t getABus();
 *    - Returns the current bits on the address bus.
 *
 * const std::uint_fast64_t getDBus();
 *    - Returns the current bits on the data bus.
 *
 *
 * ## Member Variables:
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * std::uint8_t status;
 *    - Identifies the current machine cycle. Is set in the second state of every machine cycle.
 *
 * std::uint_fast_64_t pins;
 *    - The current state of the processors pins.
 *
 *
 * ## Status Information Definition:
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 *          Data Bus
 * Symbols     Bit                                      Definition
 * INTA        D0  -  Acknowledge signal for INTERRUPT request. Signal should be used to gate a restart instruction onto
 *                    the data bus when DBIN is active.
 * WO          D1  -  Indicates that the operation in the current machine cycle will be a WRITE memory or OUTPUT
 *                    function (WO = 0). Otherwise a READ memory or INPUT operation will be executed.
 * STACK       D2  -  Indicates that the address bus hold the pushdown stack address from the Stack Pointer.
 * HLTA        D3  -  Acknowledge signal for HALT instruction.
 * OUT         D4  -  Indicates that the address bus contains the address of an output device and the data bus will
 *                    contain the output data when WR is active.
 * M1          D5  -  Provides a signal to indicate that the CPU is in the fetch cycle for the first byte of an
 *                    instruction.
 * INP         D6  -  Indicates that the address bus contains the address of an input device and the input data should
 *                    be placed on the data bus when DBIN is active.
 * MEMR        D7  -  Designates that the data bus will be used for memory read data.
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * *Data bus bit constants are named as their respective symbols (instead of D0-D7) for readability.
 *
 *
 * ## Machine Cycle Identification:
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 *
 *
 *
 * ## Control Output Definition:
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * SYNC  -  Marks the beginning of every machine cycle. Active from T1 until T2.
 */

class Intel8080 {
public:
    // pin bit constants
    static constexpr std::uint_fast64_t A0 {1ULL << 0ULL};
    static constexpr std::uint_fast64_t A1 {1ULL << 1ULL};
    static constexpr std::uint_fast64_t A2 {1ULL << 2ULL};
    static constexpr std::uint_fast64_t A3 {1ULL << 4ULL};
    static constexpr std::uint_fast64_t A4 {1ULL << 5ULL};
    static constexpr std::uint_fast64_t A5 {1ULL << 6ULL};
    static constexpr std::uint_fast64_t A6 {1ULL << 7ULL};
    static constexpr std::uint_fast64_t A7 {1ULL << 8ULL};
    static constexpr std::uint_fast64_t A8 {1ULL << 9ULL};
    static constexpr std::uint_fast64_t A9 {1ULL << 10ULL};
    static constexpr std::uint_fast64_t A10 {1ULL << 11ULL};
    static constexpr std::uint_fast64_t A11 {1ULL << 12ULL};
    static constexpr std::uint_fast64_t A12 {1ULL << 13ULL};
    static constexpr std::uint_fast64_t A13 {1ULL << 14ULL};
    static constexpr std::uint_fast64_t A14 {1ULL << 15ULL};
    static constexpr std::uint_fast64_t A15 {1ULL << 16ULL};
    static constexpr std::uint_fast64_t INTA {1ULL << 17ULL};
    static constexpr std::uint_fast64_t WO {1ULL << 18ULL};
    static constexpr std::uint_fast64_t STACK {1ULL << 19ULL};
    static constexpr std::uint_fast64_t HLTA {1ULL << 20ULL};
    static constexpr std::uint_fast64_t OUT {1ULL << 21ULL};
    static constexpr std::uint_fast64_t M1 {1ULL << 22ULL};
    static constexpr std::uint_fast64_t INP {1ULL << 23ULL};
    static constexpr std::uint_fast64_t MEMR {1ULL << 24ULL};
    static constexpr std::uint_fast64_t INTE {1ULL << 25ULL};
    static constexpr std::uint_fast64_t DBIN {1ULL << 26ULL};
    static constexpr std::uint_fast64_t WR {1ULL << 27ULL};
    static constexpr std::uint_fast64_t SYNC {1ULL << 28ULL};
    static constexpr std::uint_fast64_t HLDA {1ULL << 29ULL};
    static constexpr std::uint_fast64_t WAIT {1ULL << 30ULL};
    static constexpr std::uint_fast64_t RESET {1ULL << 31ULL};
    static constexpr std::uint_fast64_t HOLD {1ULL << 32ULL};
    static constexpr std::uint_fast64_t INT {1ULL << 33ULL};
    static constexpr std::uint_fast64_t READY {1ULL << 34ULL};

    // bus constants
    static constexpr std::uint_fast64_t abus {0xFFFFULL};
    static constexpr std::uint_fast64_t dbus {0xFF0000ULL};

    void tick();
    void reset() { pc_ = step_ = 0; }
    [[nodiscard]] std::uint_fast64_t getABus() const { return pins & abus; }
    [[nodiscard]] std::uint_fast64_t getDBus() const { return pins & dbus; }

    std::uint8_t status {0};
    std::uint_fast64_t pins {0};
private:
    // flag bit constants
    static constexpr std::uint8_t signBit {0x80U};
    static constexpr std::uint8_t zeroBit {0x40U};
    static constexpr std::uint8_t auxiliaryBit {0x10U};
    static constexpr std::uint8_t parityBit {0x04U};
    static constexpr std::uint8_t carryBit {0x01U};

    // register name constants
    static constexpr std::uint8_t B {0U};
    static constexpr std::uint8_t C {1U};
    static constexpr std::uint8_t D {2U};
    static constexpr std::uint8_t E {3U};
    static constexpr std::uint8_t H {4U};
    static constexpr std::uint8_t L {5U};
    static constexpr std::uint8_t W {9U};
    static constexpr std::uint8_t Z {10U};
    static constexpr std::uint8_t BC {B};
    static constexpr std::uint8_t DE {D};
    static constexpr std::uint8_t HL {H};
    static constexpr std::uint8_t WZ {W};
    static constexpr std::uint8_t A {7U};
    static constexpr std::uint8_t F {8U};

    void setDBus(std::uint_fast64_t val) { pins = pins & ~0xFF0000ULL | val<<16ULL; }
    void setABus(std::uint_fast64_t val) { pins = pins & ~0xFFULL | val; }

    void t2_()
    {
        if (pins & HOLD) {

        }
        status = getDBus();
        pins &= ~SYNC;
        if (!(pins & READY))
            pins |= WAIT;
    };
    void fetch_() { pins |= SYNC; setDBus(WO|M1|MEMR); setABus(pc_); }
    void readT1_(const std::uint16_t addr) { pins |= SYNC; setDBus(WO|MEMR); setABus(addr); }
    void readT2_() { t2_(); pins |= DBIN; }
    void writeT1_(const std::uint16_t addr) { pins |= SYNC; setDBus(0); setABus(addr); }
    void writeT2_(const std::uint8_t r) { t2_(); pins |= DBIN; setDBus(r); }
    void stackWriteT1_() { pins |= SYNC; setDBus(STACK); setABus(sp_); }
    void stackReadT1_() { pins |= SYNC; setDBus(WO|STACK|MEMR); setABus(sp_); }
    void stopDataIn() { pins &= ~DBIN; }

    [[nodiscard]] bool waiting_() const { return pins & WAIT; }
    std::uint16_t getPair_(const std::uint8_t rp) { return reg_[rp] << 8U | reg_[rp + 1U]; }
    void setPair_(const std::uint8_t rp, std::uint16_t val) { reg_[rp] = val >> 8U; reg_[rp + 1U] = val & 0xFF; }
    void transfer8_(std::uint8_t& dst) const { dst = getDBus(); }
    void transfer16_(const std::uint8_t rp) { reg_[rp << 1] = getDBus(); }
    [[nodiscard]] std::uint8_t rp_() const { return ir_ >> 4U & 0b111; }
    [[nodiscard]] std::uint8_t dst_() const { return ir_ >> 3U & 0b111000; }
    [[nodiscard]] std::uint8_t src_() const { return ir_ & 7U; }
    [[nodiscard]] std::uint8_t pch_() const { return pc_ >> 8U & 0xFF; }
    [[nodiscard]] std::uint8_t pcl_() const { return pc_ & 0xFF; }
    [[nodiscard]] std::uint8_t nnn_() const { return (ir_ & 0b111000) << 3U; }
    [[nodiscard]] bool ccc_() const;


    void setSignFlag(const bool enabled) { enabled ? reg_[F] |= signBit : reg_[F] &= ~signBit; }
    void setZeroFlag(const bool enabled) { enabled ? reg_[F] |= zeroBit : reg_[F] &= ~zeroBit; }
    void setAuxCarryFlag(const bool enabled) { enabled ? reg_[F] |= auxiliaryBit : reg_[F] &= ~auxiliaryBit; }
    void setParityFlag(const bool enabled) { enabled ? reg_[F] |= parityBit : reg_[F] &= ~parityBit; }
    void setCarryFlag(const bool enabled) { enabled ? reg_[F] |= carryBit : reg_[F] &= ~carryBit; }

    void add(const std::uint8_t addend) { preFlagsAdd(addend); reg_[A] += addend; postFlags(); }
    void sub(const std::uint8_t subtrahend) { preFlagsSub(subtrahend); reg_[A] -= subtrahend; postFlags(); }
    void inr(std::uint8_t& operand) { setAuxCarryFlag(((operand & 0xF) + 1U & 0x10)); ++operand; postFlags(); }
    void dcr(std::uint8_t& operand) { setAuxCarryFlag(((operand & 0xF) - 1U & 0x10)); --operand; postFlags(); }
    void ana(const std::uint8_t operand) { preFlagsAnd(operand); reg_[A] &= reg_[src_()]; postFlags(); }
    void ani(const std::uint8_t operand) { preFlagsLog(); reg_[A] &= operand; postFlags(); }
    void xra(const std::uint8_t operand) { preFlagsLog(); reg_[A] ^= operand; postFlags(); }
    void ora(const std::uint8_t operand) { preFlagsLog(); reg_[A] |= operand; postFlags(); }
    void cmp(const std::uint8_t operand)
    {
        const std::uint8_t res {static_cast<uint8_t>(reg_[A] - operand)};
        preFlagsSub(operand);
        setZeroFlag(reg_[A] == operand);
        setSignFlag(res & 0x8000U);
        setParityFlag(getParity8(res));
    }

    void preFlagsLog()
    {
        setAuxCarryFlag(false);
        setCarryFlag(false);
    }
    void preFlagsAnd(std::uint8_t operand)
    {
        setAuxCarryFlag(reg_[A] | operand & 0x4U);
        setCarryFlag(false);
    }
    void preFlagsAdd(std::uint8_t addend)
    {
        setAuxCarryFlag(((reg_[A] & 0xF) + (addend & 0xF) & 0x10));
        setCarryFlag( reg_[A] + addend > 0xFF);
    }
    void preFlagsSub(std::uint8_t subtrahend)
    {
        setAuxCarryFlag((reg_[A] & 0xF) - (subtrahend & 0xF) & 0x10);
        setCarryFlag( reg_[A] < subtrahend);
    }
    void postFlags()
    {
        setZeroFlag(!reg_[A]);
        setSignFlag(reg_[A] & 0x8000U);
        setParityFlag(getParity8(reg_[A]));
    }


    [[nodiscard]] static bool getParity8(std::uint8_t x)
    {
        x ^= x >> 4;
        x ^= x >> 2;
        x ^= x >> 1;
        return x;
    }

    // registers
    std::uint16_t pc_ {0}, sp_ {0};
    std::uint8_t ir_ {0}, tmp_ {0};
    std::uint8_t reg_[11] {};
    std::uint16_t step_ {0};
};

#endif //INTEL8080_INTEL8080_H
