#pragma once

#include <stdint.h>


#define PACKET_HEADER_SIZE      (3)
#define PACKET_DATA_INDEX       (3)
#define PACKET_START_INDEX      (0)
#define PACKET_NUMBER_INDEX     (1)
#define PACKET_CNUMBER_INDEX    (2)
#define PACKET_TRAILER_SIZE     (3)
#define PACKET_OVERHEAD_SIZE    (PACKET_HEADER_SIZE + PACKET_TRAILER_SIZE - 1)

#define PACKET_SIZE_128B    (128)
#define PACKET_SIZE_1KB     (1024)

#define PACKET_BUFF_SIZE    (PACKET_HEADER_SIZE + PACKET_SIZE_1KB + PACKET_TRAILER_SIZE)

#define FILE_NAME_LENGTH    (64)
#define FILE_SIZE_LENGTH    (16)


typedef enum
{
  YMODEM_OK = 0,
  YMODEM_TIMEOUT,
  YMODEM_ERROR
}YMODEM_STATUS_EN;

typedef struct
{
  size_t(*read_block)(uint8_t *data, size_t size, uint32_t timeout);
  size_t(*write)(uint8_t *data, size_t size);
}YMODE_DRIVER_ST;

typedef struct
{
  char name[FILE_NAME_LENGTH];
  size_t size;
}FILE_INFO_ST;

typedef struct
{
  uint8_t data[PACKET_BUFF_SIZE];

  size_t(*read_block)(uint8_t *data, size_t size, uint32_t timeout);
  size_t(*write)(uint8_t *data, size_t size);
  YMODEM_STATUS_EN(*receive_data_handler)(uint8_t *data, size_t size);
}YMODEM_HANDLER;

void YMODEM_Init(YMODEM_HANDLER *ymodem, YMODE_DRIVER_ST *driver);
YMODEM_STATUS_EN SendFile(uint8_t *data, size_t size);
YMODEM_STATUS_EN ReceiveFile(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info);
