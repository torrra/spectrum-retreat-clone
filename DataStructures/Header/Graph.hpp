#pragma once

#include <unordered_map>
#include <string>

template<typename NodeT>
class Graph
{
public:

	// Create empty graph
	Graph() = default;

	// Recursively delete all nodes
	~Graph()
	{
		if (m_worldRoot)
			delete m_worldRoot;
	}


	template <typename Tobj, typename... TArgs>
	Tobj* AddChild(const std::string key, const TArgs&... args)
	{
		// Create object
		Tobj* newObj = new Tobj(args...);

		// Create node to hold object
		NodeT* newNode = new NodeT(nullptr, newObj);


		if (!m_worldRoot)
			AddRoot();

		m_worldRoot->AddNode(key, newNode);

		m_worldRoot->m_objects[key] = newNode;

		return newObj;
	}


	template <typename Tobj, typename... TArgs>
	Tobj* AddChild(const std::string parentKey, const std::string key, const TArgs&... args)
	{
		// Create object
		Tobj* newObj = new Tobj(args...);

		NodeT* parent = GetNode(parentKey);

		// Create node to hold object
		NodeT* newNode = new NodeT(parent, newObj);


		if (!m_worldRoot)
			AddRoot();

		// Add to parent's vector
		if (parent)
			parent->m_children.push_back(newNode);
		else
			m_worldRoot->AddNode(key, newNode);

		m_worldRoot->m_objects[key] = newNode;

		return newObj;
	}

	NodeT* GetNode(const std::string key)
	{
		// Get iterator to object
		if (!m_worldRoot)
			return nullptr;

		auto found = m_worldRoot->m_objects.find(key);

		// Return resource if it exists
		if (found != m_worldRoot->m_objects.end())
			return dynamic_cast<NodeT*>(found->second);

		// Return nullptr if it it does not
		else
			return nullptr;
	}


	// Delete a node from its key
	void DeleteNode(const std::string key)
	{
		// Get node if it exists
		NodeT* targetNode = m_worldRoot->GetNode(key);

		// Delete it from its address
		if (targetNode)
			DeleteNode(targetNode);

		// Get object from hash map
		auto foundObj = m_worldRoot->m_objects.find(key);

		// Remove it from hash map
		if (foundObj != m_worldRoot->m_objects.end())
			m_worldRoot->m_objects.erase(foundObj);

	}

	// Delete node from its address
	void DeleteNode(NodeT* target)
	{
		// Delete from parent's vector
		if (target->m_parent)
		{
			auto childIt = target->m_parent->m_children.begin();

			// Loop through vector
			while (childIt != target->m_parent->m_children.end())
			{
				if (*childIt == target)
				{
					// Erase does not call recursive Node destructor
					target->m_parent->m_children.erase(childIt);


					break;
				}

				// Check next iterator
				else
					++childIt;
			}
		}

		// Delete from world root's vector
		else
		{
			auto childIt = m_worldRoot->m_children.begin();

			// Loop through vector
			while (childIt != m_worldRoot->m_children.end())
			{
				if (*childIt == target)
				{
					// Erase does not call recursive Node destructor
					m_worldRoot->m_children.erase(childIt);

					break;
				}

				// Check next iterator
				else
					++childIt;
			}
		}

		// Delete should call recursive Node destructor
		target->~NodeT();
		delete target;

	}

	// Update Graph from world root
	void UpdateGraph()
	{
		// Update root's children
		if (m_worldRoot)
			m_worldRoot->Update();
	}


	class RootNode
	{
		// Create empty root
		RootNode(void) = default;

		// Destroy all nodes + root
		~RootNode(void)
		{
			for (NodeT* node : m_children)
				delete node;
		}

		// Add parentless node
		NodeT* AddNode(const std::string key, NodeT* node)
		{
			NodeT* found = GetNode(key);

			if (found)
				delete found;

			if (node && !node->m_parent)
			{
				m_children.push_back(node);
			}


			return m_objects[key] = node;
		}

		// Retrieve node from key
		NodeT* GetNode(const std::string key)
		{
			// Get iterator to objects
			auto found = m_objects.find(key);

			// Return resource if it exists
			if (found != m_objects.end())
				return dynamic_cast<NodeT*>(found->second);

			// Return nullptr if it it does not
			else
				return nullptr;
		}

		// Update all nodes
		void Update()
		{
			for (NodeT* node : m_children)
			{
				node->Update();
			}
		}

		void Clear()
		{
			if (m_worldRoot)
				delete m_worldRoot;
		}

		// Unordered map to get objects easily
		std::unordered_map<std::string, NodeT*>				m_objects;

	public:
		// Root's children; parentless nodes
		std::vector<NodeT*>									m_children;

		friend class Graph;
	};

public:
	// Add root to graph
	void AddRoot()
	{
		// Delete previous node(s)
		if (m_worldRoot)
			delete m_worldRoot;

		// Create new nodes
		m_worldRoot = new RootNode();
	}

	RootNode* m_worldRoot = nullptr;

};