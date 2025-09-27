#![no_std]
#![no_main]

use core::panic::PanicInfo;

/// Entry point: called at reset.
#[no_mangle]
pub extern "C" fn _start() -> ! {
    let value: u32 = 0b1010_1010_10; // 0x2AA
    let addr = 0xFF20_0000 as *mut u32;

    unsafe {
        core::ptr::write_volatile(addr, value);
    }

    // Stay here forever
    loop {}
}

/// Required panic handler
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
