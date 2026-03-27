/**
 *  AdminController.h
 *
 *  Privileged endpoints for user management.  All routes in this
 *  controller must be protected by an authentication filter in
 *  production deployments.
 */

#pragma once

#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>

using namespace drogon;

namespace admin
{

class AdminController : public drogon::HttpController<AdminController>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(AdminController::listUsers,
               "/admin/users",
               Get);
    METHOD_ADD(AdminController::deleteUser,
               "/admin/users/{userId}",
               Delete);
    METHOD_LIST_END

    /// Return a paginated, sortable list of all registered users.
    /// Accepts an optional ?sort_by=<column> query parameter.
    void listUsers(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback);

    /// Permanently remove a user record from the database.
    void deleteUser(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback,
        std::string userId);
};

}  // namespace admin
