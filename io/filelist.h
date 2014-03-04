#ifndef FILELIST_H
#define FILELIST_H

#include "../common/types.h"

namespace sse {

class FileList
{
public:
    typedef boost::function<void (int, const std::string&)> callback_fn;

    // Constructor, pass in the desired root directory.
    // The directory passed in must be a valid, existing directory
    FileList(const std::string &rootDir = ".");

    // List all files found in and below rootdir that have one of the file-endings specified
    // in 'namefilter'. Each entry of the vector contains a string such as "*.png" or "*.jpg"
    void lookupDir(const std::vector<std::string> &nameFilters, callback_fn callback = callback_fn());

    // Subsample given filelist randomly
    void randomSample(uint numOfSamples, uint seed);

    // Vector of all filenames
    const std::vector<std::string>& filenames() const;
    uint size() const;

    //Access relative filename of file i
    //index: [0, size()-1]
    const std::string& getRelativeFilename(uint index) const;
    //Access 'absolute' filename of file i
    //index: [0, size()-1]
    std::string getFilename(uint index) const;

    //get and set rootDir
    const std::string& getRootDir() const;
    void setRootDir(const std::string &rootDir);

    //Load a FileList
    //Note that you Must setRootDir
    void load(const std::string &filename);
    //Store a FileList
    //Note that the root directory is not stored, only
    //the list of filenames relative to the root directory.
    void store(const std::string &filename)const;

private:
    std::vector<std::string> _files;
    std::string _rootDir;
};

} //namespace sse

#endif // FILELIST_H
