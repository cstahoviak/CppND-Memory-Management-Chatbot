#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include <algorithm>

#include "graphedge.h"
#include "graphnode.h"
#include "chatbot.h"
#include "chatlogic.h"

// added by me 
using namespace std;

ChatLogic::ChatLogic()
{
    //// STUDENT CODE
    ////

    // cout << "ChatLogic Constructor" << endl;

    ////
    //// EOF STUDENT CODE
}

ChatLogic::~ChatLogic()
{
    //// STUDENT CODE
    ////
    // cout << "ChatLogic Destructor" << endl;

    // delete all nodes
    // for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    // {
    //     // cout << "\tdeleting node at:\t\t" << &it << endl;
    //     delete *it;
    // }

    // TASK 4: _edges removed from instances of ChatLogic
    // delete all edges
    // for (auto it = std::begin(_edges); it != std::end(_edges); ++it)
    // {
    //     // cout << "\tdeleting edge at:\t\t" << &it << endl;
    //     delete *it;
    // }

    ////
    //// EOF STUDENT CODE
}

template <typename T>
void ChatLogic::AddAllTokensToElement(std::string tokenID, tokenlist &tokens, T &element)
{
    // find all occurences for current node
    auto token = tokens.begin();
    while (true)
    {
        token = std::find_if(token, tokens.end(), [&tokenID](const std::pair<std::string, std::string> &pair) { return pair.first == tokenID;; });
        if (token != tokens.end())
        {
            element.AddToken(token->second); // add new keyword to edge
            token++;                         // increment iterator to next element
        }
        else
        {
            break; // quit infinite while-loop
        }
    }
}

void ChatLogic::LoadAnswerGraphFromFile(std::string filename)
{
    // load file with answer graph elements
    std::ifstream file(filename);

    // check for file availability and process it line by line
    if (file)
    {
        // loop over all lines in the file
        std::string lineStr;
        while (getline(file, lineStr))
        {
            // extract all tokens from current line
            tokenlist tokens;
            while (lineStr.size() > 0)
            {
                // extract next token
                int posTokenFront = lineStr.find("<");
                int posTokenBack = lineStr.find(">");
                if (posTokenFront < 0 || posTokenBack < 0)
                    break; // quit loop if no complete token has been found
                std::string tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - 1);

                // extract token type and info
                int posTokenInfo = tokenStr.find(":");
                if (posTokenInfo != std::string::npos)
                {
                    std::string tokenType = tokenStr.substr(0, posTokenInfo);
                    std::string tokenInfo = tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

                    // add token to vector
                    tokens.push_back(std::make_pair(tokenType, tokenInfo));
                }

                // remove token from current line
                lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
            }

            // process tokens for current line
            auto type = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "TYPE"; });
            if (type != tokens.end())
            {
                // check for id
                auto idToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "ID"; });
                if (idToken != tokens.end())
                {
                    // extract id from token
                    int id = std::stoi(idToken->second);

                    // node-based processing
                    if (type->second == "NODE")
                    {
                        //// STUDENT CODE
                        ////

                        // check if node with this ID exists already
                        // auto newNode = std::find_if(_nodes.begin(), _nodes.end(), [&id](GraphNode *node) { return node->GetID() == id; });
                        auto newNode = std::find_if(_nodes.begin(), _nodes.end(), [&id](std::unique_ptr<GraphNode> &node) { return node->GetID() == id; });

                        // create new element if ID does not yet exist
                        if (newNode == _nodes.end())
                        {
                            // _nodes.emplace_back(new GraphNode(id));
                            _nodes.emplace_back(std::make_unique<GraphNode>(id));
                            newNode = _nodes.end() - 1; // get iterator to last element

                            // add all answers to current node
                            AddAllTokensToElement("ANSWER", tokens, **newNode);
                        }

                        ////
                        //// EOF STUDENT CODE
                    }

                    // edge-based processing
                    if (type->second == "EDGE")
                    {
                        //// STUDENT CODE
                        ////

                        // find tokens for incoming (parent) and outgoing (child) node
                        auto parentToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "PARENT"; });
                        auto childToken = std::find_if(tokens.begin(), tokens.end(), [](const std::pair<std::string, std::string> &pair) { return pair.first == "CHILD"; });

                        if (parentToken != tokens.end() && childToken != tokens.end())
                        {
                            // get iterator on incoming and outgoing node via ID search
                            auto parentNode = std::find_if(_nodes.begin(), _nodes.end(), [&parentToken](std::unique_ptr<GraphNode> &node) { return node->GetID() == std::stoi(parentToken->second); });
                            auto childNode = std::find_if(_nodes.begin(), _nodes.end(), [&childToken](std::unique_ptr<GraphNode> &node) { return node->GetID() == std::stoi(childToken->second); });

                            // create new edge
                            GraphEdge *edge = new GraphEdge(id);

                            /* Q: Make this a unique_ptr here?
                            *  A: No, this causes a problem below - see comment "store
                            * reference in child node and parent node". I think the
                            * problem is that the unique_ptr edge is trying to be
                            * copied and this is not allowed for unique_ptr
                            */
                            // unique_ptr<GraphEdge> edge = make_unique<GraphEdge>(id);

                            edge->SetChildNode( (*childNode).get() );
                            edge->SetParentNode( (*parentNode).get() );

                            // NOTE: vector of edges removed from ChatLogic class
                            // _edges.push_back(edge);

                            // find all keywords for current node
                            AddAllTokensToElement("KEYWORD", tokens, *edge);

                            /* TASK 4: Change ownership model of GraphEdges
                            * Now: Edges owned by ChatLogic class
                            * Goal: Transfer edge ownership to GraphNode instances
                            * from which the edges originate, i.e. transfer
                            * ownership to "parent" nodes
                            * 
                            * "Use move semantics when transferring ownership from
                            * class ChatLogic into instances of GraphNode
                            * 
                            * NOTE: use unique_ptr only! Not shared_ptr!
                            */

                            // store reference in child node and parent node
                            (*childNode)->AddEdgeToParentNode(edge);
                            (*parentNode)->AddEdgeToChildNode(edge);

                            // Why does this not work with edge as a unique_ptr?
                            // (*childNode)->AddEdgeToParentNode(edge.get());
                            // (*parentNode)->AddEdgeToChildNode(std::move(edge));
                        }

                        ////
                        //// EOF STUDENT CODE
                    }
                }
                else
                {
                    cout << "Error: ID missing. Line is ignored!" << endl;
                }
            }
        } // eof loop over all lines in the file

        file.close();

    } // eof check for file availability
    else
    {
        cout << "File could not be opened!" << endl;
        return;
    }

    //// STUDENT CODE
    ////

    // identify root node
    GraphNode *rootNode = nullptr;
    // for (auto it = std::begin(_nodes); it != std::end(_nodes); ++it)
    // {
    //     // search for nodes which have no incoming edges
    //     if ((*it)->GetNumberOfParents() == 0)
    //     {

    //         if (rootNode == nullptr)
    //         {
    //             // Q: Still not sure exactly how it.get() differs from
    //             // (*it).get() in this context? Why need to dereference?
    //             rootNode = (*it).get(); // assign current node to root
    //         }
    //         else
    //         {
    //             cout << "ERROR : Multiple root nodes detected" << endl;
    //         }
    //     }
    // }

    /* NOTE: ON ITERATORS (from Sasha)
    * "Basically iterators are to vectors what pointers are to arrays, plus the
    * existence of the end() iterator and other features... *it provides access
    * to the vector element currently pointed by the iterator it, and then the
    * rest is invocation of a method of the vector element, e.g. get()"
    */

    for( auto const &node_ptr : _nodes ) {
        // search for nodes which have no incoming edges
        if (node_ptr->GetNumberOfParents() == 0) {
            if (rootNode == nullptr) {
                // assign current node to root
                rootNode = node_ptr.get();
            }
            else {
                cout << "ERROR : Multiple root nodes detected" << endl;
            }
        }
    }

    // TASK 5: Create Chatbot here rather than in ChatLogic Constructor so that
    // it can be pased (via move semantics) into the root node

    /* NOTE: On Mvoe Constructor
    * A move constructor is executed only when you construct an object. A move
    * assignment operator is executed on a previously constructed object. It is
    * exactly the same scenario as in the copy case.
    * 
    * The only way that I could think of to force the Move Constructor to be
    * called was in the following way:
    * 
    * 1. Create a ChatBot object (on the heap) via make_unique
    * 2. Use move sematics to move the heap-allocated ChatBot object into a local
    *    (stack-allocated) ChatBot instance. This envokes the Move Constructor.
    */

    string str = "LoadAnswerGraphFromFile() ";

    // create unique_ptr chatbot (on stack) to ChatBot object on heap
    std::unique_ptr chatbot = std::make_unique<ChatBot>("../images/chatbot.png");
    // cout << str << "chatbot unique pointer STACK ADDRESS:\t" << &chatbot << endl;
    // cout << str << "chatbot object HEAP ADDRESS:\t\t" << chatbot.get() << endl;

    // create local (on stack) ChatBot object via move semantics (move constr.)
    ChatBot chatBot = std::move(*chatbot);  // move constructor
    // cout << str << "chatBot STACK ADDRESS:\t\t" << &chatBot << endl;

    // set non-owning reference to ChatBot object (on stack)
    _chatBot = &chatBot;
    // cout << str << "ChatLogic instance " << this << " points to ChatBot at " << _chatBot << endl;

    chatBot.SetChatLogicHandle(this);
    chatBot.SetRootNode(rootNode);
    // cout << str << "Passing ChatBot to root node:\t\t" << rootNode << endl;

    // pass an r-value ref - this does NOT call the move assignment operator
    rootNode->MoveChatbotHere(std::move(chatBot)); 

    ////
    //// EOF STUDENT CODE
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot)
{
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(std::string message)
{
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(std::string message)
{
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot()
{
    return _chatBot->GetImageHandle();
}
