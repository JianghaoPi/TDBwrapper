#ifndef _UTIL_H_
#define _UTIL_H_
#include "TDBAPIStruct.h"

#ifdef __cplusplus
extern "C" {
#endif

//请求代码表
void GetCodeTable(THANDLE hTdb, char* szMarket);

void GetKData(THANDLE hTdb, const char* szCode, const char* szMarket, int nBeginDate, int nEndDate, int nCycle, int nUserDef, int nCQFlag, int nAutoComplete);

//tick
void GetTickData(THANDLE hTdb, const char* szCode, const char* szMarket, int nDate);

//逐笔成交
void GetTransaction(THANDLE hTdb, const char* szCode, const char* szMarketKey, int nDate);

//逐笔委托
void GetOrder(THANDLE hTdb, const char* szCode, const char* szMarketKey, int nDate);

//委托队列
void GetOrderQueue(THANDLE hTdb, const char* szCode, const char* szMarketKey, int nDate);

//指标公式
void UseEZFFormula(THANDLE hTdb);

#ifdef __cplusplus
}
#endif

#endif