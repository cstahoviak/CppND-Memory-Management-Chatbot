#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include <vector>
#include <string>
#include "chatbot.h"


// forward declarations
class GraphEdge;

class GraphNode
{
private:
    //// STUDENT CODE
    ////

    // data handles (owned)
    /* NOTE: "For now, edges are owned by ChatLogic which is storing
    *  the edges as well as nodes in a vector. In the final version,
    *  the child edges will be owned by a graph node."
    */
    std::vector<GraphEdge *> _childEdges;  // edges to subsequent nodes

    // data handles (not owned)
    std::vector<GraphEdge *> _parentEdges; // edges to preceding nodes

    /* "Handle to the ChatBot. Will be nullptr if the ChatBot is not
    *  currently at this particular node. When ChatBot comes to a
    *  particular node, it becomes a valid (heap?) memory address. And
    *  when ChaBot leaves the node, this pointer is invalidated again."
    */
    ChatBot *_chatBot;

    ////
    //// EOF STUDENT CODE

    // proprietary members (loaded from 'answergraph.txt' file)
    int _id;
    // could have multiple answers and randomize them, so as not to
    // have a completely deterministic ChatBot
    std::vector<std::string> _answers;

public:
    // constructor / destructor
    GraphNode(int id);
    ~GraphNode();

    // getter / setter
    int GetID() { return _id; }
    int GetNumberOfChildEdges() { return _childEdges.size(); }
    GraphEdge *GetChildEdgeAtIndex(int index);
    std::vector<std::string> GetAnswers() { return _answers; }
    int GetNumberOfParents() { return _parentEdges.size(); }

    // proprietary functions
    void AddToken(std::string token); // add answers to list
    void AddEdgeToParentNode(GraphEdge *edge);
    void AddEdgeToChildNode(GraphEdge *edge);

    //// STUDENT CODE
    ////

    void MoveChatbotHere(ChatBot *chatbot);

    ////
    //// EOF STUDENT CODE

    void MoveChatbotToNewNode(GraphNode *newNode);
};

#endif /* GRAPHNODE_H_ */