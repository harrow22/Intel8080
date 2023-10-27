#include <fstream>
#include <iostream>
#include <format>
#include <string>
#include <string_view>
#include <chrono>
#include "Intel8080.h"

using Memory = std::array<std::uint8_t, 0x10000>;
namespace MachineCycle {
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
}

static constexpr std::string testDirectory {"../tests/"};
static constexpr std::string disassambleTable[256] = {
        "nop", "lxi b,#", "stax b", "inx b",
        "inr b", "dcr b", "mvi b,#", "rlc", "ill", "dad b", "ldax b", "dcx b",
        "inr c", "dcr c", "mvi c,#", "rrc", "ill", "lxi d,#", "stax d", "inx d",
        "inr d", "dcr d", "mvi d,#", "ral", "ill", "dad d", "ldax d", "dcx d",
        "inr e", "dcr e", "mvi e,#", "rar", "ill", "lxi h,#", "shld", "inx h",
        "inr h", "dcr h", "mvi h,#", "daa", "ill", "dad h", "lhld", "dcx h",
        "inr l", "dcr l", "mvi l,#", "cma", "ill", "lxi sp,#", "sta $", "inx sp",
        "inr M", "dcr M", "mvi M,#", "stc", "ill", "dad sp", "lda $", "dcx sp",
        "inr a", "dcr a", "mvi a,#", "cmc", "mov b,b", "mov b,c", "mov b,d",
        "mov b,e", "mov b,h", "mov b,l", "mov b,M", "mov b,a", "mov c,b", "mov c,c",
        "mov c,d", "mov c,e", "mov c,h", "mov c,l", "mov c,M", "mov c,a", "mov d,b",
        "mov d,c", "mov d,d", "mov d,e", "mov d,h", "mov d,l", "mov d,M", "mov d,a",
        "mov e,b", "mov e,c", "mov e,d", "mov e,e", "mov e,h", "mov e,l", "mov e,M",
        "mov e,a", "mov h,b", "mov h,c", "mov h,d", "mov h,e", "mov h,h", "mov h,l",
        "mov h,M", "mov h,a", "mov l,b", "mov l,c", "mov l,d", "mov l,e", "mov l,h",
        "mov l,l", "mov l,M", "mov l,a", "mov M,b", "mov M,c", "mov M,d", "mov M,e",
        "mov M,h", "mov M,l", "hlt", "mov M,a", "mov a,b", "mov a,c", "mov a,d",
        "mov a,e", "mov a,h", "mov a,l", "mov a,M", "mov a,a", "add b", "add c",
        "add d", "add e", "add h", "add l", "add M", "add a", "adc b", "adc c",
        "adc d", "adc e", "adc h", "adc l", "adc M", "adc a", "sub b", "sub c",
        "sub d", "sub e", "sub h", "sub l", "sub M", "sub a", "sbb b", "sbb c",
        "sbb d", "sbb e", "sbb h", "sbb l", "sbb M", "sbb a", "ana b", "ana c",
        "ana d", "ana e", "ana h", "ana l", "ana M", "ana a", "xra b", "xra c",
        "xra d", "xra e", "xra h", "xra l", "xra M", "xra a", "ora b", "ora c",
        "ora d", "ora e", "ora h", "ora l", "ora M", "ora a", "cmp b", "cmp c",
        "cmp d", "cmp e", "cmp h", "cmp l", "cmp M", "cmp a", "rnz", "pop b",
        "jnz $", "jmp $", "cnz $", "push b", "adi #", "rst 0", "rz", "ret", "jz $",
        "ill", "cz $", "call $", "aci #", "rst 1", "rnc", "pop d", "jnc $", "out p",
        "cnc $", "push d", "sui #", "rst 2", "rc", "ill", "jc $", "in p", "cc $",
        "ill", "sbi #", "rst 3", "rpo", "pop h", "jpo $", "xthl", "cpo $", "push h",
        "ani #", "rst 4", "rpe", "pchl", "jpe $", "xchg", "cpe $", "ill", "xri #",
        "rst 5", "rp", "pop psw", "jp $", "di", "cp $", "push psw", "ori #",
        "rst 6", "rm", "sphl", "jm $", "ei", "cm $", "ill", "cpi #", "rst 7"};

void log (Intel8080& intel8080, const Memory& memory, unsigned long currentCycle)
{
    std::cout << std::format(
            "PC: {:0>4X}, AF: {:0>4X}, BC: {:0>4X}, DE: {:0>4X}, HL: {:0>4X}, SP: {:0>4X}, CYC: {:d}\t({:0>2X} {:0>2X} {:0>2X} {:0>2X}) - {:s}\n",
            intel8080.pc,
            intel8080.getReg(Intel8080::A) << 8U | intel8080.getReg(Intel8080::F),
            intel8080.getPair(Intel8080::BC),
            intel8080.getPair(Intel8080::DE),
            intel8080.getPair(Intel8080::HL),
            intel8080.getPair(Intel8080::SP),
            currentCycle - 1,
            memory[intel8080.pc],
            memory[intel8080.pc + 1U],
            memory[intel8080.pc + 2U],
            memory[intel8080.pc + 3U],
            disassambleTable[memory[intel8080.pc]]);
}

int loadFile(Memory& memory, const std::string& path, int addr)
{
    std::ifstream file {path, std::ios::binary};
    if (!file) {
        std::cerr << std::format("error: can't open file '{:s}'.\n", path);
        return -1;
    }
    file.read(reinterpret_cast<char*>(&memory[addr]), memory.size());

    return 0;
}

void test(Intel8080& intel8080, const std::string& testName, unsigned long expectedCycles, bool debug, bool verbose)
{
    Memory memory {};
    if (loadFile(memory, testDirectory + testName, 0x100) < 0)
        return;
    std::cout << std::format("*** TEST: {:s}\n", testName);

    // inject "out 0,a" at 0x0000 (signal to stop the test)
    memory[0x0000] = 0xD3;
    memory[0x0001] = 0x00;

    // inject "out 1,a" at 0x0005 (signal to output some characters)
    memory[0x0005] = 0xD3;
    memory[0x0006] = 0x01;
    memory[0x0007] = 0xC9;

    // reset the cpu for the next test
    intel8080.reset();
    intel8080.pc = 0x100U;

    bool testRunning {true};
    unsigned long executedCycles {0};
    long instructions {0};
    std::chrono::steady_clock::time_point begin {std::chrono::steady_clock::now()};

    while (testRunning) {
        intel8080.tick();
        ++executedCycles;

        switch (intel8080.status) {
            case MachineCycle::instructionFetch:
                if (intel8080.pins & Intel8080::SYNC) {
                    ++instructions;
                    if (debug)
                        log(intel8080, memory,  executedCycles);
                } else if (intel8080.pins & Intel8080::DBIN) {
                    intel8080.setDBus(memory[intel8080.getABus()]);
                    if (debug and verbose)
                        std::cout << std::format(
                                "\tFETCH CYCLE\t[{:s}]: abus={:0>4X}, dbus={:0>2X}\n",
                                disassambleTable[memory[intel8080.getABus()]],
                                intel8080.getABus(),
                                intel8080.getDBus());
                }
                break;
            case MachineCycle::memoryRead: case MachineCycle::stackRead:
                if (intel8080.pins & Intel8080::DBIN) {
                    intel8080.setDBus(memory[intel8080.getABus()]);
                    if (debug and verbose)
                        std::cout << std::format(
                                "\tREAD CYCLE\t[{:s}]: abus={:0>4X}, dbus={:0>2X}\n",
                                disassambleTable[intel8080.ir],
                                intel8080.getABus(),
                                intel8080.getDBus());
                }
                break;
            case MachineCycle::memoryWrite: case MachineCycle::stackWrite:
                if (intel8080.pins & Intel8080::WR) {
                    memory[intel8080.getABus()] = intel8080.getDBus();
                    if (debug and verbose)
                        std::cout << std::format(
                                "\tWRITE CYCLE\t[{:s}]: abus={:0>4X}, dbus={:0>2X}, mem={:0>2X}\n",
                                disassambleTable[intel8080.ir],
                                intel8080.getABus(),
                                intel8080.getDBus(),
                                memory[intel8080.getABus()]);
                }
                break;
            case MachineCycle::inputRead:
                if (intel8080.pins & Intel8080::DBIN)
                    intel8080.setDBus(0ULL);
                break;
            case MachineCycle::outputWrite:
                if (intel8080.pins & Intel8080::WR) {
                    std::uint16_t port {intel8080.getABus()};
                    if (port == 0) {
                        testRunning = false;
                    } else if (port == 1) {
                        const std::uint8_t operation {intel8080.getReg(Intel8080::C)};
                        if (operation == 9) { // print from memory at (DE) until '$' char
                            std::uint16_t addr {intel8080.getPair(Intel8080::DE)};
                            do {
                                std::cout << (char) memory[addr++];
                            } while (memory[addr] != '$');
                        } else if (operation == 2 or operation == 5) { // print a character stored in E
                            std::cout << (char) intel8080.getReg(Intel8080::E);
                        }
                    }
                }
                break;
            case MachineCycle::interruptAck: case MachineCycle::haltAck: case MachineCycle::interruptAckWhileHalt:
                // interrupt+halts acknowledges not implemented
                break;
            default:
                std::cerr << std::format("\tUnrecognized machine cycle status word '{:b}'\n", intel8080.status);
                break;
        }
    }
    ++executedCycles; // +1 because loop ended on last cycle

    long long diff {expectedCycles > executedCycles ? expectedCycles - executedCycles : executedCycles - expectedCycles};
    std::cout << std::format("\n*** {:d} instructions executed on {:d} cycles (expected={:d}, diff={:d}) in {:d}ms\n\n",
           instructions, executedCycles, expectedCycles, diff,
           std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count());
}

int main(int argc, char** argv)
{
    bool debug {false};
    bool verbose {false};

    // simple command line parsing
    using namespace std::string_view_literals;
    for (int i {1}; i < argc; ++i) {
        if (argv[i] == "-debug"sv) {
            debug = true;
        } else if (argv[i] == "-v"sv) {
            verbose = true;
        } else {
            std::cout << std::format("Unrecognized command line argument '{:s}'.\nAvailable arguments are:\n\tenable logging: -debug\n\tenable verbose logging: -v\n\n", argv[i]);
        }
    }
    verbose = (verbose and debug); // verbose only makes sense if debug is also enabled

    Intel8080 intel8080 {};
    std::chrono::steady_clock::time_point begin {std::chrono::steady_clock::now()};

    test(intel8080, "TST8080.COM", 4924LU, debug, verbose);
    //test(intel8080, "8080PRE.COM", 7817LU, debug);
    //test(intel8080, "CPUTEST.COM", 255653383LU, debug);
    //test(intel8080, "8080EXM.COM", 23803381171LU, debug);

    std::cout << "Total time elapsed " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() << "ms" << std::endl;

    return 0;
}