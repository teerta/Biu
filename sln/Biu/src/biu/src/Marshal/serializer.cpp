
#include "stdafx.h"
#include "bs.h"
#include "bstype.h"
#include <string.h>

#if defined(__linux__) || defined(__APPLE__)
#include <arpa/inet.h>
#endif


using namespace Biu;


BSerializer::BSerializer()
        : m_nReadPos(0)
        , m_pShack(NULL)
        , m_pBrick(NULL)
{
}

BSerializer::~BSerializer()
{
}

void BSerializer::Serialize(bool val)
{
    char buff[] = { X_S_N1, 0 };
    buff[1] = val;
    BWriteBrick(m_pBrick, m_pShack, buff, 2);
}

void BSerializer::Serialize(wchar_t val)
{
    wchar_t v = htons(val);
    char buff[] = { X_S_N2, 0, 0 };
    memcpy(buff + 1, &v, 2);
    BWriteBrick(m_pBrick, m_pShack, buff, 3);
}

void BSerializer::Serialize(char val)
{
    char buff[] = { X_S_N1, 0 };
    buff[1] = val;
    BWriteBrick(m_pBrick, m_pShack, buff, 2);
}

void BSerializer::Serialize(unsigned char val)
{
    char buff[] = { X_S_N1, 0 };
    buff[1] = val;
    BWriteBrick(m_pBrick, m_pShack, buff, 2);
}

void BSerializer::Serialize(short val)
{
    short v = htons(val);
    char buff[] = { X_S_N2, 0, 0 };
    memcpy(buff + 1, &v, 2);
    BWriteBrick(m_pBrick, m_pShack, buff, 3);
}

void BSerializer::Serialize(unsigned short val)
{
    unsigned short v = htons(val);
    char buff[] = { X_S_N2, 0, 0 };
    memcpy(buff + 1, &v, 2);
    BWriteBrick(m_pBrick, m_pShack, buff, 3);
}

void BSerializer::Serialize(int val)
{
    int v = htonl(val);
    char buff[] = { X_S_N4, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
}

void BSerializer::Serialize(unsigned int val)
{
    unsigned int v = htonl(val);
    char buff[] = { X_S_N4, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
}

void BSerializer::Serialize(long val)
{
    long v = htonl(val);
    char buff[] = { X_S_N4, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
}

void BSerializer::Serialize(unsigned long val)
{
    unsigned long v = htonl(val);
    char buff[] = { X_S_N4, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
}

void BSerializer::Serialize(long long val)
{
    long long v = htonll(val);
    char buff[] = { X_S_N8, 0, 0, 0, 0, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 8);
    BWriteBrick(m_pBrick, m_pShack, buff, 9);
}

void BSerializer::Serialize(unsigned long long val)
{
    unsigned long long v = htonll(val);
    char buff[] = { X_S_N8, 0, 0, 0, 0, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 8);
    BWriteBrick(m_pBrick, m_pShack, buff, 9);
}

void BSerializer::Serialize(float val)
{
    unsigned long v = htonl(pack754_32(val));
    char buff[] = { X_S_N4, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
}

void BSerializer::Serialize(double val)
{
    unsigned long long v = htonll(pack754_64(val));
    char buff[] = { X_S_N8, 0, 0, 0, 0, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 8);
    BWriteBrick(m_pBrick, m_pShack, buff, 9);
}

void BSerializer::Serialize(long double val)
{
    unsigned long long v = htonll(pack754_64(val));
    char buff[] = { X_S_N8, 0, 0, 0, 0, 0, 0, 0, 0 };
    memcpy(buff + 1, &v, 8);
    BWriteBrick(m_pBrick, m_pShack, buff, 9);
}

void BSerializer::Serialize(wchar_t* pval, unsigned int cb)
{
    for (unsigned int i = 0; i < cb; ++i)
    {
        pval[i] = htons(pval[i]);
    }
    char buff[] = { X_S_V, 0, 0, 0, 0 };
    unsigned int c = htonl(cb);
    memcpy(buff + 1, &c, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
    BWriteBrick(m_pBrick, m_pShack, reinterpret_cast<char*>(pval), cb * 2);
}

void BSerializer::Serialize(char* pval, unsigned int cb)
{
    char buff[] = { X_S_V, 0, 0, 0, 0 };
    unsigned int c = htonl(cb);
    memcpy(buff + 1, &c, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
    BWriteBrick(m_pBrick, m_pShack, pval, cb);
}

void BSerializer::Serialize(unsigned char* pval, unsigned int cb)
{
    char buff[] = { X_S_V, 0, 0, 0, 0 };
    unsigned int c = htonl(cb);
    memcpy(buff + 1, &c, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
    BWriteBrick(m_pBrick, m_pShack, reinterpret_cast<char*>(pval), cb);
}

void BSerializer::Serialize(BCabUTF8String& str)
{
    char buff[] = { X_S_V, 0, 0, 0, 0 };
    unsigned int c = htonl(str.GetLength());
    memcpy(buff + 1, &c, 4);
    BWriteBrick(m_pBrick, m_pShack, buff, 5);
    BWriteBrick(m_pBrick, m_pShack, (char*)str, str.GetLength());
}

bool BSerializer::Deserialize(bool& val)
{
    char buff[] = { 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 2, m_nReadPos));
    CHK_false(X_S_N1 == buff[0]);
    val = buff[1];
    
    return true;
}

bool BSerializer::Deserialize(wchar_t& val)
{
    char buff[] = { 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 3, m_nReadPos));
    CHK_false(X_S_N2 == buff[0]);
    val = *reinterpret_cast<wchar_t*>(buff + 1);
    val = ntohs(val);
    
    return true;
}

bool BSerializer::Deserialize(char& val)
{
    char buff[] = { 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 2, m_nReadPos));
    CHK_false(X_S_N1 == buff[0]);
    val = buff[1];
    
    return true;
}

bool BSerializer::Deserialize(unsigned char& val)
{
    char buff[] = { 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 2, m_nReadPos));
    CHK_false(X_S_N1 == buff[0]);
    val = buff[1];
    
    return true;
}

bool BSerializer::Deserialize(short& val)
{
    char buff[] = { 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 3, m_nReadPos));
    CHK_false(X_S_N2 == buff[0]);
    val = *reinterpret_cast<short*>(buff + 1);
    val = ntohs(val);
    
    return true;
}

bool BSerializer::Deserialize(unsigned short& val)
{
    char buff[] = { 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 3, m_nReadPos));
    CHK_false(X_S_N2 == buff[0]);
    val = *reinterpret_cast<unsigned short*>(buff + 1);
    val = ntohs(val);
    
    return true;
}

bool BSerializer::Deserialize(int& val)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_N4 == buff[0]);
    val = *reinterpret_cast<int*>(buff + 1);
    val = ntohl(val);
    
    return true;
}

bool BSerializer::Deserialize(unsigned int& val)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_N4 == buff[0]);
    val = *reinterpret_cast<unsigned int*>(buff + 1);
    val = ntohl(val);
    
    return true;
}

bool BSerializer::Deserialize(long& val)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_N4 == buff[0]);
    val = *reinterpret_cast<long*>(buff + 1);
    val = ntohl(val);
    
    return true;
}

bool BSerializer::Deserialize(unsigned long& val)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_N4 == buff[0]);
    val = *reinterpret_cast<unsigned long*>(buff + 1);
    val = ntohl(val);
    
    return true;
}

bool BSerializer::Deserialize(long long& val)
{
    char buff[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 9, m_nReadPos));
    CHK_false(X_S_N8 == buff[0]);
    val = *reinterpret_cast<long long*>(buff + 1);
    val = ntohll(val);
    
    return true;
}

bool BSerializer::Deserialize(unsigned long long& val)
{
    char buff[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 9, m_nReadPos));
    CHK_false(X_S_N8 == buff[0]);
    val = *reinterpret_cast<unsigned long long*>(buff + 1);
    val = ntohll(val);
    
    return true;
}

bool BSerializer::Deserialize(float& val)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_N4 == buff[0]);
    long l = *reinterpret_cast<long*>(buff + 1);
    val = (float)unpack754_32(ntohl(l));
    
    return true;
}

bool BSerializer::Deserialize(double& val)
{
    char buff[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 9, m_nReadPos));
    CHK_false(X_S_N8 == buff[0]);
    long long ll = *reinterpret_cast<long long*>(buff + 1);
    val = unpack754_64(ntohll(ll));
    
    return true;
}

bool BSerializer::Deserialize(long double& val)
{
    char buff[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 9, m_nReadPos));
    CHK_false(X_S_N8 == buff[0]);
    long long ll = *reinterpret_cast<long long*>(buff + 1);
    val = unpack754_64(ntohll(ll));
    
    return true;
}

bool BSerializer::Deserialize(wchar_t** ppval, unsigned int* lpcb, bool bAddNull)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_V == buff[0]);
    unsigned int cb = ntohl(*reinterpret_cast<unsigned int*>(buff + 1));
    *lpcb = cb + (bAddNull ? 1 : 0);
    *ppval = new wchar_t[*lpcb];
    CHK_false(m_funcRead(m_pBrick, reinterpret_cast<char*>(*ppval), cb * 2, m_nReadPos));
    if (bAddNull)
    {
        (*ppval)[cb] = L'\0';
        *lpcb -= 1;
    }    
    
    return true;
}

bool BSerializer::Deserialize(char** ppval, unsigned int* lpcb, bool bAddNull)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_V == buff[0]);
    unsigned int cb = ntohl(*reinterpret_cast<unsigned int*>(buff + 1));
    *lpcb = cb + (bAddNull ? 1 : 0);
    *ppval = new char[*lpcb];
    CHK_false(m_funcRead(m_pBrick, *ppval, cb, m_nReadPos));
    if (bAddNull)
    {
        (*ppval)[cb] = '\0';
        *lpcb -= 1;
    }    
    
    return true;
}

bool BSerializer::Deserialize(unsigned char** ppval, unsigned int* lpcb)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_V == buff[0]);
    *lpcb = ntohl(*reinterpret_cast<unsigned int*>(buff + 1));
    *ppval = new unsigned char[*lpcb];
    CHK_false(m_funcRead(m_pBrick, reinterpret_cast<char*>(*ppval), *lpcb, m_nReadPos));
    
    return true;
}

bool BSerializer::Deserialize(unsigned char* pval, unsigned int cb)
{
    char buff[] = { 0, 0, 0, 0, 0 };
    CHK_false(m_funcRead(m_pBrick, buff, 5, m_nReadPos));
    CHK_false(X_S_V == buff[0]);
    unsigned int cbData = ntohl(*reinterpret_cast<unsigned int*>(buff + 1));
    CHK_false(cbData == cb);
    CHK_false(m_funcRead(m_pBrick, reinterpret_cast<char*>(pval), cb, m_nReadPos));
    
    return true;
}

bool BSerializer::Deserialize(BCabUTF8String& str, bool bAddNull)
{
    unsigned int* cb = (unsigned int*)str.LockLength();
    char** ppsz = str.LockData();
    
    return Deserialize(ppsz, cb, bAddNull);
}
