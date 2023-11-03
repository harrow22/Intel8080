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
 *
 * The processor will announce what it needs on its pins every state. It expects external devices to notice this and
 * respond correctly. For example,
 *      - In a read cycle (when DBIN is active) you should load the data bus with data.
 *      - In a write cycle (when WR is active) you should store the data on the data bus at the location on the address
 *        bus.
 *
 * The only place my emulator "cheats" is by not overlapping certain instructions with the next fetch/decode cycle.
 * Instead, the overlapping portion is handled in the last state of that instruction. Also, I'm not emulating the
 * two-phase clock.
 *
 * For more information on individual instructions see chapter 4 of the Intel 8080 user manual.
 */
class Intel8080 {
public:
    // address pins
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

    // D0 - Acknowledge signal for INTERRUPT request. Signal should be used to gate a restart instruction onto the data
    //      bus when DBIN is active.
    static constexpr std::uint_fast64_t INTA {1ULL << 16ULL};

    // D1 - Indicates that the operation in the current machine cycle will be a WRITE memory or OUTPUT function (WO = 0).
    //      Otherwise a READ memory or INPUT operation will be executed.
    static constexpr std::uint_fast64_t WO {1ULL << 17ULL};

    // D2 - Indicates that the address bus holds the pushdown stack address from the Stack Pointer.
    static constexpr std::uint_fast64_t STACK {1ULL << 18ULL};

    // D3 - Acknowledge signal for HALT instruction.
    static constexpr std::uint_fast64_t HLTA {1ULL << 19ULL};

    // D4 - Indicates that the address bus contains the address of an output device and the data bus will contain the
    //      output data when WR is active.
    static constexpr std::uint_fast64_t OUT {1ULL << 20ULL};

    // D5 - Provides a signal to indicate that the CPU is in the fetch cycle for the first byte of an instruction.
    static constexpr std::uint_fast64_t M1 {1ULL << 21ULL};

    // D6 - Indicates that the address bus contains the address of an input device and the input data should be placed
    //      on the data bus when DBIN is active.
    static constexpr std::uint_fast64_t INP {1ULL << 22ULL};

    // D7 - Designates that the data bus will be used for memory read data.
    static constexpr std::uint_fast64_t MEMR {1ULL << 23ULL};

    // Internal interrupt enabled pin
    static constexpr std::uint_fast64_t INTE {1ULL << 24ULL};

    // Output control signal indicating that the processor is ready to accept data on the data bus
    static constexpr std::uint_fast64_t DBIN {1ULL << 25ULL};

    // Output control signal indicating that processor has loaded data onto the data bus to be written at location
    // on address bus
    static constexpr std::uint_fast64_t WR {1ULL << 26ULL};

    // Output control signal indicating the process is in the first state of a given machine cycle
    // You can detect the start of a new instruction when both M1 and SYNC pins are active.
    static constexpr std::uint_fast64_t SYNC {1ULL << 27ULL};

    // Output control signal indicating the process is currently waiting on an external device
    static constexpr std::uint_fast64_t WAIT {1ULL << 28ULL};

    // Input control signal, set high to indicate an interrupt to the processor. The next cycle will then have the
    // INTA pin active and the program counter will not be incremented. External devices should notice the INTA pin is
    // active and take appropriate steps. Intended to be used with RST n instruction. This pin will *not* be cleared
    // internally.
    static constexpr std::uint_fast64_t INT {1ULL << 29ULL};

    // Input control signal, set low to prior to the T3 state to tell the processor to wait. The processor will then
    // enter the wait state at the end of T2, where it will remain until the READY line is pulled high. Intended to
    // be used with coordinating data reads/writes. This pin will *not* be cleared internally.
    static constexpr std::uint_fast64_t READY {1ULL << 30ULL};

    // possible instruction cycle constants
    static constexpr  std::uint8_t instructionFetch {0b10100010U};
    static constexpr  std::uint8_t memoryRead {0b10000010U};
    static constexpr  std::uint8_t memoryWrite {0b00000000U};
    static constexpr  std::uint8_t stackRead {0b10000110U};
    static constexpr  std::uint8_t stackWrite {0b00000100U};
    static constexpr  std::uint8_t inputRead {0b01000010U};
    static constexpr  std::uint8_t outputWrite {0b00010000U};
    static constexpr  std::uint8_t interruptAck {0b00100011U};
    static constexpr  std::uint8_t haltAck {0b10001010U};
    static constexpr  std::uint8_t interruptAckWhileHalt {0b00101011U};

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

    // 16-bit one-directional address bus
    static constexpr std::uint_fast64_t abus {0xFFFFULL};

    // 8-bit bi-directional data bus
    static constexpr std::uint_fast64_t dbus {0xFF0000ULL};

    /**
     * Steps the processor one state forward. Each instruction consists of 1-5 machine cycles and 3-5 states (T1-T5)
     * constitute a machine cycle. A full instruction cycle requires anywhere from 4-18 states for its completion.
     *
     * If you want more information on how exactly this function works, see the timing charts and individual instruction
     * state charts in chapter 2 of the Intel 8080 user manual.
     */
    void tick();

    /**
     * Restores the processor's internal program counter to zero, and the cpu will begin the next cycle from T1. Note,
     * however, that this has no effect on status flags, or on any of the processor's working registers
     */
    void reset() { pc = step_ = 0; pins = 0ULL | READY; stopped_ = intff_ = intWhileHalt_ = false;  }

    /**
     * Overloaded function to prevent a common bug. The data bus pins are bits 16-24. This function left-shifts a byte
     * the correct amount to set the pins.
     * @param val a byte to set the data bus to
     */
    void setDBus(std::uint_fast8_t val) { pins = (pins & ~dbus) | (val << 16ULL); }

    /**
     * For setting the data bus pins with the named data bus pin constants defined above (D0-D7).
     * @param val a 64-bit number to set the data bus to
     */
    void setDBus(std::uint_fast64_t val) { pins = (pins & ~dbus) | val; }

    /**
     * Read the bits on the address bus.
     * @return the current value of the address bus
     */
    [[nodiscard]] std::uint16_t getABus() const { return pins & abus; }

    /**
     * Read the bits on the data bus.
     * @return a byte representing the current value of the data bus
     */
    [[nodiscard]] std::uint8_t getDBus() const { return (pins & dbus) >> 16ULL; }

    /**
     * If the result of an instruction has the value 0, this flag is set; otherwise it is reset.
     * @return 1 if the zero flag is set; 0 otherwise
     */
    [[nodiscard]] std::uint8_t z() const { return (f_ & zeroBit) >> 6U; }

    /**
     * If the modulo 2 sum of the bits of the result of the operation is 0, (i.e., if the result has even parity),
     * this flag is set; otherwise it is reset (i.e., if the result has odd parity).
     * @return 1 if the sign flag is set; 0 otherwise
     */
    [[nodiscard]] std::uint8_t s() const { return (f_ & signBit) >> 7U; }

    /**
     * If the most significant bit of the result of the operation has the value 1, this flag is set; otherwise it is
     * reset.
     * @return 1 if parity flag is set; 0 otherwise
     */
    [[nodiscard]] std::uint8_t p() const { return (f_ & parityBit) >> 2U; }

    /**
     * If the instruction resulted in a carry (from addition), or a borrow (from subtraction or a comparison) out of the
     * high-order bit, this flag is set; otherwise it is reset.
     * @return 1 if carry flag is set; 0 otherwise
     */
    [[nodiscard]] std::uint8_t cy() const { return f_ & carryBit; }

    /**
     * Auxiliary Carry: If the instruction caused a carry out of bit 3 and into bit 4 of the resulting value, the
     * auxiliary carry is set; otherwise it is reset.
     *
     * This flag is affected by single precision additions, subtractions, increments, decrements, comparisons, and
     * logical operations, but is principally used with additions and increments preceding a DAA
     * (Decimal Adjust Accumulator) instruction.
     * @return 1 if the auxiliary carry flag is set; 0 otherwise
     */
    [[nodiscard]] std::uint8_t ac() const { return (f_ & auxiliaryBit) >> 4U; }

    /**
     * For getting the value of a register based on register symbol names.
     * @param r a constant representing the register to get (see comment register name constants)
     * @return the value of the register
     */
    [[nodiscard]] std::uint8_t getReg(std::uint8_t) const;

    /**
     * For getting the value of a register pair based on register pair symbol names
     * @param rp a constant representing the register to get (see comment register name constants)
     * @return the value of the register pair
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
    std::uint8_t status {0U};

    // the cpu's pins (see pinout at top of header file)
    std::uint_fast64_t pins {0ULL | READY};

    // pointer to internal instruction register for debugging
    const std::uint8_t& ir {ir_};
private:
    // flag bit constants
    static constexpr std::uint8_t zeroBit {0b01000000U};
    static constexpr std::uint8_t signBit {0b10000000U};
    static constexpr std::uint8_t parityBit {0b00000100U};
    static constexpr std::uint8_t carryBit {0b00000001U};
    static constexpr std::uint8_t auxiliaryBit {0b00010000U};

    // set flag functions
    void setZeroFlag_(const bool enabled) { enabled ? f_ |= zeroBit : f_ &= ~zeroBit; }
    void setSignFlag_(const bool enabled) { enabled ? f_ |= signBit : f_ &= ~signBit; }
    void setParityFlag_(const bool enabled) { enabled ? f_ |= parityBit : f_ &= ~parityBit; }
    void setCarryFlag_(const bool enabled) { enabled ? f_ |= carryBit : f_ &= ~carryBit; }
    void setAuxCarryFlag_(const bool enabled) { enabled ? f_ |= auxiliaryBit : f_ &= ~auxiliaryBit; }

    // symbol functions (see ch4 intel 8080 data sheet)
    [[nodiscard]] std::uint8_t rp_() const { return (ir_ & 0b110000U) >> 4U; }
    [[nodiscard]] std::uint8_t dst_() const { return (ir_ & 0b111000U) >> 3U; }
    [[nodiscard]] std::uint8_t src_() const { return ir_ & 0b111U; }
    [[nodiscard]] std::uint8_t nnn_() const { return (ir_ & 0b111000) >> 3U; }
    [[nodiscard]] std::uint8_t psw_() const { return f_; }
    [[nodiscard]] bool ccc_() const;

    // common pin manipulation functions
    void setABus_(std::uint16_t val) { pins = pins & ~0xFFFFULL | val; }
    void stopDataIn_() { pins &= ~DBIN; }
    void stopDataOut_() { pins &= ~WR; }
    [[nodiscard]] bool waiting_() const { return pins & WAIT; }

    // helper functions
    void setReg_(std::uint8_t, std::uint8_t);
    void setHi_(const std::uint8_t rp, std::uint8_t val) { pair_[rp] = (pair_[rp] & 0x00FF) | (val << 8U); }
    void setLo_(const std::uint8_t rp, std::uint8_t val) { pair_[rp] = (pair_[rp] & 0xFF00) | val; }
    [[nodiscard]] static std::uint8_t hi_(const std::uint16_t val) { return (val & 0xFF00) >> 8U; }
    [[nodiscard]] static std::uint8_t lo_(const std::uint16_t val) { return val & 0xFF; }

    // state functions
    void t1_(); // ONLY TO BE CALLED AFTER setDBus() CALL
    void t2_();;
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
    void add_(std::uint8_t);
    void adc_(std::uint8_t);
    void sub_(std::uint8_t);
    void sbb_(std::uint8_t);
    std::uint8_t inr_(std::uint8_t);
    std::uint8_t dcr_(std::uint8_t);
    void ana_(std::uint8_t);
    void ani_(std::uint8_t);
    void xra_(std::uint8_t);
    void ora_(std::uint8_t);
    void cmp_(std::uint8_t);

    // flag helper functions
    void carryFlagsAlg_();
    void carryFlagsAnd_(std::uint8_t);
    void carryFlagsAdd_(std::uint8_t, std::uint8_t cy= 0);
    void carryFlagsSub_(std::uint8_t, std::uint8_t cy= 0);
    void zspFlags_(std::uint8_t);

    std::uint16_t step_ {0};
    bool stopped_ {false};
    bool intWhileHalt_ {false};
    bool intff_ {false};

    // registers
    std::uint8_t ir_ {0}, tmp_ {0};
    std::uint8_t a_ {0}, f_ {0b10U};
    std::uint16_t pair_[5] {};
};

#endif //INTEL8080_INTEL8080_H
