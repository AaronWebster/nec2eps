#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "absl/base/internal/raw_logging.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_split.h"

ABSL_FLAG(double, scale, 1.0, "Output scale factor.");
ABSL_FLAG(bool, xy, false, "");
ABSL_FLAG(bool, yz, false, "");
ABSL_FLAG(bool, xz, false, "");

using Point = std::pair<double, double>;
using Line = std::pair<Point, Point>;

// Number of elements in a GW line
constexpr int kGwElementSize = 10;
// 0  1   2  3   4   5   6   7   8   9
// GW ITG NS XW1 YW1 ZW1 XW2 YW2 ZW2 RAD

// Returns a pair representing the X Y indices specified from flags.

absl::StatusOr<std::pair<int, int>> GetXYIndicesFromFlags() {
  const bool xy = absl::GetFlag(FLAGS_xy);
  const bool yz = absl::GetFlag(FLAGS_yz);
  const bool xz = absl::GetFlag(FLAGS_xz);

  if (xy && !yz && !xz) {
    ABSL_RAW_LOG(INFO, "Using x-y projection.");
    return std::make_pair(0, 1);
  }

  if (!xy && yz && !xz) {
    ABSL_RAW_LOG(INFO, "Using y-z projection.");
    return std::make_pair(1, 2);
  }

  if (!xy && !yz && xz) {
    ABSL_RAW_LOG(INFO, "Using x-z projection.");
    return std::make_pair(0, 2);
  }

  return absl::InvalidArgumentError("Invalid projection specified from flags.");
}

int main(int argc, char** argv) {
  const auto positional_args = absl::ParseCommandLine(argc, argv);
  const double scale = absl::GetFlag(FLAGS_scale);
  const auto [x_index, y_index] = GetXYIndicesFromFlags().value();

  std::streambuf* nec_buf = nullptr;
  std::ifstream nec_stream;
  if (positional_args.size() > 1) {
    nec_stream.open(positional_args.at(1));
    nec_buf = nec_stream.rdbuf();
    ABSL_RAW_LOG(INFO, "Reading NEC from %s.", positional_args[1]);
  } else {
    nec_buf = std::cin.rdbuf();
    ABSL_RAW_LOG(INFO, "Reading NEC from standard input.");
  }
  std::istream nec(nec_buf);
  ABSL_RAW_CHECK(nec.good(), "Could not create input buffer.");

  std::streambuf* eps_buf = nullptr;
  std::ofstream eps_stream;
  if (positional_args.size() > 2) {
    eps_stream.open(positional_args.at(2));
    eps_buf = eps_stream.rdbuf();
    ABSL_RAW_LOG(INFO, "Writing EPS to %s.", positional_args[2]);
  } else {
    eps_buf = std::cout.rdbuf();
    ABSL_RAW_LOG(INFO, "Writing EPS to standard output.");
  }
  std::ostream eps(eps_buf);
  ABSL_RAW_CHECK(eps.good(), "Could not create output buffer.");

  std::vector<Line> lines;
  auto p_max = std::make_pair(std::numeric_limits<double>::lowest(),
                              std::numeric_limits<double>::lowest());
  auto p_min = std::make_pair(std::numeric_limits<double>::max(),
                              std::numeric_limits<double>::max());

  std::string nec_line;
  while (std::getline(nec, nec_line)) {
    if (!absl::StartsWith(nec_line, "GW")) continue;

    std::vector<absl::string_view> elements =
        absl::StrSplit(nec_line, " ", absl::SkipWhitespace());
    if (elements.size() != kGwElementSize) continue;

    lines.emplace_back();
    auto& line = lines.back();
    ABSL_RAW_CHECK(
        (absl::SimpleAtod(elements[3 + x_index], &line.first.first) &&
         absl::SimpleAtod(elements[3 + y_index], &line.first.second) &&
         absl::SimpleAtod(elements[6 + x_index], &line.second.first) &&
         absl::SimpleAtod(elements[6 + y_index], &line.second.second)),
        "Error parsing NEC input.");

    line.first.first *= scale;
    line.first.second *= scale;
    line.second.first *= scale;
    line.second.second *= scale;

    p_max.first =
        std::max(std::max(line.first.first, line.second.first), p_max.first);
    p_max.second =
        std::max(std::max(line.first.second, line.second.second), p_max.second);
    p_min.first =
        std::min(std::min(line.first.first, line.second.first), p_max.first);
    p_min.second =
        std::min(std::min(line.first.second, line.second.second), p_max.second);
  }

  ABSL_RAW_CHECK(!lines.empty(), "No GW lines found in input.");

  eps << "%!PS-Adobe-3.0 EPSF-3.0" << std::endl;
  eps << "%%Pages: 1" << std::endl;
  eps << "%%BoundingBox: " << p_min.first << " " << p_min.second << " "
      << p_max.first << " " << p_max.second << std::endl;

  for (const auto& line : lines) {
    eps << "newpath"
        << " " << line.first.first << " " << line.first.second << " moveto "
        << line.second.first << " " << line.second.second << " lineto"
        << " stroke"
        << " closepath" << std::endl;
  }
  eps.flush();
  return EXIT_SUCCESS;
}
