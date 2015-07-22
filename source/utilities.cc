#include "../include/utilities.h"
#include <vector>

#ifdef DEAL_II_SAK_WITH_BOOST
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include "boost/filesystem.hpp"
#else
#include <stdlib.h>
#include <sys/stat.h>
#endif

namespace
{
  struct handle
  {
    char *p;
    handle(char *ptr) : p(ptr) { }
    ~handle()
    {
      delete p;
    }
  };
}

std::string demangle(const char *name)
{
  int status = -4; // some arbitrary value to eliminate the compiler warning
  handle result( abi::__cxa_demangle(name, NULL, NULL, &status) );
  return (status==0) ? result.p : name ;
}

bool file_exists(const std::string &file)
{
#ifdef DEAL_II_SAK_WITH_BOOST
  return boost::filesystem::exists( file );
#else
  struct stat st;
  lstat(file.c_str(), &st);
  if (S_ISREG(st.st_mode))
    return true;
  else
    return false;
#endif
}

bool dir_exists(const std::string &dir)
{
#ifdef DEAL_II_SAK_WITH_BOOST
  return boost::filesystem::exists( dir );
#else
  struct stat st;
  lstat(dir.c_str(), &st);
  if (S_ISDIR(st.st_mode))
    return true;
  else
    return false;
#endif
}

int get_next_available_index_directory_name(const std::string &base, int n_digits)
{
  unsigned int index = 0;
  while ( dir_exists( base + Utilities::int_to_string (index, n_digits) ) ) index++;
  return index;
}

std::string get_next_available_directory_name(const std::string &base, int n_digits)
{
  unsigned int index = get_next_available_index_directory_name(base, n_digits);
  return base + Utilities::int_to_string (index, n_digits);
}

bool create_directory(const std::string &name)
{
  std::string name_cleaned = name;
  name_cleaned.erase(std::remove(name_cleaned.begin(),name_cleaned.end(),' '),name_cleaned.end());
#ifdef DEAL_II_SAK_WITH_BOOST
  boost::filesystem::create_directories("./" + name_cleaned + "/");
#else
  std::string cmd = "";
  cmd = "mkdir -p " + name_cleaned + "/ ;";
  std::system(cmd.c_str());
#endif
  return dir_exists(name_cleaned + "/");
}

bool copy_files(const std::string &files, const std::string &destination)
{
  create_directory("./"+destination);
  bool result = true;
  std::vector<std::string> strs;
  std::string new_file;
  strs = dealii::Utilities::split_string_list(files, ' ');
  if (files!="")
    {
      for (size_t i = 0; i < strs.size(); i++)
        {
          if (file_exists(strs[i]))
            {
              new_file = destination+"/"+strs[i];
#ifdef DEAL_II_SAK_WITH_BOOST
              boost::filesystem::copy_file(strs[i], new_file,
                                           boost::filesystem::copy_option::overwrite_if_exists);
#else
              std::string cmd = "cp -f ./" + strs[i] + " ./" +new_file + " ;";
              std::system( cmd.c_str() );
#endif
            }
          result &= file_exists(new_file);
        }
      return result;
    }
}

bool copy_file(const std::string &file, const std::string &new_file)
{
#ifdef DEAL_II_SAK_WITH_BOOST
  boost::filesystem::copy_file(file, new_file,
                               boost::filesystem::copy_option::overwrite_if_exists);
#else
  std::string cmd = "cp -f ./" + file + " ./" + new_file + " ;" ;
  std::system( cmd.c_str() );
#endif
  return file_exists(new_file);
}

bool rename_file(const std::string &file, const std::string &new_file)
{
#ifdef DEAL_II_SAK_WITH_BOOST
  boost::filesystem::rename(file, new_file);
#else
  std::string cmd = "mv ./" + file + " ./" + new_file +" ;";
  std::system( cmd.c_str() );
#endif
  return file_exists(new_file);
}
