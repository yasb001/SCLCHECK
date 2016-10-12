/*
 *  @file  : stdafx.h
 *  @author: user
 *  @date  : 2015-08-11 10:59:37.352
 *  @note  : Generated by SlxTemplates
 */

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__07DB08EF_6632_4DD5_8F63_46654352FF33__INCLUDED_)
#define AFX_STDAFX_H__07DB08EF_6632_4DD5_8F63_46654352FF33__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// gdi+ 头文件
#include <GdiPlus.h> 
using namespace Gdiplus;  
#pragma comment(lib, "GdiPlus.lib")

// #include "rsSCDCRC.h"
// #pragma  comment(lib, "GenerateCRC.lib")

#define SOFTNAME    L"智能变电站检测工具"
#define SOFTVERSION L"V1.0"


#ifdef _UNICODE
#define _xtoi        _wtoi
#define _xaccess     _waccess
#define xLPSTR       LPWSTR
#define xCHAR        wchar_t
#define xstrlen      wcslen
#define xstring      wstring
#else
#define _xtoi        _ttoi
#define _xaccess     _access
#define xLPSTR       LPSTR
#define xCHAR        char
#define xstrlen      strlen
#define xstring      string
#endif


// 界面资源加载类
#include "shared\ImageInfo.h"

// 界面绘制双缓存类
#include "shared\MemoryDC.h"
#include <afxcontrolbars.h>

// #pragma warning(disable:4996)
#pragma warning(disable:4100)

// warning C4244: “参数”: 从“int”转换到“Gdiplus::REAL”，可能丢失数据
#pragma warning(disable:4244)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__07DB08EF_6632_4DD5_8F63_46654352FF33__INCLUDED_)
