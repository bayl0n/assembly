# Chapter 2

- Modern computer architecture is based off of an architecture call the **Von Neumman** architecture
- The Vonn Neumman architecture divides the computer into two main parts - the CPU (for Central Processing Unit) and the memory.
- This architecture is used in all modern computers, including personal computers, supercomputers, mainframes, and even cell phones

## Structure of the Memory

- Memory is organized in addresses in numbered sequences of fixed sizes that can only store a single number
- The computer's memory is used for a number of different things: results of calculations, location of the cursor on the screen, window sizes, etc.
- The Von Neumman architecture also specifies that programs that control the computer's operation should live there too
    - In fact, when reading off memory, a computer can't tell the different between a number and an instruction unless it is pointed to by an instruction pointer
    - Instructions and numbers are both stored and accessed the same way


## The CPU

- The CPU reads in instructions from memory one at a time and executes them. This is known as the **fetch-execute cycle**
- It contains the following elements:
    - Program Counter
    - Instruction Decoder
    - Data bus
    - General-purpose registers
    - Arithmetic and logic unit
- The *program counter* is used to tell the computer where to fetch the next instruction from
    - It holds the memory address of the next instruction to be executed
- The CPU begins by looking at the program counter, and fetching whatever number is stored in memory at the location specified
- It is then passed on to the *instruction decoder* which figures out what the instruction means.
- This includes what process needs to take place (addition, subtraction, multiplication, data movement, etc.)
- Computer instructions usually consist of both the actual instruction and the list of memory locations that are used to carry it out
- The computer then uses the *data bus* to fetch the memory locations to be used in the calculation. The data bus is the connection between the CPU and memory.
    - It is the actual wire that connects them!
- If you look at the motherboard of the computer, the wires that go out from the memory are your data bus
- In addition to memory outside of the processor, the processor itself has special, high-speed memory locations called *registers*
- There are two kinds of registers - *general registers* and *special-purpose registers*
- General-purpose registers are where the main action happens.
    - Addition, subtraction, mutliplication, comparisons, and other operations generally use these registers for processing
    - However, computers have very few general-purpose registers
- Most information is stored in main memory, brought in to the registers for processing, and then put back into memory when the processing is completed
- *Special-purpose registers* are registers which have very specific purposes (duh, jk will be discussed later)
- When all the needed data has been retrieved, it passes on the data and the decoded instruction to the *arithmetic and logic unit* (ALU) for further processing
- The ALU is where the instruction is actually executed.
- After the results of the computation have been calculated, the results are then placed on the data bus and sent to the appropriate location in memory or in a register, as specified by the instruction
- However, modern CPUs have advanced quite a bit in recent time and are now much more complex
    - The basic operation is still the same, but it is complicated by the use of cache hierarchies, superscalar processors, pipelining, branch prediction, out-of-order execution, microcode translation, coprocessors, and other optimizations

## Some Terms
- Computer memory is a numbered swquence of fixed-size storage programs
- The number attached to each storage location is called it's *address*
- The size of a single storage location is called a *byte*
    - On x86 processors, a byte is a number between 0 and 255
- These numbers can represent different things on a computer, such as a character using an ASCII code
    - e.g. 'A' is represented using the number 65
- To represent larger numbers, a combination of bytes can be used
    - TWo bytes can be used to represent any number between 0 and 65536
- It can be difficult to stick bytes together to represent numbers and requires math
    - Luckily, some computer architectures can do the calculations for four-bytes by default
- *Registers* are a special-purpose storage containers that the computer uses for computation
    - They represent what the computer is currently working on/computing
- On the x86 computer architecture, registers are four bytes long
- The size of a typical register is called a computer's **word** size
- x86 processors have four-byte words
- This means that it is most natural on these computers to do computations four bytes at a time
    - This comes out to roughly 4 billion values
- Addresses are also four bytes (1 word) long, and also fit in a register
