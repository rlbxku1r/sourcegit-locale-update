use clap::Parser as _;
use std::io;

use locale_res::LocaleRes;

mod locale_res;
mod printer;

#[derive(clap::Parser)]
#[command(about)]
struct CommandLineOptions {
    update_mode: UpdateMode,
    localized_xml_file: String,
    #[arg(default_value_t = String::from("en_US.axaml"))]
    base_xml_file: String,
}

#[derive(Clone, PartialEq, clap::ValueEnum)]
enum UpdateMode {
    Merge,
    Review,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let options = CommandLineOptions::parse();
    let base_res = LocaleRes::from_file(&options.base_xml_file)?;
    let localized_res = LocaleRes::from_file(&options.localized_xml_file)?;
    printer::print_xml(io::stdout(), &options, &base_res, &localized_res)?;
    Ok(())
}
