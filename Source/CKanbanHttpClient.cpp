/*
  ==============================================================================

    CKanbanHttpClient.cpp
    Created: 21 Sep 2021 12:01:36am
    Author:  michal.strug

  ==============================================================================
*/

#include "CKanbanHttpClient.h"

const int KSOcketRetryCount = 1;


int CHttpClient::Get(const String & aIp, uint16 aPort, const String& aUrl, const String& aBody, const String& aAdditionalHeaders, String& aReturnCode, String& aReturnMessage)
{    
    String body(aBody);
    String url(aUrl);

    String buffer("GET ");
    buffer += url + " HTTP/1.1\r\n";
    buffer += "Host: " + aIp + ":" + String(aPort) + "\r\n";
    buffer += "Content-Type: text/plain\r\n";
    buffer += "Content-Length: " + String(body.length()) + "\r\n" + "\r\n";
    buffer += body;

    StreamingSocket sock;
    if (!sock.connect( aIp, aPort))
    {
        return -1;
    }

    auto wait_ret = sock.waitUntilReady(false, 1000);
    if (wait_ret == 0)
    {
        sock.close();
        return -2;
    }
    if (wait_ret == -1)
    {
        sock.close();
        return -3;
    }

    for (int i = 0; i <= KSOcketRetryCount; i++)
    {
        auto write_ret = sock.write(buffer.toRawUTF8(), buffer.length());
        if (write_ret == -1)
        {
             if (i <= KSOcketRetryCount - 1)
             { //retry
                   Time::waitForMillisecondCounter(Time::getMillisecondCounter() + (i + 1) * 900);
                   continue;
             }

             sock.close();
             return -4;
         }
         else if (write_ret != buffer.length())
         {
             if (i <= KSOcketRetryCount - 1)
             { //retry
                   Time::waitForMillisecondCounter(Time::getMillisecondCounter() + (i + 1) * 900);
                   continue;
             }

             sock.close();
             return -5;
        }
        break;
    }


    wait_ret = sock.waitUntilReady(true, 1000);
    if (wait_ret == 0)
    {
        sock.close();
        return -6;
    }
    if (wait_ret == -1)
    {
//              sock.close();
//              return false;
    }

    Array<uint8> readbuf;
    int read_ret;
    for (int i = 0; i <= KSOcketRetryCount; i++)
    {
        readbuf.resize(1024);
        read_ret = sock.read(readbuf.getRawDataPointer(), readbuf.size(), false);

        if (read_ret == -1)
        {
            if (i <= KSOcketRetryCount - 1)
            { //retry
                  Time::waitForMillisecondCounter(Time::getMillisecondCounter() + (i + 1) * 900);
                  continue;
            }

            sock.close();
            return false;
        }
        readbuf.resize(read_ret);
        break;
    }

    String rsp((char*)readbuf.data(), readbuf.size());

    sock.close();
    
	String code = rsp.upToFirstOccurrenceOf("\r\n", false, true);

	aReturnMessage = rsp;
    aReturnCode = code;
    
    return readbuf.size();
}


