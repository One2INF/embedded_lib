#pragma once


typedef enum
{
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARNING,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_NUM
}LOG_LEVEL_EN;


#define LOG_PRINTF(fmt, ...)    LOG(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)

/* log types */
#define LOG_DEBUG(fmt, ...)      LOG(LOG_LEVEL_DEBUG,   "[D] %s(%u)@%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)       LOG(LOG_LEVEL_INFO,    "[I] %s(%u)@%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...)    LOG(LOG_LEVEL_WARNING, "[W] %s(%u)@%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)      LOG(LOG_LEVEL_ERROR,   "[E] %s(%u)@%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_CRITICAL(fmt, ...)   LOG(LOG_LEVEL_ERROR,   "[C] %s(%u)@%s: "fmt"\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

/* check condition then print log */
#define LOG_ASSERT_DEBGU(condition, expression, fmt, ...)      do{if(!(condition)){LOG_DEBUG(fmt, ##__VA_ARGS__); expression;}}while(0)
#define LOG_ASSERT_INFO(condition, expression, fmt, ...)       do{if(!(condition)){LOG_INFO(fmt, ##__VA_ARGS__); expression;}}while(0)
#define LOG_ASSERT_WARNING(condition, expression, fmt, ...)    do{if(!(condition)){LOG_WARNING(fmt, ##__VA_ARGS__); expression;}}while(0)
#define LOG_ASSERT_ERROR(condition, expression, fmt, ...)      do{if(!(condition)){LOG_ERROR(fmt, ##__VA_ARGS__); expression;}}while(0)
#define LOG_ASSERT(condition, fmt, ...)                        do{if(!(condition)){LOG_CRITICAL(fmt, ##__VA_ARGS__); while(1);}}while(0)


void LOG_setLogLevel(LOG_LEVEL_EN enLogLevel);
void LOG(LOG_LEVEL_EN enLogLevel, const char* fmt, ...);

