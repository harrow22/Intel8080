#ifndef INTEL8080_INTEL8080_H
#define INTEL8080_INTEL8080_H

#include <cstdint>

/*
 * Intel 8080 Emulated Pinout:
 *             ┌───────────────────┐
 *    A0 ○<————│  1             31 │<————○ READY
 *    A1 ○<————│  2             30 │<————○ INT
 *    A2 ○<————│  3             29 │————>○ WAIT
 *    A3 ○<————│  4             28 │————>○ SYNC
 *    A4 ○<————│  5             27 │————>○ WR
 *    A5 ○<————│  6    INTEL    26 │————>○ DBIN
 *    A6 ○<————│  7     8080    25 │————>○ INTE
 *    A7 ○<————│  8             24 │<———>○ D7
 *    A8 ○<————│  9             23 │<———>○ D6
 *    A9 ○<————│ 10             22 │<———>○ D5
 *   A10 ○<————│ 11             21 │<———>○ D4
 *   A11 ○<————│ 12             20 │<———>○ D3
 *   A12 ○<————│ 13             19 │<———>○ D2
 *   A13 ○<————│ 14             18 │<———>○ D1
 *   A14 ○<————│ 15             17 │<———>○ D0
 *   A15 ○<————│ 16                │
 *             └───────────────────┘
 * See README for usage.
 */

class Intel8080 {
public:
    // pin bit constants
    static constexpr std::uint_fast64_t A0 {1ULL << 0ULL};
    static constexpr std::uint_fast64_t A1 {1ULL << 1ULL};
    static constexpr std::uint_fast64_t A2 {1ULL << 2ULL};
    static constexpr std::uint_fast64_t A3 {1ULL << 3ULL};
    static constexpr std::uint_fast64_t A4 {1ULL << 4ULL};
    static constexpr std::uint_fast64_t A5 {1ULL << 5ULL};
    static constexpr std::uint_fast64_t A6 {1ULL << 6ULL};
    static constexpr std::uint_fast64_t A7 {1ULL << 7ULL};
    static constexpr std::uint_fast64_t A8 {1ULL << 8ULL};
    static constexpr std::uint_fast64_t A9 {1ULL << 9ULL};
    static constexpr std::uint_fast64_t A10 {1ULL << 10ULL};
    static constexpr std::uint_fast64_t A11 {1ULL << 11ULL};
    static constexpr std::uint_fast64_t A12 {1ULL << 12ULL};
    static constexpr std::uint_fast64_t A13 {1ULL << 13ULL};
    static constexpr std::uint_fast64_t A14 {1ULL << 14ULL};
    static constexpr std::uint_fast64_t A15 {1ULL << 15ULL};
    static constexpr std::uint_fast64_t INTA {1ULL << 16ULL};
    static constexpr std::uint_fast64_t WO {1ULL << 17ULL};
    static constexpr std::uint_fast64_t STACK {1ULL << 18ULL};
    static constexpr std::uint_fast64_t HLTA {1ULL << 19ULL};
    static constexpr std::uint_fast64_t OUT {1ULL << 20ULL};
    static constexpr std::uint_fast64_t M1 {1ULL << 21ULL};
    static constexpr std::uint_fast64_t INP {1ULL << 22ULL};
    static constexpr std::uint_fast64_t MEMR {1ULL << 23ULL};
    static constexpr std::uint_fast64_t INTE {1ULL << 24ULL};
    static constexpr std::uint_fast64_t DBIN {1ULL << 25ULL};
    static constexpr std::uint_fast64_t WR {1ULL << 26ULL};
    static constexpr std::uint_fast64_t SYNC {1ULL << 27ULL};
    static constexpr std::uint_fast64_t WAIT {1ULL << 28ULL};
    static constexpr std::uint_fast64_t INT {1ULL << 29ULL};
    static constexpr std::uint_fast64_t READY {1ULL << 30ULL};

    // register name constants
    static constexpr std::uint8_t B {0U};
    static constexpr std::uint8_t C {1U};
    static constexpr std::uint8_t D {2U};
    static constexpr std::uint8_t E {3U};
    static constexpr std::uint8_t H {4U};
    static constexpr std::uint8_t L {5U};
    static constexpr std::uint8_t A {7U};
    static constexpr std::uint8_t F {8U};
    static constexpr std::uint8_t BC {0U};
    static constexpr std::uint8_t DE {1U};
    static constexpr std::uint8_t HL {2U};
    static constexpr std::uint8_t SP {3U};
    static constexpr std::uint8_t WZ {4U};

    // bus constants
    static constexpr std::uint_fast64_t abus {0xFFFFULL};
    static constexpr std::uint_fast64_t dbus {0xFF0000ULL};

    // TODO: fill in doc comments
    /**
     *
     */
    void tick();

    /**
     *
     */
    void reset() { pc = step_ = 0; stopped = false; }

    /**
     *
     * @param val
     */
    void setDBus(std::uint_fast8_t val) { pins = (pins & ~dbus) | (val << 16ULL); }

    /**
     *
     * @param val
     */
    void setDBus(std::uint_fast64_t val) { pins = (pins & ~dbus) | val; }

    /**
     *
     * @return
     */
    [[nodiscard]] std::uint16_t getABus() const { return pins & abus; }

    /**
     *
     * @return
     */
    [[nodiscard]] std::uint8_t getDBus() const { return (pins & dbus) >> 16ULL; }

    /**
     *
     * @return
     */
    [[nodiscard]] std::uint8_t cy() const { return f_ & carryBit; }

    /**
     *
     * @return
     */
    [[nodiscard]] std::uint8_t p() const { return (f_ & parityBit) >> 2U; }

    /**
     *
     * @return
     */
    [[nodiscard]] std::uint8_t ac() const { return (f_ & auxiliaryBit) >> 4U; }

    /**
     *
     * @return
     */
    [[nodiscard]] std::uint8_t z() const { return (f_ & zeroBit) >> 6U; }

    /**
     *
     * @return
     */
    [[nodiscard]] std::uint8_t s() const { return (f_ & signBit) >> 7U; }

    /**
     *
     * @param
     * @return
     */
    [[nodiscard]] std::uint8_t getReg(std::uint8_t) const;

    /**
     *
     * @param rp
     * @return
     */
    [[nodiscard]] std::uint16_t getPair(const std::uint8_t rp) const { return pair_[rp]; }

    // the cpu's program counter
    std::uint16_t pc {0};

    /* the current cpu status word
     * 10100010: Instruction Fetch
     * 10000010: Memory Read
     * 00000000: Memory Write
     * 10000110: Stack Read
     * 00000100: Stack Write
     * 01000010: Input Read
     * 00010000: Output Write
     * 00100011: Interrupt Acknowledge
     * 10001010: Halt Acknowledge
     * 00101011: Interrupt Acknowledge While Halted
     */
    std::uint8_t status {0};

    // the cpu's pins (see pinout at top of header file)
    std::uint_fast64_t pins {0 | READY};

    // pointer to internal instruction register for debugging
    const std::uint8_t& ir {ir_};
private:
    // flag bit constants
    static constexpr std::uint8_t signBit {0b10000000U};
    static constexpr std::uint8_t zeroBit {0b01000000U};
    static constexpr std::uint8_t auxiliaryBit {0b00010000U};
    static constexpr std::uint8_t parityBit {0b00000100U};
    static constexpr std::uint8_t carryBit {0b00000001U};

    // set flag functions
    void setSignFlag(const bool enabled) { enabled ? f_ |= signBit : f_ &= ~signBit; }
    void setZeroFlag(const bool enabled) { enabled ? f_ |= zeroBit : f_ &= ~zeroBit; }
    void setAuxCarryFlag(const bool enabled) { enabled ? f_ |= auxiliaryBit : f_ &= ~auxiliaryBit; }
    void setParityFlag(const bool enabled) { enabled ? f_ |= parityBit : f_ &= ~parityBit; }
    void setCarryFlag(const bool enabled) { enabled ? f_ |= carryBit : f_ &= ~carryBit; }

    // symbol functions (see ch4 intel 8080 data sheet)
    [[nodiscard]] std::uint8_t rp_() const { return (ir_ & 0b110000U) >> 4U; }
    [[nodiscard]] std::uint8_t dst_() const { return (ir_ & 0b111000U) >> 3U; }
    [[nodiscard]] std::uint8_t src_() const { return ir_ & 0b111U; }
    [[nodiscard]] std::uint8_t nnn_() const { return (ir_ & 0b111000) >> 3U; }
    [[nodiscard]] std::uint8_t psw_() const { return f_; }
    [[nodiscard]] bool ccc_() const;

    // common pin manipulation functions
    void setABus(std::uint16_t val) { pins = pins & ~0xFFFFULL | val; }
    void stopDataIn() { pins &= ~DBIN; }
    void stopDataOut() { pins &= ~WR; }
    [[nodiscard]] bool waiting_() const { return pins & WAIT; }
    [[nodiscard]] bool interrupted_() const { return pins & (INT | INTE); }

    // helper functions
    void setReg(std::uint8_t, std::uint8_t);
    void setHi_(const std::uint8_t rp, std::uint8_t val) { pair_[rp] = (pair_[rp] & 0x00FF) | (val << 8U); }
    void setLo_(const std::uint8_t rp, std::uint8_t val) { pair_[rp] = (pair_[rp] & 0xFF00) | val; }
    [[nodiscard]] static std::uint8_t hi_(const std::uint16_t val) { return (val & 0xFF00) >> 8U; }
    [[nodiscard]] static std::uint8_t lo_(const std::uint16_t val) { return val & 0xFF; }

    // state functions
    void t1_(); // ONLY TO BE CALLED AFTER setDBus() CALL
    void t2_();;
    void fetchT1_();
    void readT1_(std::uint16_t);
    void writeT1_(std::uint16_t);
    void stackWriteT1_();
    void stackWriteT1_(std::uint16_t);
    void stackReadT1_();
    void stackReadT1_(std::uint16_t);
    void inputReadT1_();
    void outputWriteT1_();
    void readT2_();
    void writeT2_(std::uint8_t);

    // arithmetic & logical functions
    void add(std::uint8_t);
    void adc(std::uint8_t);
    void sub(std::uint8_t);
    void sbb(std::uint8_t);
    std::uint8_t inr(std::uint8_t);
    std::uint8_t dcr(std::uint8_t);
    void ana(std::uint8_t);
    void ani(std::uint8_t);
    void xra(std::uint8_t);
    void ora(std::uint8_t);
    void cmp(std::uint8_t);

    // flag helper functions
    void carryFlagsAlg();
    void carryFlagsAnd(std::uint8_t);
    void carryFlagsAdd(std::uint8_t, std::uint8_t cy=0);
    void carryFlagsSub(std::uint8_t, std::uint8_t cy=0);
    void zspFlags(std::uint8_t);

    std::uint16_t step_ {0};
    bool stopped {false};

    // registers
    std::uint8_t ir_ {0}, tmp_ {0};
    std::uint8_t a_ {0}, f_ {0b10U};
    std::uint16_t pair_[5] {};
};

#endif //INTEL8080_INTEL8080_H
