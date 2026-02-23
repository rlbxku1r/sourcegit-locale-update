#include "locale_xml_printer.hpp"
#include "tinyxml2/tinyxml2.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string>
#include <unordered_map>

static const char *DEFAULT_APP_NAME{"sourcegit-locale-update"};

enum class UpdateMode { Merge, Review };

struct CommandLineOptions {
  UpdateMode update_mode;
  std::string base_xml_file;
  std::string localized_xml_file;
};

CommandLineOptions parse_command_line_options(int argc, char *argv[]) {
  if (argc != 3 && argc != 4) {
    std::cerr << "Usage: " << (argc == 0 ? DEFAULT_APP_NAME : argv[0])
              << " UPDATE_MODE LOCALIZED_XML_FILE [BASE_XML_FILE]" << std::endl;
    std::exit(1);
  }

  UpdateMode update_mode;
  if (strcmp(argv[1], "merge") == 0) {
    update_mode = UpdateMode::Merge;
  } else if (strcmp(argv[1], "review") == 0) {
    update_mode = UpdateMode::Review;
  } else {
    std::cerr << "Invalid UPDATE_MODE '" << argv[1] << "'." << std::endl;
    std::cerr << "Accepted values are: 'merge', 'review'." << std::endl;
    std::exit(1);
  }

  std::string base_xml_file{"en_US.axaml"};
  if (argc == 4) {
    base_xml_file = argv[3];
  }

  return {.update_mode = update_mode,
          .base_xml_file = base_xml_file,
          .localized_xml_file = argv[2]};
}

void add_mergeddictionaries_metadata(
    tinyxml2::XMLDocument &doc,
    tinyxml2::XMLElement *resourcedictionary_element,
    const std::string &base_xml_file) {
  // <ResourceInclude> - Source
  std::string resourceinclude_source{"avares://SourceGit/Resources/Locales/"};
  auto pos{base_xml_file.find_last_of("/\\")};
  resourceinclude_source +=
      (pos == std::string::npos ? base_xml_file
                                : base_xml_file.substr(pos + 1));
  // <ResourceInclude>
  auto resourceinclude_element{doc.NewElement("ResourceInclude")};
  resourceinclude_element->SetAttribute("Source",
                                        resourceinclude_source.c_str());
  // <ResourceDictionary.MergedDictionaries>
  auto mergeddictionaries_element{
      doc.NewElement("ResourceDictionary.MergedDictionaries")};
  mergeddictionaries_element->InsertFirstChild(resourceinclude_element);
  // <ResourceDictionary>
  resourcedictionary_element->InsertFirstChild(mergeddictionaries_element);
}

std::unordered_map<std::string, std::string>
parse_localized_strings(tinyxml2::XMLDocument &from) {
  auto element{tinyxml2::XMLHandle{from}
                   .FirstChildElement("ResourceDictionary")
                   .FirstChildElement("x:String")
                   .ToElement()};
  if (!element) {
    std::cerr << "Could not parse the localized XML file." << std::endl;
    std::exit(1);
  }
  std::unordered_map<std::string, std::string> localized_strings;
  while (element) {
    const char *key;
    if (element->QueryStringAttribute("x:Key", &key) == tinyxml2::XML_SUCCESS) {
      localized_strings[key] = element->GetText();
    }
    element = element->NextSiblingElement();
  }
  return localized_strings;
}

int main(int argc, char *argv[]) {
  auto command_line_options{parse_command_line_options(argc, argv)};

  tinyxml2::XMLDocument doc_base, doc_loc;
  doc_base.LoadFile(command_line_options.base_xml_file.c_str());
  if (doc_base.Error()) {
    std::cerr << "Could not load '" << command_line_options.base_xml_file
              << "'." << std::endl;
    std::exit(1);
  }
  doc_loc.LoadFile(command_line_options.localized_xml_file.c_str());
  if (doc_loc.Error()) {
    std::cerr << "Could not load '" << command_line_options.localized_xml_file
              << "'." << std::endl;
    std::exit(1);
  }

  auto resourcedictionary_handle{
      tinyxml2::XMLHandle{doc_base}.FirstChildElement("ResourceDictionary")},
      xstring_handle{resourcedictionary_handle.FirstChildElement("x:String")};
  auto resourcedictionary_element{resourcedictionary_handle.ToElement()},
      xstring_element{xstring_handle.ToElement()};
  if (!resourcedictionary_element || !xstring_element) {
    std::cerr << "Could not parse the base XML file." << std::endl;
    std::exit(1);
  }
  auto localized_strings{parse_localized_strings(doc_loc)};
  while (xstring_element) {
    const char *key;
    if (xstring_element->QueryStringAttribute("x:Key", &key) ==
        tinyxml2::XML_SUCCESS) {
      if (command_line_options.update_mode == UpdateMode::Review) {
        xstring_element->InsertNewComment(xstring_element->GetText());
      }
      auto iter{localized_strings.find(key)};
      if (iter != localized_strings.end()) {
        xstring_element->SetText(iter->second.c_str());
      }
    }
    xstring_element = xstring_element->NextSiblingElement();
  }

  add_mergeddictionaries_metadata(doc_base, resourcedictionary_element,
                                  command_line_options.base_xml_file);

  LocaleXMLPrinter stream{stdout};
  doc_base.Print(&stream);
  if (doc_base.Error()) {
    std::cerr << "Could not print the final XML data." << std::endl;
    std::exit(1);
  }

  return 0;
}
