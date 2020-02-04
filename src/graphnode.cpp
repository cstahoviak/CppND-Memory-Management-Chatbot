#include "graphedge.h"
#include "graphnode.h"

// added by me
#include "chatlogic.h"
#include <iostream>
#include <memory>

using namespace std;

GraphNode::GraphNode(int id)
{
    // std::cout << "GraphNode Constructor" << std::endl;
    _id = id;
}

GraphNode::~GraphNode()
{
    //// STUDENT CODE
    ////

    // std::cout << "GraphNode Destructor" << std::endl;

    // Q: Why does it not segfault at the delete statement??
    // std::cout << "\t_chatBot deleted:\t\t" << _chatBot << std::endl;
    // delete _chatBot;     // commeted out to complete Task Zero

    ////
    //// EOF STUDENT CODE
}

void GraphNode::AddToken(std::string token)
{
    _answers.push_back(token);
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
// void GraphNode::AddEdgeToParentNode(std::unique_ptr<GraphEdge> edge)
{
    _parentEdges.push_back(edge);
    // _parentEdges.push_back(edge.get());
}

void GraphNode::AddEdgeToChildNode(GraphEdge *edge)
// void GraphNode::AddEdgeToChildNode(std::unique_ptr<GraphEdge> edge)
{
    // _childEdges.push_back(edge);
    _childEdges.push_back(std::unique_ptr<GraphEdge>(edge));
}

//// STUDENT CODE
////

// void GraphNode::MoveChatbotHere(ChatBot *chatbot)
// void GraphNode::MoveChatbotHere(std::unique_ptr<ChatBot> chatbot)
// {
//     cout << "\tChatBot " << chatbot.get() << " moved to node " << this << endl;
//     // _chatBot = chatbot;
//     _chatBot = std::move(chatbot);
//     _chatBot->SetCurrentNode(this);
// }

// void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
// {
//     cout << "Moving ChatBot " << _chatBot.get() << " from node " << this << endl;
//     // newNode->MoveChatbotHere(_chatBot);
//     newNode->MoveChatbotHere(std::move(_chatBot));
//     _chatBot = nullptr; // invalidate pointer at source
// }

// input arg &&chatbot is an rvalue reference to a ChatBot object
void GraphNode::MoveChatbotHere(ChatBot &&chatbot)
{
    string str = "MoveChatbotHere() ";
    cout << str << "ChatBot " << &chatbot << " moved to node " << this << endl;
    _chatBot = std::move(chatbot);

    // need to update ChatLogic._chatBot handle that points to the ChatBot
    _chatBot.GetChatLogicHandle()->SetChatbotHandle(&_chatBot);
    cout << str << "ChatLogic instance " << _chatBot.GetChatLogicHandle() <<
        " now points to ChatBot at " << &_chatBot << endl;

    _chatBot.SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    string str = "MoveChatbotToNewNode() ";
    cout << str << "Moving ChatBot " << &_chatBot << " from node " << this << 
        " to node " << newNode << endl;
    newNode->MoveChatbotHere(std::move(_chatBot));
    // _chatBot = nullptr; // invalidate pointer at source
}
////
//// EOF STUDENT CODE

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
// std::unique_ptr<GraphEdge> GraphNode::GetChildEdgeAtIndex(int index)
{
    //// STUDENT CODE
    ////

    return (_childEdges[index]).get();

    ////
    //// EOF STUDENT CODE
}