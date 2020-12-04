#include <string>
#include <boost/filesystem.hpp>

bool file_check(std::string file_path) {
    const boost::filesystem::path path(file_path);

    boost::system::error_code error;
    const bool result = boost::filesystem::exists(path, error);
    
    return result && !error;
}