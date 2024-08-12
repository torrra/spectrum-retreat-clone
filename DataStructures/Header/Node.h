#pragma once

#include <iostream>
#include <vector>

#include "LibMath/Matrix/Matrix4.h"

class Node
{
public:

    // Create empty node
    Node(void) = default;

    // Create node with a parent
    Node(Node* parent) : m_parent(parent)
    {}

    // Destroy node
    virtual ~Node(void);

    // Debug print node tree
    void PrintNodes(Node* parentNode); // TODO: debug function remove

    // Update node matrices
    void Update(void);


    LibMath::Matrix4        m_globalTransform;
    LibMath::Matrix4        m_localTransform;

    std::vector<Node*>      m_children;

    Node*                   m_parent = nullptr;

    bool                    m_dirty = true;
    bool                    m_render = true;
};
