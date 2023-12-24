#include "strmsrv.h"
#include <iostream>
#include <fstream>
using namespace std;


StreamService::StreamService()
{
    cUser_ = NULL;
}


StreamService::~StreamService()
{
  for(User* user : users_)
  {
    delete user;
  }
  for (Content* content: content_)
  {
    delete content;
  }
}


void StreamService::readAndParseData(std::istream& is, Parser& p)
{
    p.parse(is, content_, users_);
    cout << "Read " << content_.size() << " content items." << endl;
    cout << "Read " << users_.size() << " users." << endl;
}


void StreamService::userLogin(const std::string& uname)
{
  if(getUserIndexByName(uname) == -1)
  {
    throw std::invalid_argument("Invalid Username");
  }
  if(cUser_ != NULL) 
  {
    throw std::runtime_error("Please logout of the current user first");
  }
    cUser_ = users_[getUserIndexByName(uname)]; //gets user based on name
}


void StreamService::userLogout()
{
  cUser_ = NULL;
}


std::vector<CID_T> StreamService::searchContent(const std::string& partial) const
{
    std::vector<CID_T> results;
    for(size_t i = 0; i < content_.size(); i++){
        if(partial == "*" || content_[i] -> name().find(partial) != std::string::npos){
            results.push_back(i);
        }        
    }
    return results;
}


std::vector<CID_T> StreamService::getUserHistory() const
{
    throwIfNoCurrentUser();
    return cUser_->history;
}

/**
     * @brief Updates data structures to indicate the current User has watched
     *        this content
     * 
     * @param contentID ID of the content to watch
     * @throw UserNotLoggedInError if no User is currently logged in.
     * @throw range_error      if the contentID is not valid
     * @throw RatingLimitError if the content's rating is above the User's 
     *                         rating limit
     */
void StreamService::watch(CID_T contentID)
{
  throwIfNoCurrentUser();
  if(!isValidContentID(contentID))
  {
    throw range_error("Invalid Content ID");
  }
  if(content_[contentID] -> rating() > cUser_ -> ratingLimit)
  {
    throw RatingLimitError("Restricted from watching");
  }
  cUser_ -> addToHistory(contentID);
  content_[contentID] -> addViewer(cUser_ -> uname);
}

  /**
     * @brief Add a review with the given number of stars to the specified
     *        content. We will NOT worry about a user reviewing content
     *        more than once.
     * 
     * @param contentID ContentID to review
     * @param numStars Number of stars for the review (must be 0-5)
     * @throw UserNotLoggedInError if no User is currently logged in.
     * @throw ReviewRangeError if the contentID is not valid or the 
     *                    number of stars is out of the range 0-5
     */
void StreamService::reviewShow(CID_T contentID, int numStars)
{
  throwIfNoCurrentUser();
  if(!isValidContentID(contentID) || numStars < 0 || numStars > 5)
  {
    throw ReviewRangeError("Please enter a rating from 0 - 5");
  }
  content_[contentID] -> review(numStars);
}

/**
     * @brief Returns the ID of the chosen content suggestion
     *  
     * For the given content, Cp, and current user, Uq, consider the set of 
     * Users, Uj, (Uj /= Uq) who also watched content Cp and find the single, 
     * content item, Ck, (Ck /= Cp) that was viewed the most times by users
     * in the set, Uj.
     * 
     * @param contentID ID of the content for which we want suggested similar
     *                  content
     * @return CID_T of the suggestion.
     * @throw UserNotLoggedInError if no User is currently logged in.
     * @throw range_error      if the contentID is not valid
     **/
CID_T StreamService::suggestBestSimilarContent(CID_T contentID) const
{
  throwIfNoCurrentUser();
  if(!isValidContentID(contentID)){throw range_error("Not a valid ID");}
  vector<int> contentCount(content_.size(), 0);
  for(const string& viewer : content_[contentID]->getViewers())
  {
    if(viewer != cUser_ -> uname)
    {
      int viewerIndex = getUserIndexByName(viewer);
        for (CID_T watchedContent : users_[viewerIndex]->history) 
        {
          
            if (watchedContent != contentID && !(cUser_ -> haveWatched(watchedContent))) 
            {
              contentCount[watchedContent]++;
            }
        }
    }
  }
  int max = 0;
  int maxIndex = -1;
  for(size_t k = 0; k < contentCount.size(); k++)
  {
    cout << "index: " << k << " count: " << contentCount[k] << endl;
    if(contentCount[k] > max)
    {
      max = contentCount[k]; 
      maxIndex = k;
    }
  }
  return maxIndex;
}


void StreamService::displayContentInfo(CID_T contentID) const
{
    // Do not alter this
    if(! isValidContentID(contentID)){
        throw std::invalid_argument("Watch: invalid contentID");
    }

    // Call the display abitlity of the appropriate content object
    cout << "========" << endl;
    cout << "ID: " << contentID << endl;
    cout << "Name: " << content_[contentID] -> name() << endl;
    cout << "Views: " << content_[contentID] -> getViewers().size() << endl;
    cout << "Average review: " << content_[contentID] -> getStarAverage() << endl;
    Series* seriesContent = dynamic_cast<Series*>(content_[contentID]);
    if (seriesContent != nullptr) {
        cout << "Number of Episodes: " << seriesContent -> numEpisodes() << endl;
    }
}

bool StreamService::isValidContentID(CID_T contentID) const
{
    return (contentID >= 0) && (contentID < (int) content_.size());
}


void StreamService::throwIfNoCurrentUser() const
{
    if(cUser_ == NULL){
        throw UserNotLoggedInError("No user is logged in");
    }
}


int StreamService::getUserIndexByName(const std::string& uname) const
{
    for(size_t i = 0; i < users_.size(); i++){
        if(uname == users_[i]->uname) {
            return (int)i;
        }
    }
    return -1;
}
