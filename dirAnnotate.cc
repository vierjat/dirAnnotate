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

bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

struct dir_t{
  vector<dir_t> subdirs;
  string name;
  int level;
  int totsubd;
  int nFits;
  bool hasBlanks;
  string readme;
  dir_t(const char* dirName, int l):name(dirName),level(l),totsubd(0),nFits(0),hasBlanks(false),readme(""){};
};


void printTable(const dir_t d){
  
  cout << "|";
  if(d.totsubd>1){
    cout << "/" << d.totsubd << ".";
  }
  
  if(d.subdirs.size()==0){
    if(d.hasBlanks)
      cout  << "{background:#afa}.";
    else
      cout  << "{background:#ff8}.";
  }
  cout << "*" << d.name << "*";
  if(d.nFits>0){
    cout  << "\n*@#fits:" << d.nFits <<"@*";
  }
  if(d.readme.size()>1) cout << "\n" << d.readme;
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

                    if(strcmp(dirp->d_name, "stats") == 0)
                      continue;
                    if(strcmp(dirp->d_name, "blank") == 0){
                      dir.hasBlanks = true;
                      continue;
                    }

                    subDirFound = true;
                    
                    dir_t subdir(dirp->d_name,depth+1);
                    listFiles(baseDir + dirp->d_name + "/", depth+1, subdir);
                    sort(subdir.subdirs.begin(), subdir.subdirs.end());
                    dir.totsubd += subdir.totsubd;
                    dir.subdirs.push_back(subdir);
                }
                else {
                  
                  int len = strlen(dirp->d_name);
                  if(len > 4){
                    const char *last_three = dirp->d_name + (len-4);
                    if(strcmp(last_three, "fits") == 0){
                      dir.nFits++;
                    }
                  }
                  if(len > 7){
                    const char *last_seven = dirp->d_name + (len-7);
                    if(strcmp(last_seven, "fits.fz") == 0){
                      dir.nFits++;
                    }
                  }
                  
                  if(strcmp(dirp->d_name, "README") == 0){
//                     cout << "[FILE]\t" << depth << " " << dirp->d_name << endl;
                    
                    //Read README file and replace newline chars with spaces
                    std::ifstream t((baseDir + dirp->d_name).c_str());
                    std::stringstream buffer;
                    buffer << t.rdbuf();
                    t.close();
                    dir.readme = buffer.str();
                    std::replace( dir.readme.begin(), dir.readme.end(), '\n', ' ');
                    std::string::iterator new_end = std::unique(dir.readme.begin(), dir.readme.end(), BothAreSpaces);
                    dir.readme.erase(new_end, dir.readme.end());
                    trim(dir.readme);
                    
                    const unsigned int kWL = 25;
                    for(unsigned int m=kWL; m<dir.readme.size(); ++m){
                      if(dir.readme[m] == ' '){
                        dir.readme[m] = '\n';
                        m += kWL;
                        if(m >= dir.readme.size()) break;
                      }
                    }
                    
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
      
      vector<std::string> argsv(argv + 1, argv + argc);
      sort(argsv.begin(), argsv.end());
      
      cout << endl;
      for(unsigned int i=0; i<argsv.size(); ++i){
        dir_t baseDir(argsv[i].c_str(),0);
        listFiles(argsv[i], 0, baseDir);
        sort(baseDir.subdirs.begin(), baseDir.subdirs.end());
        printTable(baseDir);
      }
      cout << endl;
      
    }
    
    return 0;
}
