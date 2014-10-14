#include <fstream>
#include <vector>
#include <unordered_set>
#include <string>
#include "Util.hpp"

using namespace std;


struct Timer {
  CS207::Clock clock;
  std::string msg;

  Timer(const std::string& _msg)
      : msg(_msg) {
    clock.start();
  }

  ~Timer() {
    double elapsed_ms = clock.seconds() * 1000;
    std::cout << msg << ": " << elapsed_ms << " ms" << std::endl;
  }
};


int main(int argc, char **argv)
{
    vector<string> words_vec;
    unordered_set<string> words_set;
    (void) argc;
    (void) argv;
    
    if (1) {
        Timer timer("loading words vec");
        ifstream wordfile("/usr/share/dict/words");
        string line;
        
        while(getline(wordfile, line)) {
            words_vec.push_back(line);
        }
    }

    if (1) {
        Timer timer("loading words unordered_set");
        ifstream wordfile("/usr/share/dict/words");
        string line;
        
        while(getline(wordfile, line)) {
            words_set.insert(line);
        }
    }


    if (1)    { 
        Timer timer("binary_search(vector)");
        int sum = 0;
        for (unsigned int i = 0; i < words_vec.size(); i++) {
            int idx = int(CS207::random(0,words_vec.size()));
            sum += binary_search(words_vec.begin(), words_vec.end(), words_vec[idx]) ? 1 : 0;
        }
    };

    if (1)    { 
        Timer timer("unordered_set.find()");
        int sum = 0;
        for (unsigned int i = 0; i < words_vec.size(); i++) {
            int idx = int(CS207::random(0,words_vec.size()));
            sum += words_set.find(words_vec[idx]) != words_set.end();
        }
    };

}
