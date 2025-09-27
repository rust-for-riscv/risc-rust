vlib work
vlog RiscVRustToolchain.v picorv32.v simple_memory.v

vsim -voptargs=+acc work.RiscVRustToolchain

# Add a few important signals only
add wave /RiscVRustToolchain/CLOCK_50
add wave /RiscVRustToolchain/resetn
add wave /RiscVRustToolchain/LEDR

# Force clocks (correct comma syntax)
force -repeat 20ns /RiscVRustToolchain/CLOCK_50 0 0ns, 1 10ns

# Set inputs to zero
force /RiscVRustToolchain/KEY 0x0
force /RiscVRustToolchain/SW 0x0

run 1000ns
