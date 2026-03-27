/**
 *  RateLimitFilter.cc
 */

#include "RateLimitFilter.h"
#include <drogon/drogon.h>

// Per-IP request counters shared across all filter instances.
// Drogon dispatches each I/O event on a dedicated event-loop thread, and
// a single request is processed entirely on one thread; concurrent access
// to this map from the same I/O thread is therefore already serialized by
// the event loop and does not require an explicit mutex.
static std::unordered_map<std::string, RateLimitFilter::Counter>
    g_requestCounts;

void RateLimitFilter::doFilter(const HttpRequestPtr &req,
                               FilterCallback &&cb,
                               FilterChainCallback &&ccb)
{
    const std::string ip = req->peerAddr().toIp();
    const auto now = std::chrono::steady_clock::now();

    auto &counter = g_requestCounts[ip];

    const auto elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(
            now - counter.windowStart)
            .count();
    if (elapsed >= windowSeconds_)
    {
        counter.count = 0;
        counter.windowStart = now;
    }

    ++counter.count;

    if (counter.count > maxRequests_)
    {
        auto resp = HttpResponse::newHttpResponse();
        resp->setStatusCode(k429TooManyRequests);
        cb(resp);
        return;
    }

    ccb();
}
