#ifndef _SOCKS5_H
#define _SOCKS5_H
//#pragma pack(1)

#define SOCKS5_VERSION 0x05
#define SOCKS5_CMD_CONNECT 0x01
#define SOCKS5_IPV4 0x01
#define SOCKS5_DOMAIN 0x03
#define SOCKS5_CMD_NOT_SUPPORTED 0x07
#define SOCKS5_ADDR_NOT_SUPPORTED 0x08
#define SOCKS5_CMD_CONNECT 0x01
#define SOCKS5_CMD_BIND 0x02
#define SOCKS5_CMD_UDPASSOC 0x03
#define SOCKS5_ATTYPE_IPV4 0x01
#define SOCKS5_ATTYPE_DOMAIN 0x03
#define SOCKS5_ATTYPE_IPV6 0x04
typedef struct _socks5_method_req_t
{
    uint8_t ver;
    uint8_t nmethods;
}socks5_method_req_t;

typedef struct _socks5_method_res_t
{
    uint8_t ver;
    uint8_t method;
}socks5_method_res_t;

typedef struct _socks5_request_t
{
    uint8_t ver;
    uint8_t cmd;
    uint8_t rsv;
    uint8_t atype;
}socks5_request_t;

typedef socks5_request_t socks5_response_t;

//#pragma pack()
#endif
