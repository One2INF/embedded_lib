#ifdef __cplusplus
extern "C" {
#endif


#include "ymodem.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#if defined USE_TINYPRINTF

#include "tinyprintf.h"

#define printf tinyprintf
#define snprintf tinysnprintf

#endif


#define PACKET_DATA_INDEX       (3)
#define PACKET_START_INDEX      (0)
#define PACKET_NUMBER_INDEX     (1)
#define PACKET_CNUMBER_INDEX    (2)

#define SOH                     ((uint8_t)0x01)  /* start of 128-byte data packet */
#define STX                     ((uint8_t)0x02)  /* start of 1024-byte data packet */
#define EOT                     ((uint8_t)0x04)  /* end of transmission */
#define ACK                     ((uint8_t)0x06)  /* acknowledge */
#define NAK                     ((uint8_t)0x15)  /* negative acknowledge */
#define CA                      ((uint32_t)0x18) /* two of these in succession aborts transfer */
#define CRC16                   ((uint8_t)0x43)  /* 'C' == 0x43, request 16-bit CRC */
#define NEGATIVE_BYTE           ((uint8_t)0xFF)

#define ABORT1                  ((uint8_t)0x41)  /* 'A' == 0x41, abort by user */
#define ABORT2                  ((uint8_t)0x61)  /* 'a' == 0x61, abort by user */

#define DOWNLOAD_TIMEOUT        ((uint32_t)1000) /* One second retry delay */
#define MAX_ERRORS              ((uint32_t)3600)

static YMODEM_STATUS_EN ymodem_WaitReceiverReady(YMODEM_HANDLER *ymodem);
static YMODEM_STATUS_EN ymodem_SendFileInfo(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info);
static YMODEM_STATUS_EN ymodem_SendData(YMODEM_HANDLER *ymodem, uint8_t *data, size_t size);
static YMODEM_STATUS_EN ymodem_EndSend(YMODEM_HANDLER *ymodem);

static YMODEM_STATUS_EN ymodem_WaitFileInfo(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info);
static YMODEM_STATUS_EN ymodem_ReceiveData(YMODEM_HANDLER *ymodem);
static YMODEM_STATUS_EN ymodem_EndReceive(YMODEM_HANDLER *ymodem);

static void ymodem_c(YMODEM_HANDLER *ymodem);
static void ymodem_ack(YMODEM_HANDLER *ymodem);
static void ymodem_ack_c(YMODEM_HANDLER *ymodem);
static void ymodem_nack(YMODEM_HANDLER *ymodem);
static void ymodem_abort(YMODEM_HANDLER *ymodem);
static void ymodem_eot(YMODEM_HANDLER *ymodem);

static uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size);
static uint8_t DataPacketCheck(uint8_t *packet, size_t packet_size);


void YMODEM_Init(YMODEM_HANDLER *ymodem, YMODE_DRIVER_ST *driver)
{
  ymodem->read_block = driver->read_block;
  ymodem->write = driver->write;
  ymodem->fileinfo_handler = driver->fileinfo_handler;
  ymodem->receive_data_handler = driver->receive_data_handler;
}

YMODEM_STATUS_EN YMODEM_SendFile(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info, uint8_t *data, size_t size)
{
  ymodem_WaitReceiverReady(ymodem);
  ymodem_SendFileInfo(ymodem, file_info);
  ymodem_SendData(ymodem, data, size);
  ymodem_EndSend(ymodem);

  return YMODEM_OK;
}

static YMODEM_STATUS_EN ymodem_WaitReceiverReady(YMODEM_HANDLER *ymodem)
{
  while(!ymodem->read_block(ymodem->data, PACKET_BUFF_SIZE, DOWNLOAD_TIMEOUT));

  if('C' == ymodem->data[0])
    return YMODEM_OK;

  return YMODEM_ERROR;
}

static YMODEM_STATUS_EN ymodem_SendFileInfo(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info)
{
  memset(ymodem->data, 0x00, PACKET_OVERHEAD_SIZE + PACKET_SIZE_128B);

  ymodem->data[PACKET_START_INDEX] = SOH;
  ymodem->data[PACKET_NUMBER_INDEX] = 0x00;
  ymodem->data[PACKET_CNUMBER_INDEX] = 0xFF;

  strncpy((char*)ymodem->data + PACKET_DATA_INDEX, file_info->name, FILE_NAME_LENGTH);
  ymodem->data[PACKET_DATA_INDEX + strlen(file_info->name)] = '\0';
  snprintf((char*)ymodem->data + PACKET_DATA_INDEX + strlen(file_info->name) + 1, 128, "%zd", file_info->size);

  uint16_t crc16 = Cal_CRC16(ymodem->data + PACKET_HEADER_SIZE, PACKET_SIZE_128B);
  ymodem->data[PACKET_DATA_INDEX + PACKET_SIZE_128B] = crc16 >> 8;
  ymodem->data[PACKET_DATA_INDEX + PACKET_SIZE_128B + 1] = crc16 & 0x00FF;

  while(1)
  {
    ymodem->write(ymodem->data, PACKET_OVERHEAD_SIZE + PACKET_SIZE_128B);
    uint8_t ch[2];
    if(ymodem->read_block(ch, 2, DOWNLOAD_TIMEOUT))
    {
      if(ACK == ch[0] && 'C' == ch[1])
      {
        break;
      }
    }
  }

  return YMODEM_OK;
}

static YMODEM_STATUS_EN ymodem_SendData(YMODEM_HANDLER *ymodem, uint8_t *data, size_t size)
{
  uint32_t idx = 0;
  size_t BytesRemained = size;
  size_t PacketDataSize = 0;
  ymodem->data[PACKET_START_INDEX] = SOH;

  do
  {
    if(BytesRemained >= PACKET_SIZE_128B)
    {
      PacketDataSize = PACKET_SIZE_128B;
      BytesRemained -= PACKET_SIZE_128B;
    }
    else
    {
      PacketDataSize = BytesRemained;
      BytesRemained = 0;
    }
    ++idx;
    printf("packet_size: %zd, BytesRemained: %zd\r\n", PacketDataSize, BytesRemained);
    ymodem->data[PACKET_NUMBER_INDEX] = idx;
    ymodem->data[PACKET_CNUMBER_INDEX] = ~idx;
    memset(ymodem->data + PACKET_HEADER_SIZE, 0, PACKET_SIZE_128B);
    memcpy(ymodem->data + PACKET_HEADER_SIZE, data + PACKET_SIZE_128B * (idx - 1), PacketDataSize);

    uint16_t crc16 = Cal_CRC16(ymodem->data + PACKET_HEADER_SIZE, PACKET_SIZE_128B);
    ymodem->data[PACKET_DATA_INDEX + PACKET_SIZE_128B] = crc16 >> 8;
    ymodem->data[PACKET_DATA_INDEX + PACKET_SIZE_128B + 1] = crc16 & 0x00FF;

    while(1)
    {
      ymodem->write(ymodem->data, PACKET_OVERHEAD_SIZE + PACKET_SIZE_128B);

      uint8_t ch;
      if(ymodem->read_block(&ch, 1, DOWNLOAD_TIMEOUT))
      {
        if(ACK == ch)
        {
          break;
        }
      }
    }
  }
  while(BytesRemained);

  return YMODEM_OK;
}

static YMODEM_STATUS_EN ymodem_EndSend(YMODEM_HANDLER *ymodem)
{
  while(1)
  {
    ymodem_eot(ymodem);

    uint8_t ch[2];
    if(ymodem->read_block(ch, 2, DOWNLOAD_TIMEOUT))
    {
      if(ACK == ch[0] && 'C' == ch[1])
      {
        break;
      }
    }
  }

  ymodem->data[PACKET_NUMBER_INDEX] = 0x00;
  ymodem->data[PACKET_CNUMBER_INDEX] = 0xFF;
  memset(ymodem->data + PACKET_HEADER_SIZE, 0, PACKET_SIZE_128B);

  uint16_t crc16 = Cal_CRC16(ymodem->data + PACKET_HEADER_SIZE, PACKET_SIZE_128B);
  ymodem->data[PACKET_DATA_INDEX + PACKET_SIZE_128B] = crc16 >> 8;
  ymodem->data[PACKET_DATA_INDEX + PACKET_SIZE_128B + 1] = crc16 & 0x00FF;

  while(1)
  {
    ymodem->write(ymodem->data, PACKET_OVERHEAD_SIZE + PACKET_SIZE_128B);

    uint8_t ch[1];
    if(ymodem->read_block(ch, 2, DOWNLOAD_TIMEOUT))
    {
      if(ACK == ch[0])
      {
        break;
      }
    }
  }

  return YMODEM_OK;
}

YMODEM_STATUS_EN YMODEM_ReceiveFile(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info)
{
  YMODEM_STATUS_EN Ret;
  Ret = ymodem_WaitFileInfo(ymodem, file_info);
  if(YMODEM_ERROR == Ret)
    goto error;
  else if(YMODEM_TIMEOUT == Ret)
    return Ret;

  Ret = ymodem_ReceiveData(ymodem);
  if(YMODEM_ERROR == Ret)
    goto error;

  Ret = ymodem_EndReceive(ymodem);
  if(YMODEM_ERROR == Ret)
    goto error;

  return Ret;

error:
  ymodem_abort(ymodem);
  return Ret;

}

static YMODEM_STATUS_EN ymodem_WaitFileInfo(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info)
{
  uint_fast8_t ErrorCount = 0;

  ymodem_c(ymodem);
  while(!ymodem->read_block(ymodem->data, PACKET_BUFF_SIZE, DOWNLOAD_TIMEOUT))
  {
    if(ErrorCount++ > MAX_ERRORS)
      return YMODEM_TIMEOUT;

    ymodem_c(ymodem);
  }

  printf("%02X %02X\r\n", ymodem->data[0], ymodem->data[1]);

  if(SOH == ymodem->data[PACKET_START_INDEX])
  {
    char *p = (char*)ymodem->data + PACKET_DATA_INDEX;
    strncpy(file_info->name, p, FILE_NAME_LENGTH);
    file_info->size = strtoul(p + strlen(p) + 1, NULL, 10);
    printf("filename: %s, file size: %zd\r\n", file_info->name, file_info->size);

    if(ymodem->fileinfo_handler)
      ymodem->fileinfo_handler(file_info);

    ymodem_ack_c(ymodem);

    return YMODEM_OK;
  }

  return YMODEM_ERROR;
}

static YMODEM_STATUS_EN ymodem_ReceiveData(YMODEM_HANDLER *ymodem)
{
  uint32_t packet_idx = 0;
  size_t packet_size;
  size_t packet_offset = 0;

  while(ymodem->read_block(ymodem->data, PACKET_BUFF_SIZE, DOWNLOAD_TIMEOUT))
  {
    printf("%02X %02X\r\n", ymodem->data[0], ymodem->data[1]);

    if(SOH == ymodem->data[PACKET_START_INDEX])
    {
      packet_size = PACKET_SIZE_128B;
    }
    else if(STX == ymodem->data[PACKET_START_INDEX])
    {
      packet_size = PACKET_SIZE_1KB;
    }
    else if(EOT == ymodem->data[PACKET_START_INDEX])
    {
      ymodem_ack_c(ymodem);
      return YMODEM_OK;
    }
    else
    {
      return YMODEM_ERROR;
    }

    if(++packet_idx != ymodem->data[PACKET_NUMBER_INDEX])
    {
      return YMODEM_ERROR;
    }

    if(DataPacketCheck(ymodem->data, packet_size))
    {
      if(ymodem->receive_data_handler)
          ymodem->receive_data_handler(packet_offset, ymodem->data + PACKET_DATA_INDEX, packet_size);
      printf("after receive_data_handler\r\n");
      packet_offset += packet_size;
    }
    else
    {
      return YMODEM_ERROR;
    }
    ymodem_ack(ymodem);
  }

  return YMODEM_ERROR;
}

static YMODEM_STATUS_EN ymodem_EndReceive(YMODEM_HANDLER *ymodem)
{
  if(ymodem->read_block(ymodem->data, PACKET_BUFF_SIZE, DOWNLOAD_TIMEOUT))
  {
    ymodem_ack(ymodem);
    return YMODEM_OK;
  }

  return YMODEM_ERROR;
}

static void ymodem_putc(YMODEM_HANDLER *ymodem, char ch)
{
  ymodem->write((uint8_t*)&ch, 1);
}

static void ymodem_c(YMODEM_HANDLER *ymodem)
{
  ymodem_putc(ymodem, 'C');
}

static void ymodem_ack(YMODEM_HANDLER *ymodem)
{
  ymodem_putc(ymodem, ACK);
}

static void ymodem_ack_c(YMODEM_HANDLER *ymodem)
{
  ymodem_putc(ymodem, ACK);
  ymodem_putc(ymodem, 'C');
}

static void ymodem_eot(YMODEM_HANDLER *ymodem)
{
  ymodem_putc(ymodem, EOT);
}

static void ymodem_nack(YMODEM_HANDLER *ymodem)
{
  ymodem_putc(ymodem, NAK);
}

static void ymodem_abort(YMODEM_HANDLER *ymodem)
{
  uint8_t i = 8;
  while(i--)
    ymodem_putc(ymodem, CA);
}

static uint16_t UpdateCRC16(uint16_t crc_in, uint8_t byte)
{
  uint32_t crc = crc_in;
  uint32_t in = byte | 0x100;

  do
  {
    crc <<= 1;
    in <<= 1;
    if(in & 0x100)
      ++crc;
    if(crc & 0x10000)
      crc ^= 0x1021;
  }
  while(!(in & 0x10000));

  return crc & 0xffffu;
}

static uint16_t Cal_CRC16(const uint8_t* p_data, uint32_t size)
{
  uint32_t crc = 0;
  const uint8_t* dataEnd = p_data+size;

  while(p_data < dataEnd)
    crc = UpdateCRC16(crc, *p_data++);

  crc = UpdateCRC16(crc, 0);
  crc = UpdateCRC16(crc, 0);

  return crc&0xffffu;
}

static uint8_t DataPacketCheck(uint8_t *packet, size_t packet_size)
{
  if(!((SOH == packet[PACKET_START_INDEX]) || (STX == packet[PACKET_START_INDEX])))
    return 0;

  if(packet[PACKET_NUMBER_INDEX] != (packet[PACKET_CNUMBER_INDEX] ^ 0xFF))
    return 0;

  uint16_t crc = packet[packet_size + PACKET_DATA_INDEX] << 8;
  crc += packet[packet_size + PACKET_DATA_INDEX + 1];
  if(crc != Cal_CRC16(packet + PACKET_DATA_INDEX, packet_size))
    return 0;

  return 1;
}

#ifdef __cplusplus
}
#endif
