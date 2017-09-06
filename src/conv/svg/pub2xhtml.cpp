/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <librevenge-stream/librevenge-stream.h>
#include <librevenge/librevenge.h>
#include <libmspub/libmspub.h>

#ifndef VERSION
#define VERSION "UNKNOWN VERSION"
#endif

namespace
{

int printUsage()
{
  printf("`pub2xhtml' converts Microsoft Publisher documents to SVG.\n");
  printf("\n");
  printf("Usage: pub2xhtml [OPTION] INPUT [OUTPUT]\n");
  printf("\n");
  printf("Options:\n");
  printf("\t--help                show this help message\n");
  printf("\t--version             show version information\n");
  printf("\n");
  printf("Report bugs to <https://bugs.documentfoundation.org/>.\n");
  return -1;
}

int printVersion()
{
  printf("pub2raw " VERSION "\n");
  return 0;
}

} // anonymous namespace

int main(int argc, char *argv[])
{
  if (argc < 2)
    return printUsage();

  char *in_file = nullptr, *out_file = nullptr;

  for (int i = 1; i < argc; i++)
  {
    if (!in_file)
    {
      if (!strcmp(argv[i], "--version"))
        return printVersion();
      else if (strncmp(argv[i], "--", 2))
        in_file = argv[i];
    }
    else if (!out_file)
    {
      if (strncmp(argv[i], "--", 2))
        out_file = argv[i];
    }
    else
      return printUsage();
  }

  if (!in_file)
    return printUsage();

  librevenge::RVNGFileStream input(in_file);
  std::ofstream o;
  if (out_file)
    o.open(out_file);
  std::ostream &output = out_file ? o : std::cout;

  if (!libmspub::MSPUBDocument::isSupported(&input))
  {
    std::cerr << "ERROR: Unsupported file format!" << std::endl;
    return 1;
  }

  librevenge::RVNGStringVector outputStrings;
  librevenge::RVNGSVGDrawingGenerator generator(outputStrings, "svg");
  if (!libmspub::MSPUBDocument::parse(&input, &generator))
  {
    std::cerr << "ERROR: SVG Generation failed!" << std::endl;
    return 1;
  }
  if (outputStrings.empty())
  {
    std::cerr << "ERROR: No SVG document generated!" << std::endl;
    return 1;
  }

  output << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
  output << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">" << std::endl;
  output << "<html xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\">" << std::endl;
  output << "<body>" << std::endl;
  output << "<?import namespace=\"svg\" urn=\"http://www.w3.org/2000/svg\"?>" << std::endl;

  for (unsigned k = 0; k<outputStrings.size(); ++k)
  {
    if (k>0)
      output << "<hr/>\n";

    output << "<!-- \n";
    output << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    output << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\"";
    output << " \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    output << " -->\n";

    output << outputStrings[k].cstr() << std::endl;
  }

  output << "</body>" << std::endl;
  output << "</html>" << std::endl;

  return 0;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
