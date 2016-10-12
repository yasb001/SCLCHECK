/*
 *  @file  : Xml2Xsl.cpp
 *  @author: lt
 *  @date  : 2015-10-19 09:03:33.192
 *  @note  : Generated by SlxTemplates
 */

#include <Windows.h>
#include <Shlwapi.h>
#include "../ScdCheckCommon/getopt.h"
#include <stdio.h>
#include <map>
#include <conio.h>
#include "../ScdCheckCommon/charconv.h"
#include "../ScdCheckCommon/rapidxml-1.13/rapidxml.hpp"
#include "../ScdCheckCommon/libxl/libxl.h"

using namespace std;
using namespace rapidxml;
using namespace libxl;

class CFileBuffer
{
public:
    CFileBuffer(const char *lpFilePath)
        : m_lpBuffer(NULL)
    {
        HANDLE hFile = CreateFileA(lpFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        if (hFile != INVALID_HANDLE_VALUE)
        {
            DWORD dwFileSizeHigh = 0;
            DWORD dwFileSize = GetFileSize(hFile, &dwFileSizeHigh);

            if (dwFileSizeHigh == 0 && dwFileSize > 0)
            {
                m_lpBuffer = (char *)malloc(dwFileSize + 1);

                if (m_lpBuffer != NULL)
                {
                    DWORD dwSizeRead = 0;

                    if (ReadFile(hFile, m_lpBuffer, dwFileSize, &dwSizeRead, NULL))
                    {
                        m_lpBuffer[dwFileSize] = 0;
                    }
                    else
                    {
                        free(m_lpBuffer);
                        m_lpBuffer = NULL;
                    }
                }
            }

            CloseHandle(hFile);
        }
    }

    ~CFileBuffer()
    {
        if (m_lpBuffer != NULL)
        {
            free(m_lpBuffer);
        }
    }

    char *GetBuffer() const
    {
        return m_lpBuffer;
    }

private:
    char *m_lpBuffer;
};

enum SheetIndex
{
    ST_BASIC,
    ST_ERROR,
    ST_WARNING,
    ST_TIP,
    ST_COUNT,
};

const wchar_t *g_lpSheetName[] = {
    L"������Ϣ",
    L"����",
    L"����",
    L"��ʾ",
};

class CExcelBuilder
{
    struct ColInfo
    {
        string strXmlAttName;
        wstring strExcelHeaderName;
        double nColWidth;
        int nColIndex;
    };

public:
    CExcelBuilder(const char *lpFilePath, bool bUseXslx)
        : m_pBook(NULL)
        , m_strFilePath(AtoW(lpFilePath))
        , m_nBasicInfoRowCount(0)
        , m_bUseXslx(bUseXslx)
    {
        ColInfo colInfos[] = {
            //{ "##########", L"���", 7.0f, },
            //{ "checkItemKey", L"�����Ψһ��ʶ", 27.0f, },
            //{ "checkItemName", L"���������", 27.0f, },
            { "position", L"�к�", 27.0f, },
            { "desc", L"��ϸ��Ϣ", 27.0f, },
            { "standard", L"���ݱ�׼", 27.0f, },
            { "flag", L"��������", 27.0f, },
            { "checkClass", L"��������������", 27.0f, },
            { "xml", L"xmlƬ��", 27.0f, },
        };

        for (int i = 0, nStartColIndex = 0; i < RTL_NUMBER_OF(colInfos); ++i, nStartColIndex++)
        {
            colInfos[i].nColIndex = nStartColIndex;
            m_mapColInfo[colInfos[i].strXmlAttName] = colInfos[i];
        }

        if (m_bUseXslx)
        {
            m_pBook = xlCreateXMLBookW();
        }
        else
        {
            m_pBook = xlCreateBookW();
        }

        if (m_pBook != NULL)
        {
            m_pBook->setKey(L"�����ݸ��Ƽ����޹�˾", L"windows-26292c0705cde60766bc6667a2hce3l9");

            m_pHeaderFormat = m_pBook->addFormat();
            FontW* pHeaderFont = m_pBook->addFont();
            m_pHeaderFormat->setAlignH(ALIGNH_LEFT);
            m_pHeaderFormat->setBorder(BORDERSTYLE_MEDIUM);
            m_pHeaderFormat->setFillPattern(FILLPATTERN_SOLID);
            m_pHeaderFormat->setPatternForegroundColor(COLOR_YELLOW);
            pHeaderFont->setColor(COLOR_RED);
            pHeaderFont->setBold(true);
            pHeaderFont->setSize(10);
            pHeaderFont->setName(L"����");
            m_pHeaderFormat->setFont(pHeaderFont);
            m_pHeaderFormat->setWrap(true);
            m_pHeaderFormat->setAlignV(ALIGNV_CENTER);

            m_pDefaultFormat = m_pBook->addFormat();
            FontW* pDefaultFont = m_pBook->addFont();
            m_pDefaultFormat->setAlignH(ALIGNH_LEFT);
            m_pDefaultFormat->setBorder(BORDERSTYLE_THIN);
            pDefaultFont->setColor(COLOR_BLUE);
            pDefaultFont->setBold(true);
            pDefaultFont->setSize(10);
            pDefaultFont->setName(L"����");
            m_pDefaultFormat->setFont(pDefaultFont);
            m_pDefaultFormat->setWrap(true);
            m_pDefaultFormat->setAlignV(ALIGNV_CENTER);

            for (int i = 0; i < RTL_NUMBER_OF(m_pSheet); ++i)
            {
                m_pSheet[i] = m_pBook->addSheet(g_lpSheetName[i]);
                m_nRowCount[i] = 1;
            }

            for (int i = ST_ERROR; i < ST_COUNT; ++i)
            {
                for (map<string, ColInfo>::const_iterator it = m_mapColInfo.begin(); it != m_mapColInfo.end(); ++it)
                {
                    const ColInfo &info = it->second;

                    m_pSheet[i]->writeStr(0, info.nColIndex, info.strExcelHeaderName.c_str(), m_pHeaderFormat);
                    m_pSheet[i]->setCol(info.nColIndex, info.nColIndex, info.nColWidth);
                }
            }

            m_pSheet[ST_BASIC]->setCol(0, 1, 45.0f);
        }
    }

    ~CExcelBuilder()
    {
        if (m_pBook != NULL)
        {
            GenerateBasicInfo();
            m_pBook->save(m_strFilePath.c_str());
            m_pBook->release();
        }
    }

    int WriteReportSheetRow(xml_node<char> *pXmlNode)
    {
        int nRetCodeOnError = 0;
        xml_attribute<char> *pFlag = pXmlNode->first_attribute("flag");

        if (pFlag == NULL)
        {
            return nRetCodeOnError;
        }

        char *pFlagValue = pFlag->value();

        if (pFlagValue == NULL)
        {
            return nRetCodeOnError;
        }

        SheetIndex stValue = ST_COUNT;

        switch (*pFlagValue)
        {
        case 'E':
            stValue = ST_ERROR;
            break;

        case 'R':
        case 'T':
            stValue = ST_TIP;
            break;

        case 'W':
            stValue = ST_WARNING;
            break;

        default:
            return nRetCodeOnError;
        }

        if (stValue == ST_COUNT)
        {
            return nRetCodeOnError;
        }

        if (!m_bUseXslx && m_nRowCount[stValue] > 0xffff)
        {
            return 0;
        }

        SheetW *pSheet = m_pSheet[stValue];

        for (map<string, ColInfo>::const_iterator it = m_mapColInfo.begin(); it != m_mapColInfo.end(); ++it)
        {
            const ColInfo &info = it->second;
            xml_attribute<char> *pAttribute = pXmlNode->first_attribute(info.strXmlAttName.c_str());

            if (pAttribute != NULL && pAttribute->value() != NULL)
            {
                pSheet->writeStr(m_nRowCount[stValue], info.nColIndex, UtoW(pAttribute->value()).c_str(), m_pDefaultFormat);
            }
        }

        // pSheet->writeNum(m_nRowCount[stValue], 0, m_nRowCount[stValue], m_pDefaultFormat);
        m_nRowCount[stValue] += 1;

        return 0;
    }

    void WriteBasicSheetRow(const wstring &strKey, const wstring &strValue)
    {
        m_pSheet[ST_BASIC]->writeStr(m_nBasicInfoRowCount, 0, strKey.c_str(), m_pHeaderFormat);
        m_pSheet[ST_BASIC]->writeStr(m_nBasicInfoRowCount, 1, strValue.c_str(), m_pDefaultFormat);
        ++m_nBasicInfoRowCount;
    }

    void WriteBasicSheetRow(const wstring &strKey, int nValue)
    {
        m_pSheet[ST_BASIC]->writeStr(m_nBasicInfoRowCount, 0, strKey.c_str(), m_pHeaderFormat);
        m_pSheet[ST_BASIC]->writeNum(m_nBasicInfoRowCount, 1, nValue, m_pDefaultFormat);
        ++m_nBasicInfoRowCount;
    }

private:
    void GenerateBasicInfo()
    {
        WriteBasicSheetRow(L"������", m_nRowCount[ST_ERROR] - 1);
        WriteBasicSheetRow(L"������", m_nRowCount[ST_WARNING] - 1);
        WriteBasicSheetRow(L"��ʾ��", m_nRowCount[ST_TIP] - 1);
    }

private:
    BookW *m_pBook;
    bool m_bUseXslx;
    SheetW *m_pSheet[ST_COUNT];
    int m_nRowCount[ST_COUNT];
    int m_nBasicInfoRowCount;
    FormatW *m_pHeaderFormat;
    FormatW *m_pDefaultFormat;
    wstring m_strFilePath;
    map<string, ColInfo> m_mapColInfo;
};

wstring GetFileTimeString(const FILETIME &ft)
{
    WCHAR szTimeString[128];
    FILETIME ftLocal;
    SYSTEMTIME stLocal;

    FileTimeToLocalFileTime(&ft, &ftLocal);
    FileTimeToSystemTime(&ftLocal, &stLocal);
    wnsprintfW(
        szTimeString,
        RTL_NUMBER_OF(szTimeString),
        L"%04u-%02u-%02u %02u:%02u:%02u.%03u",
        stLocal.wYear,
        stLocal.wMonth,
        stLocal.wDay,
        stLocal.wHour,
        stLocal.wMinute,
        stLocal.wSecond,
        stLocal.wMilliseconds);

    return szTimeString;
}

int Convert(const char *lpInputFilePath, const char *lpOutputFilePath, const char *lpOriginalFilePath, bool bUseXslx)
{
    CFileBuffer fb(lpInputFilePath);
    char *lpBuffer = fb.GetBuffer();

    if (lpBuffer == NULL)
    {
        return 100;
    }

    xml_document<char> doc;
    xml_node<> *pRoot;

    try
    {
        doc.parse<0>(lpBuffer);
        pRoot = doc.first_node();
    }
    catch (std::exception &)
    {
        return 101;
    }

    try
    {
        CExcelBuilder excel(lpOutputFilePath, bUseXslx);
        xml_node<> *pNode = pRoot->first_node("CheckInfo");

        while (pNode != NULL)
        {
            int nRet = excel.WriteReportSheetRow(pNode);

            if (nRet != 0)
            {
                return nRet;
            }

            pNode = pNode->next_sibling("CheckInfo");
        }

        if (lpOriginalFilePath == NULL)
        {
            excel.WriteBasicSheetRow(L"ԭʼicd�ļ�", L"������");
        }
        else
        {
            CHAR *pfileName = PathFindFileNameA(lpOriginalFilePath);
            wstring fileName = AtoW(pfileName);
            wstring strOriginalFilePath = AtoW(lpOriginalFilePath);
            WIN32_FILE_ATTRIBUTE_DATA wfad;
            WCHAR szText[1024];

            excel.WriteBasicSheetRow(L"ԭʼicd�ļ�", fileName.c_str());

            if (GetFileAttributesExW(strOriginalFilePath.c_str(), GetFileExInfoStandard, &wfad))
            {
                ULARGE_INTEGER uliFileSize;

                uliFileSize.LowPart = wfad.nFileSizeLow;
                uliFileSize.HighPart = wfad.nFileSizeHigh;

                wnsprintfW(szText, RTL_NUMBER_OF(szText), L"%I64u KB", uliFileSize.QuadPart / 1024);
                excel.WriteBasicSheetRow(L"�ļ���С", szText);
                excel.WriteBasicSheetRow(L"����ʱ��", GetFileTimeString(wfad.ftCreationTime).c_str());
                excel.WriteBasicSheetRow(L"����޸�ʱ��", GetFileTimeString(wfad.ftLastWriteTime).c_str());
                excel.WriteBasicSheetRow(L"������ʱ��", GetFileTimeString(wfad.ftLastAccessTime).c_str());
            }
        }
    }
    catch (std::exception &)
    {
        return 102;
    }

    return 0;
}

void show_help()
{
    puts(
        "Usage: Xml2Xsl [options]\n"
        "\n"
        "Xml2Xsl - convert xml file to excel file\n"
        "\n"
        "Options:\n"
        " -i <inputfilepath>        input xml file path\n"
        " -o <outputpath>           output excel file path\n"
        " -I <originalFilePath>     original file path\n"
        " -h                        show this help\n"
        );
}

int main(int argc, char *argv[])
{
    bool bUseXlsx = false;
    char *lpInputFilePath = NULL;
    char *lpOutputFilePath = NULL;
    char *lpOriginalFilePath = NULL;
    int ch = '\0';

    while (-1 != (ch = getopt(argc, argv, "hi:o:I::")))
    {
        switch (ch)
        {
        case 'i':
            lpInputFilePath = optarg;
            break;

        case 'o':
            lpOutputFilePath = optarg;
            break;

        case 'I':
            lpOriginalFilePath = optarg;
            break;

        case 'h':
        default:
            show_help();
            _getch();
            return -1;
        }
    }

    if (optind < argc) {
        // ʣ�����������
    }

    if (lpInputFilePath == NULL || lpOutputFilePath == NULL)
    {
        show_help();
        _getch();
        return -1;
    }

    if (!PathFileExistsA(lpInputFilePath))
    {
        puts("input file not exists.\n");
        return 1;
    }

    LPCSTR lpTargetExt = PathFindExtensionA(lpOutputFilePath);

    if (lstrcmpiA(lpTargetExt, ".xls") == 0)
    {
        bUseXlsx = false;
    }
    else if (lstrcmpiA(lpTargetExt, ".xlsx") == 0)
    {
        bUseXlsx = true;
    }
    else
    {
        puts("wrong file extension.\n");
        return 2;
    }

    int nRet = Convert(lpInputFilePath, lpOutputFilePath, lpOriginalFilePath, bUseXlsx);

    if (nRet != 0)
    {
        printf("error: %d\n", nRet);
    }

    return nRet;
}