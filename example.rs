#![no_std]
#![no_main]

use core::panic::PanicInfo;

/// Entry point: called at reset.
#[no_mangle]
pub extern "C" fn _start() -> ! {
    let mut value: u32;
    let addr_led = 0xFF20_0000 as *mut u32;
    let addr_sw = 0xFF20_0040 as *const u32;

    // Stay here forever
    loop {
        unsafe {
            value = core::ptr::read_volatile(addr_sw);
            core::ptr::write_volatile(addr_led, value);
        }
    }
}

/// Required panic handler
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
