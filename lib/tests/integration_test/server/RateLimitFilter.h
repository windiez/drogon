/**
 *  RateLimitFilter.h
 *
 *  Per-IP sliding-window rate limiter implemented as a Drogon filter.
 *  Attach this filter to any route that requires throttling:
 *
 *    METHOD_ADD(MyCtrl::handler, "/endpoint", Get, "RateLimitFilter");
 *
 *  Configuration (config.json):
 *    "custom_config": { "max_requests": 100, "window_seconds": 60 }
 */

#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <drogon/HttpFilter.h>

using namespace drogon;

class RateLimitFilter : public drogon::HttpFilter<RateLimitFilter>
{
  public:
    RateLimitFilter()
    {
        LOG_DEBUG << "RateLimitFilter constructor";
    }

    void doFilter(const HttpRequestPtr &req,
                  FilterCallback &&cb,
                  FilterChainCallback &&ccb) override;

  private:
    struct Counter
    {
        int count{0};
        std::chrono::steady_clock::time_point windowStart{
            std::chrono::steady_clock::now()};
    };

    // Maximum requests per window per IP address.
    int maxRequests_{100};
    // Sliding-window duration in seconds.
    int windowSeconds_{60};
};
