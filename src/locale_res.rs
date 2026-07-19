use roxmltree::Document;
use std::{fs, path::Path};

pub struct LocaleRes {
    pub strings: Vec<LocaleStr>,
}

impl LocaleRes {
    fn from_doc(doc: &Document) -> Result<Self, Box<dyn std::error::Error>> {
        let mut strings = Vec::new();
        let node = doc
            .root()
            .first_element_child()
            .ok_or("`ResourceDictionary` element is missing")?;
        for node in node.children() {
            if node.tag_name().name() == "String" {
                let key = node
                    .attribute("Key")
                    .ok_or("`x:Key` attribute is missing")?
                    .to_owned();
                let space = node.attribute("space").and_then(|space| space.parse().ok());
                let text = node
                    .text()
                    .ok_or("`x:String` element text is missing")?
                    .to_owned();
                strings.push(LocaleStr { key, space, text });
            }
        }
        Ok(Self { strings })
    }

    pub fn from_file<P>(path: P) -> Result<Self, Box<dyn std::error::Error>>
    where
        P: AsRef<Path> + std::fmt::Display,
    {
        let content =
            fs::read_to_string(&path).map_err(|err| format!("Could not read '{path}': {err}"))?;
        let doc = Document::parse(&content)?;
        Self::from_doc(&doc)
    }
}

pub struct LocaleStr {
    pub key: String,
    pub space: Option<XmlSpace>,
    pub text: String,
}

#[derive(strum::Display, strum::EnumString)]
#[strum(serialize_all = "lowercase")]
pub enum XmlSpace {
    Default,
    Preserve,
}
