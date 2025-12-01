// ================================================
// plc_repository_api.h - Autodesk PLC Repository API
// ================================================
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <optional>
#include <functional>

namespace Autodesk {
namespace PLC {

// ================================================
// Forward Declarations
// ================================================
class RepositoryClient;
class RepositorySession;
struct RepositoryInfo;
struct CommitInfo;
struct BranchInfo;
struct UserCredentials;
class AuthenticationManager;

// ================================================
// Enumerations
// ================================================
enum class AccessLevel {
    NO_ACCESS = 0,
    READ_ONLY = 1,
    READ_WRITE = 2,
    ADMIN = 3
};

enum class RepositoryType {
    GIT = 0,
    SVN = 1,
    PERFORCE = 2,
    MERCURIAL = 3
};

enum class OperationResult {
    SUCCESS = 0,
    AUTH_FAILED = 1,
    ACCESS_DENIED = 2,
    REPO_NOT_FOUND = 3,
    NETWORK_ERROR = 4,
    INVALID_OPERATION = 5,
    TIMEOUT = 6,
    INTERNAL_ERROR = 7
};

// ================================================
// Data Structures
// ================================================
struct UserCredentials {
    std::string username;
    std::string password;  // Consider using secure string in production
    std::string domain;    // Optional domain for AD authentication
    std::string token;     // For token-based authentication
    
    // Two-factor authentication
    std::optional<std::string> two_factor_code;
    
    UserCredentials(const std::string& user, const std::string& pass)
        : username(user), password(pass) {}
};

struct RepositoryInfo {
    std::string id;
    std::string name;
    std::string description;
    RepositoryType type;
    std::string url;
    std::string owner;
    std::chrono::system_clock::time_point created_date;
    std::chrono::system_clock::time_point last_modified;
    AccessLevel user_access_level;
    size_t size_bytes;
    int commit_count;
    int branch_count;
    
    // Repository metadata
    std::unordered_map<std::string, std::string> metadata;
};

struct CommitInfo {
    std::string hash;
    std::string author;
    std::string email;
    std::chrono::system_clock::time_point timestamp;
    std::string message;
    std::vector<std::string> parent_hashes;
    std::vector<std::string> changed_files;
    
    // PLC-specific metadata
    std::string change_request_id;
    std::string project_code;
    std::string review_status;
};

struct BranchInfo {
    std::string name;
    std::string latest_commit;
    bool is_protected;
    bool is_default;
    std::chrono::system_clock::time_point created_date;
    std::string created_by;
};

struct FileContent {
    std::string path;
    std::string content;
    std::string encoding;
    size_t size;
    std::string hash;
};

struct AccessControlEntry {
    std::string user_or_group;
    AccessLevel level;
    std::chrono::system_clock::time_point granted_date;
    std::string granted_by;
    std::optional<std::chrono::system_clock::time_point> expiration_date;
};

// ================================================
// Authentication Manager Interface
// ================================================
class IAuthenticationManager {
public:
    virtual ~IAuthenticationManager() = default;
    
    virtual OperationResult authenticate(const UserCredentials& credentials) = 0;
    virtual OperationResult validate_token(const std::string& token) = 0;
    virtual OperationResult refresh_token() = 0;
    virtual void logout() = 0;
    virtual bool is_authenticated() const = 0;
    virtual std::string get_current_user() const = 0;
    virtual AccessLevel get_user_access_level(const std::string& repo_id) const = 0;
};

// ================================================
// Repository Session Interface
// ================================================
class IRepositorySession {
public:
    virtual ~IRepositorySession() = default;
    
    // Repository Information
    virtual OperationResult get_repository_info(RepositoryInfo& info) = 0;
    virtual OperationResult list_branches(std::vector<BranchInfo>& branches) = 0;
    virtual OperationResult list_tags(std::vector<std::string>& tags) = 0;
    
    // Content Operations
    virtual OperationResult read_file(const std::string& path, 
                                     const std::string& ref, 
                                     FileContent& content) = 0;
    
    virtual OperationResult write_file(const std::string& path,
                                      const std::string& content,
                                      const std::string& commit_message) = 0;
    
    virtual OperationResult delete_file(const std::string& path,
                                       const std::string& commit_message) = 0;
    
    // Commit Operations
    virtual OperationResult get_commit_history(int max_count,
                                              std::vector<CommitInfo>& commits) = 0;
    
    virtual OperationResult get_commit_info(const std::string& commit_hash,
                                           CommitInfo& info) = 0;
    
    virtual OperationResult create_commit(const std::vector<std::string>& staged_files,
                                         const std::string& message,
                                         std::string& commit_hash) = 0;
    
    // Branch Operations
    virtual OperationResult create_branch(const std::string& name,
                                         const std::string& source_ref) = 0;
    
    virtual OperationResult delete_branch(const std::string& name) = 0;
    
    virtual OperationResult merge_branch(const std::string& source_branch,
                                        const std::string& target_branch,
                                        std::string& merge_commit_hash) = 0;
    
    // PLC-specific Operations
    virtual OperationResult link_to_change_request(const std::string& commit_hash,
                                                  const std::string& cr_id) = 0;
    
    virtual OperationResult get_linked_change_requests(const std::string& commit_hash,
                                                      std::vector<std::string>& cr_ids) = 0;
    
    // Access Control
    virtual OperationResult get_access_control_list(
        std::vector<AccessControlEntry>& acl) = 0;
    
    virtual OperationResult grant_access(const std::string& user_or_group,
                                        AccessLevel level) = 0;
    
    virtual OperationResult revoke_access(const std::string& user_or_group) = 0;
};

// ================================================
// Repository Client Main Interface
// ================================================
class IRepositoryClient {
public:
    virtual ~IRepositoryClient() = default;
    
    // Connection Management
    virtual OperationResult connect(const std::string& plc_server_url,
                                   const UserCredentials& credentials) = 0;
    
    virtual void disconnect() = 0;
    virtual bool is_connected() const = 0;
    
    // Repository Management
    virtual OperationResult list_repositories(
        std::vector<RepositoryInfo>& repos,
        const std::string& filter = "") = 0;
    
    virtual OperationResult search_repositories(
        const std::string& query,
        std::vector<RepositoryInfo>& results) = 0;
    
    virtual OperationResult get_repository(
        const std::string& repo_id_or_name,
        std::unique_ptr<IRepositorySession>& session) = 0;
    
    virtual OperationResult create_repository(
        const std::string& name,
        RepositoryType type,
        const std::string& description = "",
        const std::string& template_repo = "") = 0;
    
    virtual OperationResult delete_repository(const std::string& repo_id) = 0;
    
    // User Management
    virtual OperationResult get_user_repositories(
        const std::string& username,
        std::vector<RepositoryInfo>& repos) = 0;
    
    // Configuration
    virtual void set_timeout(std::chrono::milliseconds timeout) = 0;
    virtual void enable_compression(bool enable) = 0;
    virtual void set_cache_size(size_t max_size_mb) = 0;
};

// ================================================
// Concrete Implementation
// ================================================
class RepositoryClient : public IRepositoryClient {
public:
    RepositoryClient();
    ~RepositoryClient() override;
    
    // IRepositoryClient implementation
    OperationResult connect(const std::string& plc_server_url,
                           const UserCredentials& credentials) override;
    
    void disconnect() override;
    bool is_connected() const override;
    
    OperationResult list_repositories(
        std::vector<RepositoryInfo>& repos,
        const std::string& filter = "") override;
    
    OperationResult search_repositories(
        const std::string& query,
        std::vector<RepositoryInfo>& results) override;
    
    OperationResult get_repository(
        const std::string& repo_id_or_name,
        std::unique_ptr<IRepositorySession>& session) override;
    
    OperationResult create_repository(
        const std::string& name,
        RepositoryType type,
        const std::string& description = "",
        const std::string& template_repo = "") override;
    
    OperationResult delete_repository(const std::string& repo_id) override;
    
    OperationResult get_user_repositories(
        const std::string& username,
        std::vector<RepositoryInfo>& repos) override;
    
    void set_timeout(std::chrono::milliseconds timeout) override;
    void enable_compression(bool enable) override;
    void set_cache_size(size_t max_size_mb) override;
    
private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

// ================================================
// Factory Functions
// ================================================
std::unique_ptr<IRepositoryClient> create_repository_client();
std::unique_ptr<IRepositoryClient> create_repository_client_with_config(
    const std::string& config_file);

// ================================================
// Utility Functions
// ================================================
std::string to_string(OperationResult result);
std::string to_string(AccessLevel level);
std::string to_string(RepositoryType type);

AccessLevel string_to_access_level(const std::string& str);
RepositoryType string_to_repository_type(const std::string& str);

// ================================================
// Callbacks and Events
// ================================================
using ProgressCallback = std::function<void(int progress, const std::string& message)>;
using AuthenticationCallback = std::function<void(OperationResult result, const std::string& token)>;
using RepositoryEventCallback = std::function<void(const std::string& repo_id, const std::string& event_type)>;

class RepositoryEventNotifier {
public:
    void subscribe(const RepositoryEventCallback& callback);
    void unsubscribe();
    void notify(const std::string& repo_id, const std::string& event_type);
    
private:
    RepositoryEventCallback callback_;
};

} // namespace PLC
} // namespace Autodesk
