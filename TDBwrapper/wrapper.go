package main

/*
#cgo LDFLAGS: -lutil
#cgo LDFLAGS: -lTDBAPI
#include "util.h"
#include "TDBAPI.h"
 */
import "C"
import "fmt"

func main()  {
	fmt.Println("hello")
}

func WrapperGetCodeTable(hTdb C.THANDLE, szMarket string){
	return C.GetCodeTable(hTdb, C.CString(szMarket))
}

func WrapperGetKData(hTdb C.THANDLE, szCode string, szMarket string, nBeginDate int, nEndDate int, nCycle int, nUserDef int, nCQFlag int, nAutoComplete int)  {
	return C.GetKData(hTdb, C.CString(szCode), C.CString(szMarket), nBeginDate, nEndDate, nCycle, nUserDef, nCQFlag, nAutoComplete)
}

func WrapperGetTickData(hTdb C.THANDLE, szCode string, szMarket string, nDate int)  {
	return C.GetTickData(hTdb, C.CString(szCode), C.CString(szMarket), nDate)
}

func WrapperGetTransaction(hTdb C.THANDLE, szCode string, szMarketKey string, nDate int)  {
	return C.GetTransaction(hTdb, C.CString(szCode), C.CString(szMarketKey), nDate)
}

func WrapperGetOrder(hTdb C.THANDLE, szCode string, szMarketKey string, nDate int)()  {
	return C.GetOrder(hTdb, C.CString(szCode), C.CString(szMarketKey), nDate)
}

func WrapperGetOrderQueue(hTdb C.THANDLE, szCode string, szMarketKey string, nDate int)  {
	return C.GetOrderQueue(hTdb, C.CString(szCode), C.CString(szMarketKey), nDate)
}

func WrapperUseEZFFormula(hTdb C.THANDLE)  {
	return C.UseEZFFormula(hTdb)
}