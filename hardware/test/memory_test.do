vlib work
vlog simple_memory.v

vsim simple_memory

# Add a few important signals only
add wave *

# Force clocks (correct comma syntax)
force clk -repeat 20ns 0 0ns, 1 10ns
force mem_valid -repeat 20ns 1 5ns, 0 15ns
force mem_wstrb 4'b0000
force -deposit mem_wdata 10'b0101010101

force -deposit resetn 0 0ns, 1 20ns
force -deposit mem_addr 32'hff200000 145ns


force -deposit mem_addr 32'h0 25ns

force -deposit mem_addr 32'h4 65ns

force -deposit mem_addr 32'hc 105ns

run 1000ns
