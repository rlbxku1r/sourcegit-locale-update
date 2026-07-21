use std::io;

use crate::{
    CommandLineOptions, UpdateMode,
    locale_res::{LocaleRes, LocaleStr},
};

pub fn print_xml(
    mut w: impl io::Write,
    options: &CommandLineOptions,
    base_res: &LocaleRes,
    localized_res: &LocaleRes,
) -> Result<(), io::Error> {
    writeln!(
        w,
        "<ResourceDictionary xmlns=\"https://github.com/avaloniaui\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">"
    )?;
    writeln!(w, "  <ResourceDictionary.MergedDictionaries>")?;
    writeln!(
        w,
        "    <ResourceInclude Source=\"avares://SourceGit/Resources/Locales/{}\"/>",
        extract_file_name(&options.base_xml_file)
    )?;
    writeln!(w, "  </ResourceDictionary.MergedDictionaries>")?;
    writeln!(w)?;
    for LocaleStr {
        key,
        space,
        text: base_text,
    } in &base_res.strings
    {
        let text = if let Some(LocaleStr {
            text: localized_text,
            ..
        }) = localized_res.strings.iter().find(|&s| s.key == *key)
        {
            localized_text
        } else {
            base_text
        };
        write!(w, "  <x:String x:Key=\"{}\"", escape_attribute_value(key))?;
        if let Some(space) = space {
            write!(w, " xml:space=\"{space}\"")?;
        }
        write!(w, ">{}", escape_text(text))?;
        if options.update_mode == UpdateMode::Review {
            write!(w, "<!--{}-->", escape_comment(base_text))?;
        }
        write!(w, "</x:String>")?;
        writeln!(w)?;
    }
    writeln!(w, "</ResourceDictionary>")?;
    Ok(())
}

fn extract_file_name(path: &str) -> &str {
    path.rsplit_once('/').map_or(path, |(_, name)| name)
}

fn escape_text(text: &str) -> String {
    text.replace("&", "&amp;") // "&" should be first
        .replace("<", "&lt;")
        .replace(">", "&gt;")
}

fn escape_attribute_value(attribute_value: &str) -> String {
    attribute_value.replace("\"", "&quot;")
}

fn escape_comment(comment: &str) -> String {
    comment.replace("--", "&#45;&#45;")
}
