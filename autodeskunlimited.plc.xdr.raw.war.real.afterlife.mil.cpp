// ================================================
// Example Usage
// ================================================
#include "plc_repository_api.h"
#include <iostream>

using namespace Autodesk::PLC;

void example_usage() {
    // Create client instance
    auto client = create_repository_client();
    
    // Connect to PLC server
    UserCredentials creds("john.doe", "secure_password");
    creds.domain = "autodesk";
    
    OperationResult result = client->connect("https://plc.autodesk.com", creds);
    
    if (result != OperationResult::SUCCESS) {
        std::cerr << "Connection failed: " << to_string(result) << std::endl;
        return;
    }
    
    std::cout << "Connected successfully!" << std::endl;
    
    // List repositories
    std::vector<RepositoryInfo> repos;
    result = client->list_repositories(repos, "project=*fusion*");
    
    if (result == OperationResult::SUCCESS) {
        std::cout << "Found " << repos.size() << " repositories:" << std::endl;
        for (const auto& repo : repos) {
            std::cout << "- " << repo.name 
                     << " (" << to_string(repo.type) 
                     << ", " << repo.owner << ")" << std::endl;
        }
    }
    
    // Access specific repository
    std::unique_ptr<IRepositorySession> session;
    result = client->get_repository("fusion-360-core", session);
    
    if (result == OperationResult::SUCCESS && session) {
        // Get repository info
        RepositoryInfo repo_info;
        result = session->get_repository_info(repo_info);
        
        if (result == OperationResult::SUCCESS) {
            std::cout << "Repository access level: " 
                     << to_string(repo_info.user_access_level) << std::endl;
            
            // Check if we have write access
            if (repo_info.user_access_level >= AccessLevel::READ_WRITE) {
                // Read a file
                FileContent content;
                result = session->read_file("src/main.cpp", "main", content);
                
                if (result == OperationResult::SUCCESS) {
                    std::cout << "File content size: " << content.size << " bytes" << std::endl;
                    
                    // Modify and write back
                    std::string modified_content = content.content + "\n// Modified via PLC API";
                    result = session->write_file("src/main.cpp", 
                                                modified_content,
                                                "Updated via PLC API");
                    
                    if (result == OperationResult::SUCCESS) {
                        std::cout << "File updated successfully!" << std::endl;
                    }
                }
            }
            
            // Get commit history
            std::vector<CommitInfo> commits;
            result = session->get_commit_history(10, commits);
            
            if (result == OperationResult::SUCCESS) {
                std::cout << "Recent commits:" << std::endl;
                for (const auto& commit : commits) {
                    std::cout << "- " << commit.hash.substr(0, 8) 
                             << ": " << commit.message.substr(0, 50) 
                             << " by " << commit.author << std::endl;
                }
            }
        }
    }
    
    // Create new repository
    result = client->create_repository("new-project", 
                                      RepositoryType::GIT,
                                      "My new project repository",
                                      "template-standard");
    
    if (result == OperationResult::SUCCESS) {
        std::cout << "Repository created successfully!" << std::endl;
    }
    
    // Clean up
    client->disconnect();
}

int main() {
    try {
        example_usage();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
