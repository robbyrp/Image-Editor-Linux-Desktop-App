use image::GenericImageView;
use json::{self, JsonValue};
use std::env;
use std::fs::File;
use std::io::Read;
use std::thread;

const EPS: u8 = 5;

#[derive(PartialEq)]
enum ErrorTypes {
    AllGood,
    NoRef(String),                    // <file> was not found
    NoOutput(String),                 // <file> is missing from ref folder
    BadSizes((u32, u32), (u32, u32)), // Mismatched sizes: (x1, y1) vs (x2, y2)
    BadTypes,                         // The images are not the same type
    // BadFormat,                          // The images have a wrong format
    // NotAnImage,                         // The file is not an image
    EPSError(String, u8, u32, u32), // Differnce was more than EPS (<difference>) at x, y
}

fn compare_images(path_ref: String, path_out: String) -> ErrorTypes {
    let img_ref = match image::open(&path_ref) {
        Ok(image) => image,
        Err(_) => return ErrorTypes::NoRef(path_ref),
    };

    let img_out = match image::open(&path_out) {
        Ok(image) => image,
        Err(_) => return ErrorTypes::NoOutput(path_out),
    };

    if img_ref.dimensions() != img_out.dimensions() {
        return ErrorTypes::BadSizes(img_ref.dimensions(), img_out.dimensions());
    }

    if img_ref.color() != img_out.color() {
        return ErrorTypes::BadTypes;
    }

    let ref_arr = img_ref.as_bytes();
    let out_arr = img_out.as_bytes();

    for (a, b) in ref_arr.iter().zip(out_arr) {
        if a.abs_diff(*b) > EPS {
            return ErrorTypes::EPSError(
                path_out,
                a.abs_diff(*b),
                100 / img_ref.width(),
                100 % img_ref.width(),
            );
        }
    }

    ErrorTypes::AllGood
}

fn main() -> Result<(), String> {
    let args: Vec<String> = env::args().collect();
    if args.len() == 2 && args[1] == "--version" {
        println!("Program version: 1.0.0");
        std::process::exit(0);
    }

    if env::args().len() != 3 {
        eprintln!("Usage: {} <json-file-path> <test-name>", env::args().nth(0).unwrap());
        eprintln!("Arguments passed: {:?}", &args[1..]);
        std::process::exit(1);
    }

    let test_name = &args[2];

    let mut file = File::open("tests.json").unwrap();
    let mut data: String = String::new();
    file.read_to_string(&mut data).unwrap();

    let json: JsonValue = json::parse(&data).unwrap();

    let mut threads = Vec::new();

    for i in 0..json[test_name]["files"].len() {
        let refname = String::from(json[test_name]["files"][i]["ref"].as_str().unwrap());
        let outname = String::from(json[test_name]["files"][i]["output"].as_str().unwrap());

        let handle = thread::spawn(move || compare_images(refname, outname));

        threads.push(handle);
    }

    let mut error: ErrorTypes = ErrorTypes::AllGood;

    for handle in threads {
        error = handle.join().unwrap();
        if error != ErrorTypes::AllGood {
            break;
        }
    }

    match error {
        ErrorTypes::BadSizes(a, b) => Err(format!("Mismatched sizes: {:?} vs {:?}", a, b)),

        ErrorTypes::NoOutput(name) => Err(format!("{name} was not found")),

        ErrorTypes::NoRef(name) => Err(format!("{name} ref is missing")),

        ErrorTypes::EPSError(name, diff, x, y) => Err(format!(
            "File {name} has a differnce more than {EPS} ({diff}) at {x}, {y}"
        )),
        ErrorTypes::BadTypes => Err("The images are not the same type".to_string()),
        ErrorTypes::AllGood => Ok(()),
    }
}
