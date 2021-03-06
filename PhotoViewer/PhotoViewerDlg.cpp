
// PhotoViewerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "PhotoViewer.h"
#include "PhotoViewerDlg.h"
#include "afxdialogex.h"

#include<functional>
//#include<typeinfo>
using namespace std::placeholders;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef std::function<void(int, int, int, int, void*)>Fun;

Mat m_oImage;
//Mat m_pImage;
int num = 0;
float half_scale = 0.1;
bool isDraw = false;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPhotoViewerDlg 对话框



CPhotoViewerDlg::CPhotoViewerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PHOTOVIEWER_DIALOG, pParent)
{
	m_static = NULL;
	//m_image = Mat(Size(900, 600), CV_32FC3);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPhotoViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPhotoViewerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_COMMAND(ID_32772, &CPhotoViewerDlg::OnFileOpen)
	ON_COMMAND(ID_32773, &CPhotoViewerDlg::OnEnlarge)
	ON_COMMAND(ID_32774, &CPhotoViewerDlg::OnShrink)
	ON_COMMAND(ID_32775, &CPhotoViewerDlg::OnSelect)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CPhotoViewerDlg 消息处理程序

BOOL CPhotoViewerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_bitmap.LoadBitmapW(IDB_BITMAP1);
	GetMenu()->GetSubMenu(0)->SetMenuItemBitmaps(0, MF_BYPOSITION, &m_bitmap, &m_bitmap);
	//初始化显示
	CWnd::SetWindowPos(NULL, 0, 0, 1026, 680, SWP_SHOWWINDOW);
	m_static = (CStatic*)GetDlgItem(IDC_STA_PHOTO);
	m_static->SetWindowPos(NULL, 50, 10, 900, 600, SWP_SHOWWINDOW);
	CenterWindow();

	//获取所有控件，后期缩放
	CRect rect;
	GetWindowRect(&rect);
	m_listRect.AddTail(rect);
	CWnd* pWnd = GetWindow(GW_CHILD);
	while (pWnd)
	{
		pWnd->GetWindowRect(rect);
		m_listRect.AddTail(rect);
		pWnd = pWnd->GetNextWindow();
	}

	//新建opencv窗口，加到图片控件上，显示图片用
	namedWindow("view", WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STA_PHOTO)->m_hWnd);
	::ShowWindow(hParent, SW_SHOWNOACTIVATE);
	Fun fun = std::bind(&OnMouseAction, *this, _1, _2, _3, _4, _5);
	setMouseCallback("view",fun.operator());
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CPhotoViewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPhotoViewerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	Paint();
}
//显示。

//当用户拖动最小化窗口时系统调用此函数取得光标
HCURSOR CPhotoViewerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPhotoViewerDlg::Paint()
{
	m_static->UpdateWindow();
	CDC* pDC = m_static->GetDC();

	CRect ShowRect;
	m_static->GetClientRect(&ShowRect);
	pDC->FillRect(&ShowRect, &CBrush(RGB(255, 255, 255)));

	//图片保持比例缩放，选择缩放尺寸中
	//判断图片是否放大、缩小到极致
	//放大：比例小于等于1时放大按钮无效
	//缩小：图片尺寸等于原图尺寸时缩小按钮无效
	if (m_image.rows <= 0 || m_image.cols <= 0)
		return;
	Mat imageDst;

	int h = m_oImage.rows;
	int w = m_oImage.cols;
	if (num < 0)
	{
		num = 0;
		GetMenu()->EnableMenuItem(ID_32774, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	Mat m_pImage = m_oImage(Rect(half_scale*num*w, half_scale*num*h, w - 2 * half_scale*num*w, h - 2 * half_scale*num*h)).clone();

	CRect rect;
	m_static->GetClientRect(&rect);
	float x = (float)m_pImage.cols / rect.Width();
	float y = (float)m_pImage.rows / rect.Height();
	float scale = x > y ? x : y;
	if (scale < 1)
	{
		//imshow("view", m_image);
		GetMenu()->EnableMenuItem(ID_32773, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		scale = 1;
	}
	if (scale != 1)
	{
		m_image = m_pImage.clone();
	}
	
	resize(m_image, imageDst, Size(m_image.cols / scale, m_image.rows / scale));
	imshow("view", imageDst);
}


void CPhotoViewerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_listRect.GetCount() > 0)
	{
		CRect dlgNow;
		GetWindowRect(&dlgNow);
		POSITION pos = m_listRect.GetHeadPosition();
		CRect dlgSaved;
		dlgSaved = m_listRect.GetNext(pos);
		ScreenToClient(dlgNow);
		float x = dlgNow.Width()*1.0 / dlgSaved.Width();
		float y = dlgNow.Height()*1.0 / dlgSaved.Height();
		ClientToScreen(dlgNow);

		CRect childSaved;
		CWnd* pWnd = GetWindow(GW_CHILD);
		while (pWnd)
		{
			childSaved = m_listRect.GetNext(pos);
			childSaved.left = dlgNow.left + (childSaved.left - dlgSaved.left)*x;
			childSaved.right = dlgNow.right + (childSaved.right - dlgSaved.right)*x;
			childSaved.top = dlgNow.top + (childSaved.top - dlgSaved.top)*y;
			childSaved.bottom = dlgNow.bottom + (childSaved.bottom - dlgSaved.bottom)*y;
			ScreenToClient(childSaved);
			pWnd->MoveWindow(childSaved);
			pWnd = pWnd->GetNextWindow();
		}
	}
	if (nType != SIZE_RESTORED)
	{
		Paint();
	}
}



void CPhotoViewerDlg::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	CString filename;
	CString szFilter = _T("BMP Files (*.BMP)|*.BMP|JPG Files(*.JPG)|*.JPG|TIF Files(*.TIF)|*.TIF|ALL Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, (szFilter), NULL);
	if (dlg.DoModal() == IDOK)
	{
		num = 0;
		string str = CStringA(dlg.GetPathName());
		m_image = imread(str, CV_LOAD_IMAGE_ANYCOLOR);
		m_oImage = m_image.clone();
	}

	Paint();
}


void CPhotoViewerDlg::OnEnlarge()
{
	
	// TODO: 在此添加命令处理程序代码
	GetMenu()->EnableMenuItem(ID_32774, MF_BYCOMMAND | MF_ENABLED);
	num++;
	Paint();
}


void CPhotoViewerDlg::OnShrink()
{
	// TODO: 在此添加命令处理程序代码
	GetMenu()->EnableMenuItem(ID_32773, MF_BYCOMMAND | MF_ENABLED);
	num--;
	Paint();
}


void CPhotoViewerDlg::OnSelect()
{
	// TODO: 在此添加命令处理程序代码
	isDraw = true;
}


void CPhotoViewerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (isDraw == true)
	{
		m_oPoint = point;
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CPhotoViewerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (isDraw == true)
	{
		m_point = point;
		CClientDC dc(this);
		dc.Rectangle(CRect(m_oPoint, m_point));
	}
	isDraw = false;

	CDialogEx::OnLButtonUp(nFlags, point);
}

void CPhotoViewerDlg::OnMouseAction(int event, int x, int y, int flags, void* userdata)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		OnLButtonDown(flags, CPoint(x, y));
	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		OnLButtonUp(flags, CPoint(x, y));
	}
}