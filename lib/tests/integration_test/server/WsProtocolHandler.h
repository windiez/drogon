/**
 *  WsProtocolHandler.h
 *
 *  WebSocket controller that accepts binary frames carrying a custom
 *  length-prefixed protocol.  Each frame begins with an 8-byte
 *  little-endian uint64 declaring the payload size, followed by the
 *  payload bytes.  Text frames and control frames are forwarded to
 *  the default handler without modification.
 *
 *  The per-connection maximum frame size is configurable via
 *  config.json: { "max_frame_bytes": 10485760 }.
 */

#pragma once

#include <cstdint>
#include <drogon/WebSocketController.h>

using namespace drogon;

class WsProtocolHandler
    : public drogon::WebSocketController<WsProtocolHandler>
{
  public:
    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/ws/binary");
    WS_PATH_LIST_END

    void handleNewMessage(const WebSocketConnectionPtr &wsConn,
                          std::string &&message,
                          const WebSocketMessageType &type) override;

    void handleConnectionClosed(
        const WebSocketConnectionPtr &wsConn) override;

    void handleNewConnection(const HttpRequestPtr &req,
                             const WebSocketConnectionPtr &wsConn) override;

  private:
    /// Process a validated binary frame whose header has already been
    /// parsed.  Dispatches the payload to the appropriate subsystem.
    void dispatchFrame(const WebSocketConnectionPtr &wsConn,
                       const std::vector<uint8_t> &payload);

    // Maximum accepted payload size in bytes (default 10 MiB).
    static constexpr uint32_t maxFrameBytes_{10U * 1024U * 1024U};
};
