module simple_memory #(
    parameter MEMSIZE = 32 // Number of words in memory
)(
    input  wire        clk,
    input  wire        resetn,

    // PicoRV32 memory interface
    input  wire        mem_valid,
    input  wire        mem_instr,
    output reg         mem_ready,
    input  wire [31:0] mem_addr,
    input  wire [31:0] mem_wdata,
    input  wire [3:0]  mem_wstrb,
    output reg  [31:0] mem_rdata,

    //IO
    output reg [9:0] LEDR,
    input  [9:0] SW
);

    // Assigning only the exact amount of memory needed
    reg [31:0] memory [MEMSIZE - 1:0];
    reg [31:0] current_write_data;

    // Initialize memory from firmware.hex
    initial begin
        $readmemh("example.hex", memory);
    end

    // Address decoding (word-aligned)

    wire [9:0] word_addr = mem_addr[11:2];  // 2^10 = 1024 words

    wire [31:0] memory_mapped_out;

    wire IO_inst;
	 assign IO_inst = (mem_addr[31:20] == 12'hFF2);
	 
	 wire [19:0] memory_mapped_addr;
    assign memory_mapped_addr = mem_addr[19:0];

    always @(posedge clk) begin
        mem_ready <= 0;
        if (mem_valid && !mem_ready) begin
            mem_ready <= 1;

            if(mem_addr[31:20] == 12'hFF2)begin
                memory_mapped_outputs(
                    .LEDR(LEDR),
                    .SW(SW),
                    .mem_addr(mem_addr),
                    .mem_rdata(mem_rdata)
                );
            end
            else begin
                // Read
                mem_rdata <= memory[word_addr];

                // Write with byte strobes
                if (mem_wstrb[0]) memory[word_addr][7:0]   <= mem_wdata[7:0];
                if (mem_wstrb[1]) memory[word_addr][15:8]  <= mem_wdata[15:8];
                if (mem_wstrb[2]) memory[word_addr][23:16] <= mem_wdata[23:16];
                if (mem_wstrb[3]) memory[word_addr][31:24] <= mem_wdata[31:24];
            end

        end
    end
	 
	 task automatic memory_mapped_outputs(output reg [9:0] LEDR, input [9:0]SW, input [31:0] mem_addr, output reg [31:0] mem_rdata);
        if(mem_addr[19:0] == 20'h0)begin
            //LEDS
            LEDR <= mem_wdata[9:0];
        end
        else if(mem_addr[19:0] == 20'h40)begin
            //SW
            mem_rdata <= {22'b0,SW[9:0]}; 
        end
                    
     endtask

endmodule
