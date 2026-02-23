#pragma once

#include "tinyxml2/tinyxml2.h"
#include <cstdio>

class LocaleXMLPrinter : public tinyxml2::XMLPrinter {
public:
  LocaleXMLPrinter(std::FILE *file) : tinyxml2::XMLPrinter{file} {}
  virtual ~LocaleXMLPrinter() override {}

protected:
  virtual void PrintSpace(int depth) override {
    for (int i{0}; i < depth; ++i) {
      Write("  ");
    }
  }
};
