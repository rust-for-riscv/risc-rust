vlib work
vlog simple_memory.v
vlog picorv32.v
vlog RiscVRustToolchain.v

vsim RiscVRustToolchain

# Add a few important signals only
add wave CLOCK_50
add wave LEDR

force CLOCK_50 -repeat 20ns 0 0ns, 1 10ns

run 1000ns