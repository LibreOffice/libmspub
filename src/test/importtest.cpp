/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libmspub project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstdlib>
#include <string>

#include <cppunit/extensions/HelperMacros.h>

#include <librevenge/librevenge.h>
#include <librevenge-stream/librevenge-stream.h>
#include <libmspub/libmspub.h>

namespace
{

/// Converts a test document to SVG the way pub2xhtml does and returns the first page.
std::string convertToSVG(const char *filename)
{
  librevenge::RVNGFileStream input((std::string(TDOC "/") + filename).c_str());
  CPPUNIT_ASSERT(libmspub::MSPUBDocument::isSupported(&input));
  librevenge::RVNGStringVector pages;
  librevenge::RVNGSVGDrawingGenerator generator(pages, "svg");
  CPPUNIT_ASSERT(libmspub::MSPUBDocument::parse(&input, &generator));
  CPPUNIT_ASSERT(!pages.empty());
  return pages[0].cstr();
}

}

class ImportTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(ImportTest);
  CPPUNIT_TEST(testFontSizeInPoints);
  CPPUNIT_TEST_SUITE_END();

  void testFontSizeInPoints();
};

void ImportTest::testFontSizeInPoints()
{
  // tdf#160489 the SVG generator writes fo:font-size as a bare number, so the
  // 10 pt text in this file used to come out as font-size="0.1389"
  const std::string svg = convertToSVG("fdo59355-1.pub");
  const std::string attribute("font-size=\"");
  const std::string::size_type pos = svg.find(attribute);
  CPPUNIT_ASSERT(pos != std::string::npos);
  CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, std::atof(svg.c_str() + pos + attribute.size()), 0.001);
}

CPPUNIT_TEST_SUITE_REGISTRATION(ImportTest);

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
