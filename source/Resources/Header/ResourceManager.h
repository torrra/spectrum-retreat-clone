#pragma once

#include <unordered_map>
#include <string>

// Base class dor resources
class IResource
{
public:
                IResource() = default;
    virtual     ~IResource() = default;

};


// Class to create, store and retrieve resources
class ResourceManager
{
public:

    // Empty manager
    ResourceManager() = default;

    // Deallocate memory for every resource
    ~ResourceManager();


    // Retrieve resource as its original type from its key
    template<class ValT>
    ValT* Get(const std::string& key)
    {
        // Get iterator to resource
       auto found = m_resources.find(key);

       // Return resource if it exists
       if (found != m_resources.end())
           return dynamic_cast<ValT*>(found->second);

       // Return nullptr if it it does not
       else
           return nullptr;
    }

    template<class ValT, class... Targs>
    ValT* Create(const std::string& key, Targs... args)
    {
        // Check if resource with this key already exists
        ValT* resource = Get<ValT>(key);

        // Delete it if it exists
        if (resource)
            delete resource;

        // Create new resource
        resource = new ValT(key, args...);

        // Assign new resource
        m_resources[key] = dynamic_cast<IResource*>(resource);

        return resource;
    }

    // Delete resource from manager
    void Delete(const std::string& key);

private:

    // Store resources
    std::unordered_map<std::string, IResource*> m_resources;

};