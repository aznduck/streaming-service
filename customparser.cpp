// Complete me
#include "customparser.h"
#include <iostream>
#include <sstream>

using namespace std;

const char* error_msg_1 = "Cannot read integer n";
const char* error_msg_2 = "Error in content parsing";
const char* error_msg_3 = "Error in user parsing";


void CustomFormatParser::parse(std::istream& is, std::vector<Content*>& content, std::vector<User*>& users)
{
    // Erase any old contents
    content.clear();
    users.clear();
    if (is.fail()) {
      throw ParserError(error_msg_2); 
    }
   
    int numContents = 0;
    is >> numContents;
    if(is.fail()){throw ParserError(error_msg_1);}
    //getting content
    for(int i = 0; i < numContents; i++)
    {
      int id;
      int type;
      is >> id;
      is >> type;
      string name;
      int nr = 0;
      int ts = 0;
      int rating = 0;
      is >> ws;
      getline(is, name);
      is >> ws >> nr;
      is >> ws >> ts;
      is >> ws >> rating;
      if(is.fail()){throw ParserError(error_msg_2);}
      Content* currContent;
      if(type == 0)
      {
       currContent = new Movie(id, name, nr, ts, rating);
      }
      else if (type == 1)
      {
        int numEpisodes = 0;
        is >> ws >> numEpisodes;
        currContent = new Series(id, name, nr, ts, rating, numEpisodes);
      }
      string line;
      getline(is, line);
      //is >> ws;
      getline(is, line);
      stringstream s(line);
      string username;
      while(s >> username)
      {
        currContent -> addViewer(username);
      }
      content.push_back(currContent);
    }
    //getting users
    string uname;
    while(is >> uname)
    {
      int rl;
      is >> rl;
      if(is.fail()){throw ParserError(error_msg_3);}
      User* currUser = new User(uname, rl);
      string watched;
      getline(is, watched);
      getline(is, watched);
      stringstream s(watched);
      int content;
      while(s >> content)
      {
        currUser -> addToHistory(content);
      }
      users.push_back(currUser);
    }

}
