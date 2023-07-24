#pragma once

#ifndef __has_include
static_assert(false, "__has_include not supported");
#else
#  if __has_include(<filesystem>)
#    include <filesystem>
namespace FSNamespace = std::filesystem;
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
namespace FSNamespace = std::experimental::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
namespace FSNamespace = boost::filesystem;
#  endif
#endif

