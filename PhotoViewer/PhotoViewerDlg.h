
// PhotoViewerDlg.h: 头文件
//

#pragma once
#include"resource.h"
#include<opencv2/core.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
using namespace cv;
using namespace std;


// CPhotoViewerDlg 对话框
class CPhotoViewerDlg : public CDialogEx
{
// 构造
public:
	CPhotoViewerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PHOTOVIEWER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CBitmap m_bitmap;
	Mat m_image;
	CStatic* m_static;
	CList<CRect, CRect&> m_listRect;
	CPoint m_oPoint;
	CPoint m_point;
	/*CBitmapButton m_btn1;
	CBitmapButton m_btn2;*/
	vector<CString>m_vecResDirData;
	int m_number;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void Paint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileOpen();
	afx_msg void OnEnlarge();
	afx_msg void OnShrink();
	afx_msg void OnSelect();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnBnClickedButtonLast();

	bool ReadFile(const CString& strPath, const CString & strPathName);
public:
	CButton m_btnLast;
	CButton m_btnNext;
	afx_msg void OnBnClickedButtonNext();
};
