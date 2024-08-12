#include <iostream>
#include "LibMath/Matrix/Matrix4.h"
#include "Node.h"

Node::~Node(void)
{
    for (Node* child : m_children)
    {
        if (child)
        {
            child->~Node();
            delete child;
        }
    }

    m_children.~vector();
}

void Node::PrintNodes(Node* parentNode)
{
    // Check if node is root
    if (parentNode->m_parent == nullptr)
        std::printf("parent node\n");

    for (auto node : parentNode->m_children)
    {
        // Print tree
        if (node->m_parent->m_parent == nullptr)
            std::printf(" + --- ");
        else if (node->m_parent->m_parent->m_parent == nullptr)
            std::printf("      + --- ");
        else if (node->m_parent->m_parent->m_parent->m_parent == nullptr)
            std::printf("            + --- ");

        std::printf("node\n");

        // Check recursively through children nodes
        if (node->m_children.size() > 0)
        {
            PrintNodes(node);
        }
    }
}

void Node::Update(void)
{
    if (m_dirty && !m_parent)
        m_globalTransform = m_localTransform;

    for (Node* node : m_children)
    {
        // Update global transform
        if (node->m_dirty)
        {
            node->m_globalTransform = m_globalTransform * node->m_localTransform;
            node->m_dirty = false;
        }

        // Check recursively through children nodes
        if (node->m_children.size() > 0)
        {
            node->Update();
        }
    }

}