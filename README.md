# risc-rust

Usage: cargo run -- ./to_board/<filename>.rs
or: cargo run -- /to_board/<filename>.rs /to_board/<filename>.hex

TODOs:
- Decide on open source processor, find frontend, backend, optimiser
- Figure out how to send instructions to processor (JTAG)
- Memory mapped IO
- Testing infrastructure

Making a rust compiler to a risc architecture that can
be used to make a DE1-Soc board function as an embedded system.

Using standard LLVM passes and a rust frontend.

A Risc V open source processor will be used.

Done by Jonathan Schneider and Seva Alipov