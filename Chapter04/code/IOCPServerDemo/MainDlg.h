// MainDlg.h : 头文件
//
#pragma once

#include "IOCPModel.h"

// CMainDlg 对话框
class CMainDlg : public CDialog
{
public:
	CMainDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PIGGYIOCPSERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	// 开始监听
	afx_msg void OnBnClickedOk();
	// 停止监听
	afx_msg void OnBnClickedStop();
	// "退出"按钮
	afx_msg void OnBnClickedCancel();
	//	系统退出的时候，为确保资源释放，停止监听，清空Socket类库
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:
	// 初始化Socket库以及界面信息
	void Init();

	// 初始化List控件
	void InitListCtrl();

public:
	// 当前客户端有新消息到来的时候，在主界面中显示新到来的信息(在类CIOCPModel中调用)
	void AddInformation(const CString& strInfo)
	{
		CListCtrl* pList = (CListCtrl*)GetDlgItem(IDC_LIST_INFO);
		pList->InsertItem(0, strInfo);
	}

private:
	// 主要对象，完成端口模型
	CIOCPModel m_IOCP;
};
