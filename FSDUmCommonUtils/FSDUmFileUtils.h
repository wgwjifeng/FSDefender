#pragma once

#include "FSDCommonInclude.h"
#include "AutoPtr.h"
#include "FSDStringUtils.h"

#define INVALID_HANDLE_VALUE ((HANDLE)(LONG_PTR)-1)

class CAutoFile
{
public:
    
    CAutoFile()
    {}

    // dwShareMode FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE
    // dwAccess GENERIC_READ, GENERIC_WRITE
    HRESULT Initialize(LPCWSTR wszFileName, size_t cbFileName, DWORD dwAccess, DWORD dwShareMode, DWORD dwCreationDisposition)
    {
        HRESULT hr = S_OK;

        hr = NewCopyStringW(&m_wszName, wszFileName, cbFileName);
        RETURN_IF_FAILED(hr);

        m_hFile = CreateFileW(m_wszName.Get(), dwAccess, dwShareMode, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
        if (m_hFile == INVALID_HANDLE_VALUE)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        return S_OK;
    }

    HRESULT Read(LPVOID pvBuffer, size_t* pcbBuffer)
    {
        HRESULT hr = S_OK;

        DWORD cbReadSize;
        bool fRead = ReadFile(m_hFile, pvBuffer, numeric_cast<DWORD>(*pcbBuffer), &cbReadSize, NULL);
        if (!fRead)
        {
            *pcbBuffer = 0;
            hr = HRESULT_FROM_WIN32(GetLastError());
            if (hr == E_HANDLE)
            {
                m_hFile = INVALID_HANDLE_VALUE;
                ASSERT(m_hFile == INVALID_HANDLE_VALUE);
            }
            RETURN_IF_FAILED(hr);
        }

        *pcbBuffer = cbReadSize;

        return S_OK;
    }

    HRESULT ReadAll(LPVOID pvBuffer, size_t* pcbBuffer)
    {
        HRESULT hr = S_OK;

        size_t cbData = 0;
        for (;;)
        {
            if (cbData == *pcbBuffer)
            {
                break;
            }

            size_t cbRead = *pcbBuffer - cbData;
            hr = Read((BYTE*)pvBuffer + cbData, &cbRead);
            RETURN_IF_FAILED(hr);

            cbData += cbRead;

            if (cbRead == 0)
            {
                break;
            }
        }

        *pcbBuffer = cbData;

        return S_OK;
    }

private:
    CAutoStringW m_wszName;
    CAutoHandle  m_hFile;
};

LPCWSTR GetFileExtentionFromFileName(LPWSTR wszFileName);