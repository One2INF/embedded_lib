#ifdef __cplusplus
extern "C" {
#endif


#include "ymodem.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
#define MAX_ERRORS              ((uint32_t)5)


static YMODEM_STATUS_EN ymodem_WaitFileInfo(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info);
static YMODEM_STATUS_EN ymodem_ReceiveData(YMODEM_HANDLER *ymodem);
static YMODEM_STATUS_EN ymodem_EndReceive(YMODEM_HANDLER *ymodem);
static void ymodem_c(YMODEM_HANDLER *ymodem);
static void ymodem_ack(YMODEM_HANDLER *ymodem);
static void ymodem_ack_c(YMODEM_HANDLER *ymodem);
static void ymodem_nack(YMODEM_HANDLER *ymodem);
static void ymodem_abort(YMODEM_HANDLER *ymodem);
static uint8_t DataPacketCheck(uint8_t *packet, size_t packet_size);


void YMODEM_Init(YMODEM_HANDLER *ymodem, YMODE_DRIVER_ST *driver)
{
  ymodem->read_block = driver->read_block;
  ymodem->write = driver->write;
  ymodem->receive_data_handler = driver->receive_data_handler;
}

YMODEM_STATUS_EN SendFile(uint8_t *data, size_t size)
{
  return YMODEM_OK;
}

YMODEM_STATUS_EN ReceiveFile(YMODEM_HANDLER *ymodem, FILE_INFO_ST *file_info)
{
  ymodem_WaitFileInfo(ymodem, file_info);
  ymodem_ReceiveData(ymodem);
  ymodem_EndReceive(ymodem);

  return YMODEM_OK;
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

  if(SOH == ymodem->data[PACKET_START_INDEX])
  {
    char *p = (char*)ymodem->data + PACKET_DATA_INDEX;
    strncpy(file_info->name, p, FILE_NAME_LENGTH);
    file_info->size = strtoul(p + strlen(p) + 1, NULL, 10);
    printf("filename: %s, file size: %zd\r\n", file_info->name, file_info->size);

    ymodem_ack_c(ymodem);

    return YMODEM_OK;
  }

  return YMODEM_ERROR;
}

static YMODEM_STATUS_EN ymodem_ReceiveData(YMODEM_HANDLER *ymodem)
{
  uint32_t packet_idx = 0;
  size_t packet_size;

  while(ymodem->read_block(ymodem->data, PACKET_BUFF_SIZE, DOWNLOAD_TIMEOUT))
  {
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
      ymodem->receive_data_handler(ymodem->data, packet_size);
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

static void ymodem_nack(YMODEM_HANDLER *ymodem)
{
  ymodem_putc(ymodem, NAK);
}

static void ymodem_abort(YMODEM_HANDLER *ymodem)
{
  ymodem_putc(ymodem, CA);
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

  uint32_t crc = packet[packet_size + PACKET_DATA_INDEX] << 8;
  crc += packet[packet_size + PACKET_DATA_INDEX + 1];
  if(crc != Cal_CRC16(packet + PACKET_DATA_INDEX, packet_size))
    return 0;

  return 1;
}

#ifdef __cplusplus
}
#endif
