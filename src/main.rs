use clap::Parser as _;
use std::io;

use locale_res::LocaleRes;

mod locale_res;
mod printer;

#[derive(clap::Parser)]
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
    let base_xml_name = options
        .base_xml_file
        .rsplit_once('/')
        .map_or(options.base_xml_file.as_str(), |(_, name)| name);
    printer::print_xml(
        io::stdout(),
        base_xml_name,
        &base_res,
        &localized_res,
        options.update_mode,
    )?;
    Ok(())
}
