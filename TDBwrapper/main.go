package main

/*
#cgo LDFLAGS: -L ./ -lTDBAPI
#include <stdlib.h>
#include <string.h>
*/
import "C"
import (
	"fmt"
)

func main(){
	var hTdb C.THANDLE

	//设置服务器信息
	var settings C.struct_OPEN_SETTINGS
	settings.szIP = "10.100.4.172"
	settings.szPort = "10301"
	settings.szUser = "1"
	settings.szPassword = "1"
	settings.nRetryCount = 15
	settings.nRetryGap = 1
	settings.nTimeOutVal = 1

	//proxy
	var proxy_settings C.struct_TDB_PROXY_SETTINGS
	proxy_settings.nProxyType = C.enum_TDB_PROXY_HTTP11
	proxy_settings.szProxyHostIp = "10.100.3.42"
	proxy_settings.szProxyPort = "12345"
	proxy_settings.szProxyUser = "1"
	proxy_settings.szProxyPwd = "1"

	var LoginRes C.struct_TDBDefine_ResLogin
	//TDB_OpenProxy
	//hTdb = TDB_OpenProxy(&settings, &proxy_settings, &LoginRes)

	hTdb = C.TDB_Open(&settings, &LoginRes)

	if !hTdb {
		fmt.Println("连接失败！")
	}

	//TDB_GetCOdeInfo
	const pCode *C.struct_TDBDefine_Code = C.TDB_GetCodeInfo(hTdb, "000001.SZ", "SZ-2-0")
	fmt.Println("-------------收到代码信息----------------------------\n")
	fmt.Println("交易所代码 chWindCode:%s \n", pCode.chCode)
	fmt.Println("市场代码 chWindCode:%s \n", pCode.chMarket)
	fmt.Println("证券中文名称 chWindCode:%s \n", pCode.chCNName)
	fmt.Println("证券英文名称 chWindCode:%s \n", pCode.chENName)
	fmt.Println("证券类型 chWindCode:%d \n", pCode.nType)

	/*************************** 请求数据  ***********************************/
	//code table
	pCode *C.struct_TDBDefine_Code = nil
	var pCount int = 0
	C.TDB_GetCodeTable(hTdb, "SZ", &pCode, &pCount)
	if pCount && pCode {
		for i := 0; i<pCount; i++ {
			fmt.Println("-------------code table ----------------------------\n")
			fmt.Println("chWindCode:%s \n", pCode[i].chCode)
			fmt.Println("chWindCode:%s \n", pCode[i].chMarket)
			fmt.Println("chWindCode:%s \n", pCode[i].chCNName)
			fmt.Println("chWindCode:%s \n", pCode[i].chENName)
			fmt.Println("chWindCode:%d \n", pCode[i].nType)
		}
	}
	C.TDB_Free(pCode)

	//C.GetKData(hTdb,  "IF1602.CF", "CF-1-0", 20150910, 20150915, CYC_MINUTE, 0, 0, 0)//KLine for one minute
	C.GetKData(hTdb, "600715.SH", "SH-2-0", 20151126, 20151126, C.enum_CYC_MINUTE, 0, 0, 1);//autocomplete k-minute
	C.GetTickData(hTdb, "000001.sz", "SZ-2-0", 20150910);//tick
	C.GetTransaction(hTdb, "000001.sz", "SZ-2-0", 20150910);//Transaction
	C.GetOrder(hTdb, "000001.sz", "SZ-2-0", 20150910);//Order
	C.GetOrderQueue(hTdb, "000001.sz", "SZ-2-0", 20150910);//OrderQueue
	C.UseEZFFormula(hTdb);//test for formula

	fmt.Println("输入任意键结束程序")
	C.getchar()
	var nRet int = -1
	if hTdb {
		nRet = C.TDB_Close(hTdb)
	}

	return 0
}