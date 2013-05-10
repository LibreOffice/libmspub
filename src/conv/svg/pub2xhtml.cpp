/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* libmspub
 * Version: MPL 1.1 / GPLv2+ / LGPLv2+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2011 Fridrich Strba <fridrich.strba@bluewin.ch>
 * Copyright (C) 2011 Eilidh McAdam <tibbylickle@gmail.com>
 *
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <libwpd-stream/libwpd-stream.h>
#include <libwpd/libwpd.h>
#include <libmspub/libmspub.h>

namespace
{

int printUsage()
{
  printf("Usage: pub2xhtml [OPTION] <Microsoft Publisher Document> [<Output filename>]\n");
  printf("\n");
  printf("Options:\n");
  printf("--help                Shows this help message\n");
  return -1;
}

} // anonymous namespace

int main(int argc, char *argv[])
{
  if (argc < 2)
    return printUsage();

  char *in_file = 0, *out_file = 0;

  for (int i = 1; i < argc; i++)
  {
    if (!in_file)
    {
      if (strncmp(argv[i], "--", 2))
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

  WPXFileStream input(in_file);
  std::ofstream o;
  if (out_file)
    o.open(out_file);
  std::ostream &output = out_file ? o : std::cout;

  if (!libmspub::MSPUBDocument::isSupported(&input))
  {
    std::cerr << "ERROR: Unsupported file format!" << std::endl;
    return 1;
  }

  libmspub::MSPUBStringVector outputStrings;
  if (!libmspub::MSPUBDocument::generateSVG(&input, outputStrings))
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
