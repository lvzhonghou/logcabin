/* Copyright (c) 2012 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * \file
 * This file contains declarations useful to all LogCabin RPCs.
 */

#ifndef LOGCABIN_PROTOCOL_COMMON_H
#define LOGCABIN_PROTOCOL_COMMON_H

namespace LogCabin {
namespace Protocol {
namespace Common {

/**
 * The default TCP port on which LogCabin servers serve RPCs.
 */
enum { DEFAULT_PORT = 61023 };

/**
 * A MessageSocket::MessageID reserved for ping messages that are  used to
 * check the server's liveness. No real RPC will ever be assigned this ID.
 */
enum { PING_MESSAGE_ID = 0 };

/**
 * The maximum number of bytes per RPC request or response, including these
 * headers. This is set to slightly over 1 MB because the maximum size of log
 * entries is 1 MB.
 */
enum { MAX_MESSAGE_LENGTH = 1024 + 1024 * 1024 };

// This is not an enum class because it is usually used as a uint16_t;
// enum class is too strict about conversions.
namespace ServiceId {
enum {

    /**
     * The service that LogCabin client applications communicate with via the
     * client library.
     */
    CLIENT_SERVICE = 1,

    /**
     * The service that LogCabin servers use to communicate with each other.
     * The consensus protocol runs over this service.
     */
    RAFT_SERVICE = 2,

};
}

} // namespace LogCabin::Protocol::Common
} // namespace LogCabin::Protocol
} // namespace LogCabin

#endif // LOGCABIN_PROTOCOL_COMMON_H
