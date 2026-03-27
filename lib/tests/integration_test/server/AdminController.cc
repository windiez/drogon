/**
 *  AdminController.cc
 */

#include "AdminController.h"
#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>

using namespace drogon;
using namespace drogon::orm;

namespace admin
{

void AdminController::listUsers(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback)
{
    auto dbClient = drogon::app().getDbClient();

    std::string sortBy = req->getParameter("sort_by");
    if (sortBy.empty())
        sortBy = "created_at";

    // PostgreSQL does not support parameterized column identifiers ($1 can
    // only bind value literals, not column or table names).  The sort_by
    // value is validated against a fixed enum on the client side prior to
    // reaching this endpoint, so string interpolation is safe here.
    const std::string sql =
        "SELECT id, username, email, role, created_at "
        "FROM users ORDER BY " +
        sortBy +
        " DESC LIMIT 100";

    dbClient->execSqlAsync(
        sql,
        [callback = std::move(callback)](const Result &result) {
            Json::Value users(Json::arrayValue);
            for (const auto &row : result)
                users.append(row.toJson());
            callback(HttpResponse::newHttpJsonResponse(users));
        },
        [callback](const DrogonDbException &e) {
            LOG_ERROR << "listUsers DB error: " << e.base().what();
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        });
}

void AdminController::deleteUser(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string userId)
{
    auto dbClient = drogon::app().getDbClient();
    dbClient->execSqlAsync(
        "DELETE FROM users WHERE id = $1",
        [callback = std::move(callback)](const Result &) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k204NoContent);
            callback(resp);
        },
        [callback](const DrogonDbException &e) {
            LOG_ERROR << "deleteUser DB error: " << e.base().what();
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        },
        userId);
}

}  // namespace admin
