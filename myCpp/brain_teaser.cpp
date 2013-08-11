#include <iostream>
#include <string>
#include <vector>

using namespace std;

string getMissingLetters(const string& sentence)
{
    vector<bool> vec(26, false);

    for(string::const_iterator it=sentence.begin(); it != sentence.end(); ++it)
    {
        char c = tolower(*it);
        int idx = c-'a';
        if(idx >= 0 && idx < 26) vec[idx] = true;
    }

    string res;
    for(vector<bool>::const_iterator it=vec.begin(); it != vec.end(); ++it)
    {
        if(!*it)  res.push_back((char)('a'+(it-vec.begin())));
    }
    return res;
}

void check_char(const char * cptr)
{
    string sentence(cptr);
    string res;

    res = getMissingLetters(sentence);
    cout << '\"' << sentence << '\"' << std::endl;
    cout << "==>" << '\"' << res << '\"' << std::endl;
    cout << std:: endl;
}

//*************************

typedef pair<char,char> Particle;

int update_state(vector<string>& res, const string & init, const vector<int> & loc)
{
    int count=0;
    string ss(init.size(), '.');
    for(int ii=0; ii < init.size(); ++ii)
    {
        int jj=loc[ii];
        char c = init[ii];

        if(c != '.' && jj >= 0 && jj < init.size())
        {
            ss[jj]='X';
            ++count;
        }
    }
    res.push_back(ss);
    return count;
}

vector<string> animate(int speed, const string & init)
{
    string state(init);
    vector<int> loc;
    vector<string> res;

    // initialize the location
    int nparticles=0;
    size_t nloc = init.size();

    // output the initial location
    for(int ii=0; ii < nloc; ++ii) loc.push_back(ii);

    nparticles=update_state(res, init, loc);

    // simulate
    while(nparticles)
    {
        for(int ii=0; ii < nloc; ++ii)
        {
            char c = init[ii];
            if(c=='L') loc[ii] -= speed;
            if(c=='R') loc[ii] += speed;
        }
        nparticles=update_state(res, init, loc);
    }
    return res;
}

void print_states(const vector<string> & states)
{
    for(vector<string>::const_iterator it = states.begin(); it != states.end(); ++it)
        cout << *it << std::endl;
    cout << std::endl;
}

void brain_teaser(void)
{
    //****
    {
        check_char("A quick brown fox jumps over the lazy dog");
        check_char("");
        check_char("A slow yellow fox crawls under the proactive dog");
        check_char("Lions, and tigers, and bears, oh my!");
    }

    //****
    {
        
        print_states(animate(2,  "..R...."));
        print_states(animate(3,  "RR..LRL"));
        print_states(animate(2,  "LRLR.LRLR"));
        print_states(animate(10, "RLRLRLRLRL"));
        print_states(animate(1,  "..."));
        print_states(animate(1,  "LRRL.LR.LRR.R.LRRL."));

    }
}

