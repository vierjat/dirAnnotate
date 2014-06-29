#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;


struct dir_t{
  vector<dir_t> subdirs;
  string name;
  int level;
  int totsubd;
  string readme;
  dir_t(const char* dirName, int l):name(dirName),level(l),readme(""),totsubd(0){};
};


void printTable(const dir_t d){
  
  cout << "|";
  if(d.totsubd>1){
    cout << "/" << d.totsubd << ".";
  }
  
  if(d.subdirs.size()==0){
    cout  << "{background:yellow}.";
  }
  cout << "*" << d.name << "*";
  if(d.readme.size()>1) cout << " " << d.readme;
  if(d.subdirs.size()==0){
    cout  << "|" << endl;
  }
  
  for(unsigned int i=0; i<d.subdirs.size(); ++i){
    printTable(d.subdirs[i]);
  }
  
}

std::ostream &operator<<(std::ostream &os, const dir_t &d) {

  os << setw(d.level * 2) << " " << d.name;
  if(d.subdirs.size()==0){
    os << "*";
  }
  os << endl;
  
  for(unsigned int i=0; i<d.subdirs.size(); ++i){
    os << d.subdirs[i];
  }
  return os;
}

inline bool operator< (const dir_t &lhs, const dir_t &rhs){
  return lhs.name < rhs.name;
}

bool isDir(string dir)
{
    struct stat fileInfo;
    stat(dir.c_str(), &fileInfo);
    if (S_ISDIR(fileInfo.st_mode)) {
        return true;
    } else {
        return false;
    }
}

void listFiles(string baseDir, int depth, dir_t &dir)
{
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(baseDir.c_str())) == NULL) {
        cout << "[ERROR: " << errno << " ] Couldn't open " << baseDir << "." << endl;
        return;
    } 
    else {
      bool subDirFound = false;
        while ((dirp = readdir(dp)) != NULL) {
            if (dirp->d_name[0] != '.') {
                if (isDir(baseDir + dirp->d_name) == true) {
                    subDirFound = true;
                    
                    dir_t subdir(dirp->d_name,depth+1);
                    listFiles(baseDir + dirp->d_name + "/", depth+1, subdir);
                    sort(subdir.subdirs.begin(), subdir.subdirs.end());
                    dir.totsubd += subdir.totsubd;
                    dir.subdirs.push_back(subdir);
                } else {
                  if(strcmp(dirp->d_name, "README") == 0){
                    cout << "[FILE]\t" << depth << " " << dirp->d_name << endl;
                    std::ifstream t((baseDir + dirp->d_name).c_str());
                    std::stringstream buffer;
                    buffer << t.rdbuf();
                    t.close();
                    
                    dir.readme = buffer.str();
                    std::replace( dir.readme.begin(), dir.readme.end(), '\n', ' ');
                  }
                }
            }
        }
        if(!subDirFound) dir.totsubd = 1; 
        closedir(dp);
    }
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
      cout << "[WARNING] At least one argument ( path ) expected .. exiting." << endl;
      return 1;
    } else {
      dir_t baseDir(argv[1],0);
      listFiles(argv[1], 0, baseDir);
      sort(baseDir.subdirs.begin(), baseDir.subdirs.end());
      
      cout << endl;
//       cout << baseDir;
      
      printTable(baseDir);
//       for(unsigned int i=0; i<baseDir.subdirs.size(); ++i){
//         cout << baseDir.subdirs[i] << endl;
//         for(unsigned int j=0; j<baseDir.subdirs[i].subdirs.size(); ++j){
//           cout << baseDir.subdirs[i].subdirs[j] << endl;
//         }
//       }
      
    }
    
    return 0;
}