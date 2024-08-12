#include <iostream>

#include "ResourceManager.h"

// Alias for readability
using Resource = std::pair<const std::string, IResource*>;

ResourceManager::~ResourceManager()
{
    // Get first umap element
    auto resIt(m_resources.begin());

    // Loop through all resources
    while (resIt != m_resources.end())
    {
        // Ger resource
        Resource res = *resIt;

        // Delete value
        if (res.second)
            delete res.second;

        // Remove from umap
        resIt = m_resources.erase(resIt);

    }
}

void ResourceManager::Delete(const std::string& key)
{
    // Find resource if it exists
    auto resourceIt = m_resources.find(key);

    if (resourceIt->second)
    {
        // Delete value
        delete resourceIt->second;

        // Remove from umap
        m_resources.erase(resourceIt);
    }

    // Tell user no resource was found if search fails
    else
        std::cout << "Failed to delete " << key << std::endl;
}

