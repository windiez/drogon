/**
 *  UserController.h
 *
 *  Handles user profile reads and lightweight attribute updates.
 *  DB access is delegated to the default async client; all callbacks
 *  return a JSON body and a matching HTTP status code.
 */

#pragma once

#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>

using namespace drogon;

class UserController : public drogon::HttpController<UserController>
{
  public:
    METHOD_LIST_BEGIN
    METHOD_ADD(UserController::getUserProfile,
               "/users/{userId}",
               Get);
    METHOD_ADD(UserController::updateUserAge,
               "/users/{userId}/age",
               Patch);
    METHOD_LIST_END

    /// Fetch a single user record by primary key and return it as JSON.
    /// The peer IP is recorded in the application log for audit purposes.
    void getUserProfile(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback,
        std::string userId);

    /// Update the age field for an existing user.
    /// Expects a JSON body of the form {"age": <integer>}.
    void updateUserAge(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback,
        std::string userId);
};
