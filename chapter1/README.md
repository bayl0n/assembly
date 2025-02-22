# Chapter 1 Notes

- The book (and subsequently this repo) is going to cover x86 assembly and the GNU/Linux operating system
- This repo is run on Windows WSL running an Ubuntu distro
- What is GNU/Linux?
    - GNU/Linux is an operating system modeled after UNIX
    - The GNU part comes from the GNU project
    - The GNU project contains most of the programs run in the book and most importantly the GCC tool set that will be used to program with
- Linux is the name of the *kernel*
- The kernel is the core part of an operating system that keeps track of everything
    - It allows programs to access hardware in a uniform way (so the end user doesn't need to write programs for every deviced that is interfaced on the computer).
    - The kernel handles all device-specific interactions
    - It also handles file access and interaction between processes
    - For example, the general flow for how a computer processes keyboard signals would look like this:
    - *Keyboard* -> *Kernel* -> *Windowing system* -> *Application program*
    - The kernel also controls the flow of information between programs.
    - The kernel is a program's gate to the world around it.
    - Every time that data moves between processes, the kernel controls the messaging.
    - e.g. In the keyboard example, the kernel would be involved for the windowing system to communicate the keypress to the application program
    - As a fence, the kernel prevents programs from accidentally overwriting each other's data and from accessing files and devices that they don't have permission to
    - It limits the amount of damage a poorly-written program can do to other running programs
- When it comes to computers there are three kinds of languages:
    - Machine Language
        - This is what the computer actually sees and deals with. Every command the computer sees is given as a number or sequence of numbers.
    - Assembly Language
        - This is the same as the machine language, except the command numbers have been replaced by letter sequences which are easier to memorize.
        - Other small things are done to make it easier as well
    - High-Level Language
        - High-level languages are there to make programming easier. Assembly language rquires you to work with the machine itself.
        - High-level languages allow you to describe the program in a more natural language.
        - A single command in a high-level language usually is equivalent to several commands in an assembly language
