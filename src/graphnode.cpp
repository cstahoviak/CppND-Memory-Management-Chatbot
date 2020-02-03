#include "graphedge.h"
#include "graphnode.h"

// added by me
#include <iostream>
#include <memory>

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
void GraphNode::MoveChatbotHere(ChatBot *chatbot)
{
    _chatBot = chatbot;
    _chatBot->SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    newNode->MoveChatbotHere(_chatBot);
    _chatBot = nullptr; // invalidate pointer at source
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