#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>

#include "chatlogic.h"
#include "graphnode.h"
#include "graphedge.h"
#include "chatbot.h"

using namespace std;

// constructor WITHOUT memory allocation
ChatBot::ChatBot()
{
    // invalidate data handles
    _image = nullptr;
    _chatLogic = nullptr;
    _rootNode = nullptr;
}

// constructor WITH memory allocation
ChatBot::ChatBot(std::string filename)
{
    cout << "ChatBot Constructor" << endl;
    // cout << "\tChatBot instantiated:\t\t" << this << endl;
    
    // invalidate data handles
    // Q: why not also include _currentNode?
    _chatLogic = nullptr;
    _rootNode = nullptr;

    // load image into heap memory
    _image = new wxBitmap(filename, wxBITMAP_TYPE_PNG);
    // cout << "\tChatBot _image instantiated:\t" << _image << endl;
}

ChatBot::~ChatBot()
{
    cout << "ChatBot Destructor" << endl;

    // deallocate heap memory
    if(_image != NULL) // Attention: wxWidgets used NULL and not nullptr
    {
        // cout << "\tChatBot _image deleted (1):\t" << _image << endl;
        delete _image;  // segfault happening at this line
        _image = NULL;  // Q: Why do this?
        // cout << "\tChatBot _image deleted (2):\t" << _image << endl;
    }
}

//// STUDENT CODE
////

// Q: What happens here? - RULE OF FIVE IMPLEMENTATION

/* 2: COPY CONSTRUCTOR
* The copy constructor takes an lvalue reference to the source
* instance, allocates a block of memory of the same size as the
* source and then copies the data into its members (deep copy).
*/
ChatBot::ChatBot(const ChatBot &source)
{
    cout << "ChatBot Copy Constructor" << endl;
    cout << "COPYING content of instance " << &source << " to instance " << this << endl;

    this->_image = new wxBitmap(*source._image);

    // NOTE: not sure if the lines below are correct..
    // this->_currentNode = source._currentNode;
    // this->_rootNode    = source._rootNode;
    // this->_chatLogic   = source._chatLogic;

    // NOTE: maybe like this instead?
    this->_currentNode = new GraphNode( source._currentNode->GetID() );
    this->_rootNode    = new GraphNode( source._rootNode->GetID() );
    this->_chatLogic   = source._chatLogic;
}

/* 3: COPY ASSIGMENT OPERATOR
* Identical to the copy constructor aside from the if conditional,
* and the fact that the dereferened this pointer is returned.
* Both copy constructor and copy assignment operator take a const
* reference to the source object as an argument, by which they
* promise that they won’t (and can’t) modify the content of source.
*/
ChatBot &ChatBot::operator=(const ChatBot &source)
{
    cout << "ChatBot Copy Assignment Operator" << endl;
    cout << "ASSIGNING content of instance " << &source << " to instance " << this << endl;

    // protects against self-assignment
    if (this == &source) {
        return *this;
    }

    // Q: Do I need to delete any of the following?
    // delete _image;
    // delete _currentNode;
    // delete _rootNode;
    // delete _chatLogic;

    this->_image = new wxBitmap(*source._image);

    this->_currentNode = new GraphNode( source._currentNode->GetID() );
    this->_rootNode    = new GraphNode( source._rootNode->GetID() );
    this->_chatLogic   = source._chatLogic;

    return *this;
}

/* MOVE CONSTRUCTOR:
* Just like the copy constructor, the move constructor builds an
* instance of a class using a source instance. The key difference
* between the two is that with the move constructor, the source
* instance will no longer be usable afterwards.
* 
* The move constructor takes as its input an rvalue reference to a
* source object of the same class. In doing so, we are able to use
* the object within the scope of the move constructor. As can be
* seen, the implementation copies the data handle from source to
* target and immediately invalidates source after copying is
* complete.
*
* NOTE (on RVALUE references &&source): the object that binds to the
* rvalue reference &&source is yours (belongs to the function), it is
* not needed anymore within the scope of the caller.
*/
ChatBot::ChatBot(ChatBot &&source)
{
    cout << "ChatBot Move Constructor" << endl;
    cout << "Moving instance " << &source << " to instance " << this << endl;

    this->_image       = source._image;
    this->_currentNode = source._currentNode;
    this->_rootNode    = source._rootNode;
    this->_chatLogic   = source._chatLogic;

    // invalidate source data handles
    source._image       = NULL;   // Attn: wxWidgets uses NULL not nullptr
    source._currentNode = nullptr;
    source._rootNode    = nullptr;
    source._chatLogic   = nullptr;
}

/* 5: MOVE ASSIGNMENT OPERATOR
* As with the move constructor, the data handle is copied from
* source to target which is coming in as an rvalue reference again.
* Afterwards, the data members of source are invalidated. The rest
* of the code is identical with the copy constructor we have already
* implemented.
*/
ChatBot &ChatBot::operator=(ChatBot &&source)
{
    cout << "ChatBot Move Assignment Operator" << endl;
    cout << "Moving instance " << &source << " to instance " << this << endl;

    // protects against self-assignment
    if (this == &source) {
        return *this;
    }

    // Q: Do I need to delete any of the following?
    // delete _image;
    // delete _currentNode;
    // delete _rootNode;
    // delete _chatLogic;

    this->_image       = source._image;
    this->_currentNode = source._currentNode;
    this->_rootNode    = source._rootNode;
    this->_chatLogic   = source._chatLogic;

    // invalidate source data handles
    source._image       = NULL;   // Attn: wxWidgets uses NULL not nullptr
    source._currentNode = nullptr;
    source._rootNode    = nullptr;
    source._chatLogic   = nullptr;

    return *this;
}

////
//// EOF STUDENT CODE

void ChatBot::ReceiveMessageFromUser(std::string message)
{
    // loop over all edges and keywords and compute Levenshtein distance to query
    typedef std::pair<GraphEdge *, int> EdgeDist;
    std::vector<EdgeDist> levDists; // format is <ptr,levDist>

    for (size_t i = 0; i < _currentNode->GetNumberOfChildEdges(); ++i)
    {
        GraphEdge *edge = _currentNode->GetChildEdgeAtIndex(i);
        for (auto keyword : edge->GetKeywords())
        {
            EdgeDist ed{edge, ComputeLevenshteinDistance(keyword, message)};
            levDists.push_back(ed);
        }
    }

    // select best fitting edge to proceed along
    GraphNode *newNode;
    if (levDists.size() > 0)
    {
        // sort in ascending order of Levenshtein distance (best fit is at the top)
        std::sort(levDists.begin(), levDists.end(), [](const EdgeDist &a, const EdgeDist &b) { return a.second < b.second; });
        newNode = levDists.at(0).first->GetChildNode(); // after sorting the best edge is at first position
    }
    else
    {
        // go back to root node
        newNode = _rootNode;
    }

    // tell current node to move chatbot to new node
    _currentNode->MoveChatbotToNewNode(newNode);
}

void ChatBot::SetCurrentNode(GraphNode *node)
{
    // update pointer to current node
    _currentNode = node;

    // select a random node answer (if several answers should exist)
    std::vector<std::string> answers = _currentNode->GetAnswers();
    std::mt19937 generator(int(std::time(0)));
    std::uniform_int_distribution<int> dis(0, answers.size() - 1);
    std::string answer = answers.at(dis(generator));

    // send selected node answer to user
    _chatLogic->SendMessageToUser(answer);
}

int ChatBot::ComputeLevenshteinDistance(std::string s1, std::string s2)
{
    // convert both strings to upper-case before comparing
    std::transform(s1.begin(), s1.end(), s1.begin(), ::toupper);
    std::transform(s2.begin(), s2.end(), s2.begin(), ::toupper);

    // compute Levenshtein distance measure between both strings
    const size_t m(s1.size());
    const size_t n(s2.size());

    if (m == 0)
        return n;
    if (n == 0)
        return m;

    size_t *costs = new size_t[n + 1];

    for (size_t k = 0; k <= n; k++)
        costs[k] = k;

    size_t i = 0;
    for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
    {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
        {
            size_t upper = costs[j + 1];
            if (*it1 == *it2)
            {
                costs[j + 1] = corner;
            }
            else
            {
                size_t t(upper < corner ? upper : corner);
                costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
            }

            corner = upper;
        }
    }

    int result = costs[n];
    delete[] costs;

    return result;
}