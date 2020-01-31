#ifndef CHATBOT_H_
#define CHATBOT_H_

#include <wx/bitmap.h>
#include <string>

class GraphNode; // forward declaration
class ChatLogic; // forward declaration

class ChatBot
{
private:
    // data handles (owned)
    // NOTE: "will require use of move semantics to ensure than the handle to
    // this heap-allocated object is not lost."
    wxBitmap *_image; // avatar image

    // data handles (not owned)
    GraphNode *_currentNode;
    // NOTE: when a dead-end in the graph is reached the ChatBot will
    // return to the "root node"
    GraphNode *_rootNode;
    ChatLogic *_chatLogic;

    // proprietary functions
    int ComputeLevenshteinDistance(std::string s1, std::string s2);

public:
    // constructors / destructors
    ChatBot();                      // constructor WITHOUT memory allocation
    ChatBot(std::string filename);  // constructor WITH memory allocation
    ~ChatBot();

    //// STUDENT CODE
    ////
    ChatBot(const ChatBot &source);             // copy constructor
    ChatBot &operator=(const ChatBot &source);  // copy assignment aoperator
    ChatBot(ChatBot &&source);            // move constructor
    ChatBot &operator=(ChatBot &&source); // move assignment aoperator

    ////
    //// EOF STUDENT CODE

    // getters / setters
    void SetCurrentNode(GraphNode *node);
    void SetRootNode(GraphNode *rootNode) { _rootNode = rootNode; }
    void SetChatLogicHandle(ChatLogic *chatLogic) { _chatLogic = chatLogic; }
    wxBitmap *GetImageHandle() { return _image; }

    // communication
    void ReceiveMessageFromUser(std::string message);
};

#endif /* CHATBOT_H_ */