//与客户端交互协议包头
typedef struct tagNtPkgHead
{
    unsigned char   bStartFlag;     //协议包起始标志 0xFF
    unsigned char   bVer;           //版本号
    unsigned char   bEncryptFlag;   //加密标志(如果不加密,则为0)
    unsigned char   bFrag;          //是否有包分片(1 有包分片 0 无包分片)
    unsigned short  wLen;           //总包长
    unsigned short  wCmd;           //命令号
    unsigned short  wSeq;           //包的序列号,业务使用
    unsigned short  wCrc;           //Crc16校验码
    unsigned int    dwSID;          //会话ID
    unsigned short  wTotal;         //有包分片时，分片总数
    unsigned short  wCurSeq;        //有包分片时，分片序号，从0开始，无分片时也为0
} NtPkgHead, *PNtPkgHead;

UINT CSocketClient::RecvDataThreadProc(LPVOID lpParam)
{
    LOG_NORMAL("Start recv data thread.");
    DWORD           dwWaitResult;
    std::string     strPkg;
    //临时存放一个完整的包数据的变量
    std::string     strTotalPkg;
    unsigned short  uPkgLen = 0;
    unsigned int    uBodyLen = 0;
    unsigned int    uTotalPkgLen = 0;
    unsigned int    uCmd = 0;
    NtPkgHead       pkgHead;
    unsigned short  uTotal = 0;
    //记录上一次的包分片序号，包分片序号从0开始
    unsigned short  uCurSeq = 0;
    int             nWaitTimeout = 1;

    CSocketClient* pSocketClient = (CSocketClient*)lpParam;

    while (!m_bExit)
    {      
        //检测是否有数据
        if (!pSocketClient->CheckReceivedData())
        {
            //休眠10豪秒
            Sleep(10);
            continue;
        }
            
        //接收数据，并放入pSocketClient->m_strRecvBuf中
        if (!pSocketClient->Recv())
        {
            LOG_ERROR("Recv data error");
                
            //收数据出错，清空接收缓冲区，可以做一些关闭连接、重连等动作，
            pSocketClient->m_strRecvBuf.clear();

            Reconnect();
            continue;
        }

        //一定要放在一个循环里面解包，因为可能一片数据中有多个包，
        while (true)
        {
            //判断当前收到的数据是否够一个包头大小
            if (pSocketClient->m_strRecvBuf.length() < sizeof(NtPkgHead))
                break;

            memset(&pkgHead, 0, sizeof(pkgHead));
            memcpy_s(&pkgHead, sizeof(pkgHead), pSocketClient->m_strRecvBuf.c_str(), sizeof(pkgHead));
			
			//对包消息头检验
            if (!CheckPkgHead(&pkgHead))
            {
                //如果包头检验不通过，缓冲区里面的数据已经是脏数据了，直接清空掉，
                //可以做一些关闭连接并重连的动作             
                LOG_ERROR("Check package head error, discard data %d bytes", (int)pSocketClient->m_strRecvBuf.length());
                
                pSocketClient->m_strRecvBuf.clear();

                Reconnect();
                break;
            }

            //判断当前数据是否够一个整包的大小
            uPkgLen = ntohs(pkgHead.wLen);
            if (pSocketClient->m_strRecvBuf.length() < uPkgLen)
                break;

            strPkg.clear();
            strPkg.append(pSocketClient->m_strRecvBuf.c_str(), uPkgLen);

            //从收取缓冲区中移除已经处理的数据部分
            pSocketClient->m_strRecvBuf.erase(0, uPkgLen);
       
            uTotal = ::ntohs(pkgHead.wTotal);
            uCurSeq = ::ntohs(pkgHead.wCurSeq);
            //无分片或第一个分片
            if (uCurSeq == 0)
            {
                strTotalPkg.clear();
                uTotalPkgLen = 0;
            }

            uBodyLen = uPkgLen - sizeof(NtPkgHead);
            uTotalPkgLen += uBodyLen;
            strTotalPkg.append(strPkg.data() + sizeof(NtPkgHead), uBodyLen);

            //无分包 或 分包的最后一个包 则将组装后的包发送出去
            if (uTotal == 0 || (uTotal != 0 && uTotal == uCurSeq + 1))
            {
                uCmd = ::ntohs(pkgHead.wCmd);

                //ProxyPackage是解析出来的业务包定义
                ProxyPackage proxyPackage;
                //拷贝业务号
                proxyPackage.nCmd = uCmd;
                //拷贝包长度
                proxyPackage.nLength = uTotalPkgLen;
                //拷贝包体内容
                proxyPackage.pszJson = new char[uTotalPkgLen];
                memset(proxyPackage.pszJson, 0, uTotalPkgLen * sizeof(char));
                memcpy_s(proxyPackage.pszJson, uTotalPkgLen, strTotalPkg.c_str(), strTotalPkg.length());

                //将一个完整的包交给业务处理
                pSocketClient->m_pNetProxy->AddPackage((const char*)&proxyPackage, sizeof(proxyPackage));
            }
        }// end inner-while-loop


    }// end outer-while-loop


    LOG_NORMAL("Exit recv data thread.");

    return 0;
}