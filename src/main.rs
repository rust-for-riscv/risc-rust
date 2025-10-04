use std::env;
use std::process::Stdio;
use std::process::Command;
fn main() {
    let mut args: Vec<String> = env::args().collect();
    args[0] = "./build/compiler".to_string();

    if args.len() == 2 {
        args.push(args[1].replace(".rs", ".hex"));
    }

    let status = Command::new(&args[0])
        .args(&args[1..]) // skip the first element when passing args
        .stdin(Stdio::inherit())
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .status()
        .expect("failed to execute command");

    if !status.success() {
        let errcode = status.code().unwrap();
        eprintln!("Compiler failed with code {errcode}.");
    }
}
