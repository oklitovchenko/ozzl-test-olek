#ifndef OZZLPROTOCOMMON_H
#define OZZLPROTOCOMMON_H

/*
 *  All protocols musts support (for amd64):
 *
 *  0) first two bytes:
 *     0xAA 0xFF
 *
 *  1) for Request:
 *     0xAA 0xFF 0x00 0x00  0x00 0x00 0x00 0x00  0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
 *    [ protocol  version ][     reserved      ][        request (double value)         ]
 *
 *  2) for Response:
 *     0xAA 0xFF 0x00 0x00  0x00 0x00 0x00 0x00  0x00 0x00 0x00 0x00  0x00 ...............
 *    [ protocol  version ][       error       ][     body size     ][ body as byte stream ->
 *                  ^              ^                                    ^
 *                  |              |                                    |
 *                  |              |                             (LOG INFO) if (Error code) != NULL
 *                  |              |
 *                 NULL if (Error code) == ERR_PROTO_VER_MISMATCH
 */

namespace Protocol
{
    enum Versions : unsigned int {
        VER_UNDEFINED = 0xAAFF0000,
        VER_01,
        VER_02,
    };
    static const Versions AllVers[] = { VER_01, VER_02 };

    enum Errors : int {
        ERR_NO_ERRORS,
        ERR_PROTO_VER_MISMATCH = 0x65,
    };

    struct RequestHeader {
        Versions protocolVersion = VER_UNDEFINED;
        int reserved = 0;
    };

    struct Request {
        RequestHeader header;
        double request = 0;
    };

    struct ResponseHeader {
        Versions protocolVersion;
        int error = ERR_NO_ERRORS;
        unsigned int bodySize = 0;
    };

    // Response realized as a stream
}

#endif // OZZLPROTOCOMMON_H
