#include "util.h"
#include <stdio.h>
#include "TDBAPI.h"
#include <iostream>
#include <string.h>
#include <algorithm>
#include <assert.h>

using namespace std;

#ifdef NDEBUG

#define AssertEx(expr) expr;

#else
#define AssertEx(expr) {int n = (int)(expr); assert(n);}
#endif

#define ELEMENT_COUNT(arr) (sizeof(arr)/sizeof(*arr))

long GetTickCount()
{
	time_t now=time(NULL);
	return (long)now;
}

std::string int2str(int n)
{
	char szBuf[32];
	snprintf(szBuf, sizeof(szBuf)/sizeof(szBuf[0]), "%d", n);
	return std::string(szBuf);
}

std::string array2str(const int* arr, int len)
{
	std::string str;
	for (int i=0; i<len; i++)
	{
		if (i == len-1)
		{
			str += int2str(arr[i]) + " ";
		}
		else
		{
			str += int2str(arr[i]) + ",";
		}
	}
	return str;
}

//请求代码表
void GetCodeTable(THANDLE hTdb, char* szMarket)
{
	TDBDefine_Code* pCodetable = NULL;
	int pCount;
	bool outPutTable = true;
	int ret = TDB_GetCodeTable(hTdb, szMarket, &pCodetable, &pCount);

	if (ret == TDB_NO_DATA)
	{
		printf("无代码表！");
		return;
	}
	printf("---------------------------Code Table--------------------\n");
	printf("收到代码表项数：%d，\n\n",pCount);
	//输出
	if(outPutTable)
	{
		for (int i=0;i<pCount;i++)
		{
			printf("交易所代码 chWindCode:%s \n", pCodetable[i].chCode);
			printf("市场代码 chWindCode:%s \n", pCodetable[i].chMarket);
			printf("证券中文名称 chWindCode:%s \n", pCodetable[i].chCNName);
			printf("证券英文名称 chWindCode:%s \n", pCodetable[i].chENName);
			printf("证券类型 chWindCode:%d \n", pCodetable[i].nType);
			printf("----------------------------------------\n");
		}
	}
	//释放
	TDB_Free(pCodetable);
}

void GetKData(THANDLE hTdb, const char* szCode, const char* szMarket, int nBeginDate, int nEndDate, int nCycle, int nUserDef, int nCQFlag, int nAutoComplete)
{
	//请求K线
	TDBDefine_ReqKLine* req = new TDBDefine_ReqKLine;
	strncpy(req->chCode, szCode, ELEMENT_COUNT(req->chCode));
	strncpy(req->chMarketKey, szMarket, ELEMENT_COUNT(req->chMarketKey));

	req->nCQFlag = (REFILLFLAG)nCQFlag;//除权标志，由用户定义
	req->nBeginDate = nBeginDate;//开始日期
	req->nEndDate = nEndDate;//结束日期
	req->nBeginTime = 0;//开始时间
	req->nEndTime = 0;//结束时间

	req->nCycType = (CYCTYPE)nCycle;
	req->nCycDef = 0;
	req->nAutoComplete = nAutoComplete;

	//返回结构体指针
	TDBDefine_KLine* kLine = NULL;
	//返回数
	int pCount;
	//API请求K线
	TDB_GetKLine(hTdb,req,&kLine,&pCount);
	delete req;
	req = NULL;

	printf("---------------------------K Data--------------------\n");
	printf("数据条数：%d,打印 1/100 条\n\n",pCount);
	for(int i=0;i<pCount;)
	{
		printf("WindCode:%s\n Code:%s\n Date:%d\n Time:%d\n Open:%d\n High:%d\n Low:%d\n Close:%d\n Volume:%lld\n Turover:%lld\n MatchItem:%d\n Interest:%d\n",
			kLine[i].chWindCode,kLine[i].chCode,kLine[i].nDate,kLine[i].nTime,kLine[i].nOpen,kLine[i].nHigh,kLine[i].nLow,kLine[i].nClose,
			kLine[i].iVolume,kLine[i].iTurover,kLine[i].nMatchItems,kLine[i].nInterest);
		i +=100;
	}
	//释放
	TDB_Free(kLine);
}

//tick
void GetTickData(THANDLE hTdb, const char* szCode, const char* szMarket, int nDate)
{
	//请求信息
	TDBDefine_ReqTick req = {0};
	strncpy(req.chCode, szCode, sizeof(req.chCode)); //代码写成你想获取的股票代码
	strncpy(req.chMarketKey, szMarket, sizeof(req.chMarketKey));
	req.nDate = nDate;
	req.nBeginTime = 0;
	req.nEndTime = 0;

	TDBDefine_Tick *pTick = NULL;
	int pCount;
	int ret = TDB_GetTick(hTdb,&req,&pTick, &pCount);

	printf("---------------------------------------Tick Data------------------------------------------\n");
	printf("共收到 %d 条Tick数据， 打印 1/100 条：\n", pCount);

	for(int i=0; i<pCount;)
	{
		TDBDefine_Tick& pTickCopy = pTick[i];
		printf("万得代码 chWindCode:%s \n", pTickCopy.chWindCode);
		printf("日期 nDate:%d \n", pTickCopy.nDate);
		printf("时间 nTime:%d \n", pTickCopy.nTime);

		printf("成交价 nPrice:%d \n", pTickCopy.nPrice);
		printf("成交量 iVolume:%lld \n", pTickCopy.iVolume);
		printf("成交额(元) iTurover:%lld \n", pTickCopy.iTurover);
		printf("成交笔数 nMatchItems:%d \n", pTickCopy.nMatchItems);
		printf(" nInterest:%d \n", pTickCopy.nInterest);

		printf("成交标志: chTradeFlag:%c \n", pTickCopy.chTradeFlag);
		printf("BS标志: chBSFlag:%c \n", pTickCopy.chBSFlag);
		printf("当日成交量: iAccVolume:%lld \n", pTickCopy.iAccVolume);
		printf("当日成交额: iAccTurover:%lld \n", pTickCopy.iAccTurover);

		printf("最高 nHigh:%d \n", pTickCopy.nHigh);
		printf("最低 nLow:%d \n", pTickCopy.nLow);
		printf("开盘 nOpen:%d \n", pTickCopy.nOpen);
		printf("前收盘 nPreClose:%d \n", pTickCopy.nPreClose);

		//买卖盘字段
		std::string strOut= array2str(pTickCopy.nAskPrice,ELEMENT_COUNT(pTickCopy.nAskPrice));
		printf("叫卖价 nAskPrice:%s \n", strOut.c_str());
		strOut= array2str((const int*)pTickCopy.nAskVolume,ELEMENT_COUNT(pTickCopy.nAskVolume));
		printf("叫卖量 nAskVolume:%s \n", strOut.c_str());
		strOut= array2str(pTickCopy.nBidPrice,ELEMENT_COUNT(pTickCopy.nBidPrice));
		printf("叫买价 nBidPrice:%s \n", strOut.c_str());
		strOut= array2str((const int*)pTickCopy.nBidVolume,ELEMENT_COUNT(pTickCopy.nBidVolume));
		printf("叫买量 nBidVolume:%s \n", strOut.c_str());
		printf("加权平均叫卖价 nAskAvPrice:%d \n", pTickCopy.nAskAvPrice);
		printf("加权平均叫买价 nBidAvPrice:%d \n", pTickCopy.nBidAvPrice);
		printf("叫卖总量 iTotalAskVolume:%lld \n", pTickCopy.iTotalAskVolume);
		printf("叫买总量 iTotalBidVolume:%lld \n", pTickCopy.iTotalBidVolume);
#if 0
		//期货字段
		printf("结算价 nSettle:%d \n", pTickCopy.nSettle);
		printf("持仓量 nPosition:%d \n", pTickCopy.nPosition);
		printf("虚实度 nCurDelta:%d \n", pTickCopy.nCurDelta);
		printf("昨结算 nPreSettle:%d \n", pTickCopy.nPreSettle);
		printf("昨持仓 nPrePosition:%d \n", pTickCopy.nPrePosition);

		//指数
		printf("不加权指数 nIndex:%d \n", pTickCopy.nIndex);
		printf("品种总数 nStocks:%d \n", pTickCopy.nStocks);
		printf("上涨品种数 nUps:%d \n", pTickCopy.nUps);
		printf("下跌品种数 nDowns:%d \n", pTickCopy.nDowns);
		printf("持平品种数 nHoldLines:%d \n", pTickCopy.nHoldLines);
#endif
		printf("-------------------------------\n");
		i += 1000;
	}
	//释放
	TDB_Free(pTick);
}

//逐笔成交
void GetTransaction(THANDLE hTdb, const char* szCode, const char* szMarketKey, int nDate)
{
	//请求
	TDBDefine_ReqTransaction req = {0};
	strncpy(req.chCode, szCode, sizeof(req.chCode)); //代码写成你想获取的股票代码
	strncpy(req.chMarketKey, szMarketKey, sizeof(req.chMarketKey));
	req.nDate = nDate;
	req.nBeginTime = 0;
	req.nEndTime = 0;

	TDBDefine_Transaction *pTransaction = NULL;
	int pCount;
	int ret = TDB_GetTransaction(hTdb,&req, &pTransaction, &pCount);

	printf("---------------------------------------Transaction Data------------------------------------------\n");
	printf("收到 %d 条逐笔成交消息，打印 1/10000 条\n", pCount);

	for (int i=0; i<pCount; )
	{
		const TDBDefine_Transaction& trans = pTransaction[i];
		printf("成交时间(Date): %d \n", trans.nDate);
		printf("成交时间: %d \n", trans.nTime);
		printf("成交代码: %c \n", trans.chFunctionCode);
		printf("委托类别: %c \n", trans.chOrderKind);
		printf("BS标志: %c \n", trans.chBSFlag);
		printf("成交价格: %d \n", trans.nTradePrice);
		printf("成交数量: %d \n", trans.nTradeVolume);
		printf("叫卖序号: %d \n", trans.nAskOrder);
		printf("叫买序号: %d \n", trans.nBidOrder);
		printf("------------------------------------------------------\n");
#if 0
		printf("成交编号: %d \n", trans.nBidOrder);
#endif
		i += 10000;
	}
	//释放
	TDB_Free(pTransaction);
}

//逐笔委托
void GetOrder(THANDLE hTdb, const char* szCode, const char* szMarketKey, int nDate)
{
	//请求
	TDBDefine_ReqOrder req = {0};
	strncpy(req.chCode, szCode, sizeof(req.chCode)); //代码写成你想获取的股票代码
	strncpy(req.chMarketKey, szMarketKey, sizeof(req.chMarketKey));
	req.nDate = nDate;
	req.nBeginTime = 0;
	req.nEndTime = 0;

	TDBDefine_Order *pOrder = NULL;
	int pCount;
	int ret = TDB_GetOrder(hTdb,&req, &pOrder, &pCount);

	printf("---------------------Order Data----------------------\n");
	printf("收到 %d 条逐笔委托消息，打印 1/10000 条\n", pCount);
	for (int i=0; i<pCount; )
	{
		const TDBDefine_Order& order = pOrder[i];
		printf("订单时间(Date): %d \n", order.nDate);
		printf("委托时间(HHMMSSmmm): %d \n", order.nTime);
		printf("委托编号: %d \n", order.nOrder);
		printf("委托类别: %c \n", order.chOrderKind);
		printf("委托代码: %c \n", order.chFunctionCode);
		printf("委托价格: %d \n", order.nOrderPrice);
		printf("委托数量: %d \n", order.nOrderVolume);
		printf("-------------------------------\n");

		i += 10000;
	}
	//释放
	TDB_Free(pOrder);
}

//委托队列
void GetOrderQueue(THANDLE hTdb, const char* szCode, const char* szMarketKey, int nDate)
{
	//请求
	TDBDefine_ReqOrderQueue req = {0};
	strncpy(req.chCode, szCode, sizeof(req.chCode)); //代码写成你想获取的股票代码
	strncpy(req.chMarketKey, szMarketKey, sizeof(req.chMarketKey));
	req.nDate = nDate;
	req.nBeginTime = 0;
	req.nEndTime = 0;

	TDBDefine_OrderQueue *pOrderQueue = NULL;
	int pCount;
	TDB_GetOrderQueue(hTdb,&req, &pOrderQueue, &pCount);

	printf("-------------------OrderQueue Data-------------\n");
	printf("收到 %d 条委托队列消息，打印 1/1000 条\n", pCount);

	for (int i=0; i<pCount; i++)
	{
		const TDBDefine_OrderQueue& que = pOrderQueue[i];
		printf("订单时间(Date): %d \n", que.nDate);
		printf("订单时间(HHMMSS): %d \n", que.nTime);
		printf("买卖方向('B':Bid 'A':Ask): %c \n", que.nSide);
		printf("成交价格: %d \n", que.nPrice);
		printf("订单数量: %d \n", que.nOrderItems);
		printf("明细个数: %d \n", que.nABItems);
		printf("订单明细: %s \n", array2str(que.nABVolume, que.nABItems).c_str());
		printf("-------------------------------\n");
		i += 1000;
	}
	//释放
	TDB_Free(pOrderQueue);
}

//指标公式
void UseEZFFormula(THANDLE hTdb)
{
	//公式的编写，请参考<<TRANSEND-TS-M0001 易编公式函数表V1(2).0-20110822.pdf>>;
	std::string strName = "KDJ";
	std::string strContent = "INPUT:N(9), M1(3,1,100,2), M2(3);"
		"RSV:=(CLOSE-LLV(LOW,N))/(HHV(HIGH,N)-LLV(LOW,N))*100;"
		"K:SMA(RSV,M1,1);"
		"D:SMA(K,M2,1);"
		"J:3*K-2*D;";

	//添加公式到服务器并编译，若不过，会有错误返回
	TDBDefine_AddFormulaRes* addRes = new TDBDefine_AddFormulaRes;
	int nErr = TDB_AddFormula(hTdb, strName.c_str(), strContent.c_str(),addRes);
	printf("Add Formula Result:%s",addRes->chInfo);

	//查询服务器上的公式，能看到我们刚才上传的"KDJ"
	TDBDefine_FormulaItem* pEZFItem = NULL;
	int nItems = 0;
	//名字为空表示查询服务器上所有的公式
	nErr = TDB_GetFormula(hTdb, NULL, &pEZFItem, &nItems);

	for (int i=0; i<nItems; i++)
	{
		std::string strNameInner(pEZFItem[i].chFormulaName, 0, sizeof(pEZFItem[i].chFormulaName));
		std::string strParam(pEZFItem[i].chParam, 0, sizeof(pEZFItem[i].chParam));
		printf("公式名称：%s, 参数:%s \n", strNameInner.c_str(), strParam.c_str());
	}

	struct EZFCycDefine
	{
		char chName[8];
		int  nCyc;
		int  nCyc1;
	}
	EZFCyc[5]={
		{"日线", 2, 0},
		{"30分", 0, 30},
		{"5分钟", 0, 5},
		{"1分钟", 0, 1},
		{"15秒", 11, 15}};

		//获取公式的计算结果
		TDBDefine_ReqCalcFormula reqCalc = {0};
		strncpy(reqCalc.chFormulaName, "KDJ", sizeof(reqCalc.chFormulaName));
		strncpy(reqCalc.chParam, "N=9,M1=3,M2=3", sizeof(reqCalc.chParam));
		strncpy(reqCalc.chCode, "000001.SZ", sizeof(reqCalc.chCode));
		strncpy(reqCalc.chMarketKey, "SZ-2-0", sizeof(reqCalc.chMarketKey));
		reqCalc.nCycType = (CYCTYPE)(EZFCyc[0].nCyc); //0表示日线
		reqCalc.nCycDef = EZFCyc[0].nCyc1;
		reqCalc.nCQFlag = (REFILLFLAG)0;		  //除权标志
		reqCalc.nCalcMaxItems = 4000; //计算的最大数据量
		reqCalc.nResultMaxItems = 100;	//传送的结果的最大数据量

		TDBDefine_CalcFormulaRes* pResult = new TDBDefine_CalcFormulaRes;
		nErr = TDB_CalcFormula(hTdb, &reqCalc, pResult);
		//判断错误代码

		printf("计算结果有: %d 条:\n", pResult->nRecordCount);
		char szLineBuf[1024] = {0};
		//输出字段名
		for (int j=0; j<pResult->nFieldCount;j++)
		{
			std::cout << pResult->chFieldName[j] << "  ";
		}
		std::cout << endl << endl;
		//输出数据
		for (int i=0; i<pResult->nRecordCount; i++)
		{
			for (int j=0; j<pResult->nFieldCount;j++)
			{
				std::cout << (pResult->dataFileds)[j][i] << "  ";
			}
			std::cout << endl;
		}

		//删除之前上传的公式指标
		TDBDefine_DelFormulaRes pDel = {0};
		nErr = TDB_DeleteFormula(hTdb, "KDJ", &pDel);
		printf("删除指标信息:%s", pDel.chInfo);
		//释放内存
		delete pEZFItem;
		TDB_ReleaseCalcFormula(pResult);
}
