// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// amsdView.cpp : CamsdView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "amsd.h"
#endif

#include "amsdDoc.h"
#include "amsdView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Src/EventTypes.h"
#include "Calculation/calcHelper.h"
using namespace Calculation;
// CamsdView

IMPLEMENT_DYNCREATE(CamsdView, CView)

BEGIN_MESSAGE_MAP(CamsdView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CamsdView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN,&CamsdView::OnFileOpen)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_COMMAND(ID_BUTTON2, &CamsdView::OnObjA_LengthAccuracy)
	ON_COMMAND(ID_BUTTON3, &CamsdView::OnTrianglePathLength)
	ON_COMMAND(ID_BUTTON4, &CamsdView::OnNEQCalculation)
	ON_COMMAND(ID_BUTTON6, &CamsdView::OnMetalUniformity)
	ON_COMMAND(ID_BUTTON8, &CamsdView::OnAMSD_SSP)
	ON_WM_HSCROLL()
	ON_COMMAND(sldPickThreshold, &CamsdView::Onsldpickthreshold)
END_MESSAGE_MAP()

// CamsdView 构造/析构

CamsdView::CamsdView()
{
	// TODO: 在此处添加构造代码
	keyShift=false;
	keyControl=false;
	updateViewInMouseMove=false;
	keyDown=mouseDown=false;
	app=0;
}

CamsdView::~CamsdView()
{
	//if(app)
	//delete  app;
}

BOOL CamsdView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
	return CView::PreCreateWindow(cs);
}
//参数设计对话框####################################################
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CParametersDlg : public CDialogEx
{
public:
	CParametersDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_PARAMETERS };
public:
	s32 SizeX,SizeY,SizeZ;
	f64 DistanceX,DistanceY,DistanceZ;
	s32 DistanceOffsetZ;
	bool InverseZ;
	CButton CheckButton;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
// 实现
	virtual BOOL OnInitDialog();
	virtual void OnOK();
protected:
	DECLARE_MESSAGE_MAP()
};

CParametersDlg::CParametersDlg() : CDialogEx(CParametersDlg::IDD)
{
	
}

BOOL CParametersDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	//GetDlgItem(txtSizeX)->SetWindowTextW(L"768");
	//GetDlgItem(txtSizeY)->SetWindowTextW(L"656");
	//GetDlgItem(txtSizeZ)->SetWindowTextW(L"126");

	//GetDlgItem(txtDistanceX)->SetWindowTextW(L"1.35");
	//GetDlgItem(txtDistanceY)->SetWindowTextW(L"1.35");
	//GetDlgItem(txtDistanceZ)->SetWindowTextW(L"6.67");

	//GetDlgItem(txtOffsetDistanceZ)->SetWindowTextW(L"0");
	
	SizeX = 768;
	SizeY = 656;
	SizeZ = 126;

	DistanceX =1.35;
	DistanceY =1.35;
	DistanceZ =6.67;

	//SizeX = 640;
	//SizeY = 544;
	//SizeZ = 200;

	//DistanceX =1.2;
	//DistanceY =1.2;
	//DistanceZ =3.3;

	DistanceOffsetZ = 0;
	//InverseLoad = false;
	//write initial data to control
	//reference:http://blog.csdn.net/hfnhzpe/article/details/2474889
	UpdateData(false);

	return TRUE;
}

void CParametersDlg::OnOK()
{
	//get the changed data
	UpdateData(true);
	//reference:1 https://msdn.microsoft.com/en-us/library/8tba1y6f(v=vs.100)
	//2:http://stackoverflow.com/questions/12091887/how-to-see-if-an-mfc-checkbox-is-selected
	InverseZ=(CheckButton.GetCheck()&BST_CHECKED);

	CDialogEx::OnOK();
}
void CParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//reference:http://bbs.csdn.net/topics/300059826
	DDX_Text(pDX,txtSizeX,SizeX);
	DDX_Text(pDX,txtSizeY,SizeY);
	DDX_Text(pDX,txtSizeZ,SizeZ);

	DDX_Text(pDX,txtDistanceX,DistanceX);
	DDX_Text(pDX,txtDistanceY,DistanceY);
	DDX_Text(pDX,txtDistanceZ,DistanceZ);

	DDX_Text(pDX,txtOffsetDistanceZ,DistanceOffsetZ);
	
	DDX_Control(pDX,chkInverseLoad,CheckButton);
	//DDX_Text(pDX,chkInverseLoad,InverseLoad);
}

BEGIN_MESSAGE_MAP(CParametersDlg, CDialogEx)
END_MESSAGE_MAP()


//参数设计对话框####################################################

void CamsdView::OnFileOpen()
{
	//std::cout<<"he";
	//reference:<<CAD应用程序开发详解-visual c++与OpenGL综合应用>>p286
	CFileDialog dlg(TRUE,L"CT Data",NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		L"CT元数据(*.*)|*.*",NULL);

	if(dlg.DoModal() == IDOK)
	{
		//reference:http://stackoverflow.com/questions/859304/convert-cstring-to-const-char
		//get the filename
		CString fileNameW = dlg.GetPathName();
		CStringA fileNameA(fileNameW);
		core::stringc fileName =((const char *) fileNameA);
		

		s32 sizeX,sizeY,sizeZ;
		f64 mmX,mmY,mmZ;
		s32 offset;
		bool zInverse;
		//get parameters
		//CAboutDlg aboutDlg;
		CParametersDlg paramsDlg;
		if(paramsDlg.DoModal() == IDOK)
		{
		//load CT Data
		if(app)
			delete app;
			/*
			CString inputText;
			//parse text to int

			//reference:http://www.cnblogs.com/nickyan/archive/2011/07/15/2107616.html
			paramsDlg.GetDlgItem(txtSizeX)->GetWindowTextW(inputText);
			sizeX = _ttoi(inputText);
			paramsDlg.GetDlgItem(txtSizeY)->GetWindowTextW(inputText);
			sizeY = _ttoi(inputText);
			paramsDlg.GetDlgItem(txtSizeZ)->GetWindowTextW(inputText);
			sizeZ = _ttoi(inputText);

			//parse string to float

			//reference:http://stackoverflow.com/questions/5788772/convert-cstring-to-float-in-mfc
			paramsDlg.GetDlgItem(txtDistanceX)->GetWindowTextW(inputText);
			mmX = _ttof(inputText);
			paramsDlg.GetDlgItem(txtDistanceY)->GetWindowTextW(inputText);
			mmY = _ttof(inputText);
			paramsDlg.GetDlgItem(txtDistanceZ)->GetWindowTextW(inputText);
			mmZ = _ttof(inputText);

			//offset
			paramsDlg.GetDlgItem(txtOffsetDistanceZ)->GetWindowTextW(inputText);
			offset =_ttoi(inputText);
			*/

			sizeX = paramsDlg.SizeX;
			sizeY = paramsDlg.SizeY;
			sizeZ = paramsDlg.SizeZ;

			mmX = paramsDlg.DistanceX;
			mmY = paramsDlg.DistanceY;
			mmZ = paramsDlg.DistanceZ;

			offset = paramsDlg.DistanceOffsetZ;
			zInverse = paramsDlg.InverseZ;//(paramsDlg.CheckButton.GetCheck()&BST_CHECKED);
			glInit();
			app = new IApp(fileName,sizeX,sizeY,sizeZ,(f32)mmX,(f32)mmY,(f32)mmZ,offset,zInverse);
			glResize(Width,Height);
			//std::cout<<fileName.c_str();
		}
		//aboutDlg.DoModal();
		
	}

}
// CamsdView 绘制

void CamsdView::OnDraw(CDC* /*pDC*/)
{
	CamsdDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: 在此处为本机数据添加绘制代码
	//attach rendering context
	wglMakeCurrent(m_hDC,m_hRC);
	//rendering
	
	RenderScene();
	//flush
	SwapBuffers(m_hDC);
	//deattach
	//wglMakeCurrent(m_hDC,NULL);
}


// CamsdView 打印


void CamsdView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CamsdView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CamsdView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CamsdView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CamsdView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);

	mouseDown--;
}

void CamsdView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CamsdView 诊断

#ifdef _DEBUG
void CamsdView::AssertValid() const
{
	CView::AssertValid();
}

void CamsdView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CamsdDoc* CamsdView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CamsdDoc)));
	return (CamsdDoc*)m_pDocument;
}
#endif //_DEBUG


// CamsdView 消息处理程序
//create opengl device
void CamsdView::glInit()
{
	m_hDC = ::GetDC(m_hWnd);
	ASSERT(m_hDC);

	PIXELFORMATDESCRIPTOR pixelDesc=
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|
        PFD_DOUBLEBUFFER|PFD_SUPPORT_GDI,
        PFD_TYPE_RGBA,
        24,
        0,0,0,0,0,0,
        0,
        0,
        0,
        0,0,0,0,
        32,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0,0,0
    };

	int pixelFormat = ChoosePixelFormat(m_hDC,&pixelDesc);

	ASSERT(SetPixelFormat(m_hDC,pixelFormat,&pixelDesc));

	//get rendering context
	m_hRC = ::wglCreateContext(m_hDC);

	//wglMakeCurrent(m_hDC,m_hRC);
	//setupRC(); error!!!!
	//make current
	VERIFY(wglMakeCurrent(m_hDC,m_hRC));

	//init scene rendering state
	//setupRC();
};

int CamsdView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	//memory leaks
	//_CrtSetBreakAlloc(207429);//489为内存泄露块
	// TODO:  在此添加您专用的创建代码
	glInit();
	app = new IApp();
	return 0;
}

//remove all opengl variables
void CamsdView::glRelease()
{
	wglDeleteContext(m_hRC);
	::ReleaseDC(m_hWnd,m_hDC);
};
//change viewport
void CamsdView::glResize(int x,int y)
{
	double range =1200;
	//divide zero?
	if(y==0)
		y=1;

	//setup viewport
	glViewport(0,0,x,y);
	//setup projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/*if(x<y)
		glOrtho(-range,range,-range*y/x,range*y/x,1,2*range);
	else
		glOrtho(-range*x/y,range*x/y,-range,range,1,2*range);*/
	gluPerspective(45, x/(float)y, 0.1f, 1000.0f); 
	//setup view
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	SEvent event;
	event.type = ET_USER;
	event.UserData.type = EUT_RESIZE;
	event.UserData.Size.width = x;
	event.UserData.Size.height = y;

	app->OnEvent(event);

};
//set up gl state
void CamsdView::setupRC()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	//set up the light
	float ambientLight[] = {0.2f,0.2f,0.2f,1.0f};
	float diffuseLight[] = {0.6f,0.6f,0.6f,1.0f};
	float specularLight[] = {1.0f,1.0f,1.0f,1.0f};
	float lightPos[] = {-1000.0f,1000.0f,1000.0f,1.0f};

	//setup and open a light
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specularLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	//setup material
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT,GL_SPECULAR,specularLight);
	glMateriali(GL_FRONT,GL_SHININESS,100);

	//setup background color
	glClearColor(0.0f,0.0f,0.0f,1.0f);

	//default foreground color
	glColor3ub(0,0,255);
};

void CamsdView::OnDestroy()
{
	this->glRelease();
	CView::OnDestroy();
	delete app;
	// TODO: 在此处添加消息处理程序代码
}


void CamsdView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	Width = cx;
	Height = cy;
	// TODO: 在此处添加消息处理程序代码
	VERIFY(wglMakeCurrent(m_hDC,m_hRC));
	glResize(cx,cy);
	VERIFY(wglMakeCurrent(NULL,NULL));
}
void CamsdView::RenderScene()
{
	app->draw();
    glFlush();
}

void CamsdView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nRepCnt != 1)
		return;
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_KEY;
	event.Key.keycode = nChar;
	event.Key.iskeydown = true;
	if(nChar==17)
		keyControl = true;

	event.Key.control=keyControl;
	if(nChar==16)
		keyShift = true;
	event.Key.shift = keyShift;
	//ignore the case that we pressed control and shit
	//if((nChar!=17)&&(nChar!=16))
	app->OnEvent(event);
	/*char tmp[20];
	sprintf(tmp,"ctrl=%2d shift=%2d",event.Key.control,event.Key.shift);
	::OutputDebugStringA(tmp);*/
	updateViewWindow();
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CamsdView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_KEY;
	event.Key.keycode = nChar;
	event.Key.iskeydown = false;

	if(nChar==17)
		keyControl = false;
	event.Key.control=keyControl;
	if(nChar==16)
		keyShift = false;
	event.Key.shift = keyShift;
	//ignore the case that we pressed control and shit
	if((nChar!=17)&&(nChar!=16))
	app->OnEvent(event);

	keyDown--;

	updateViewWindow();
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CamsdView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_MOUSE;
	event.Mouse.mouseaction = EMT_LEFT_DOWN;
	event.Mouse.x = point.x;
	event.Mouse.y = point.y;

	app->OnEvent(event);
	mouseDown++;
	updateViewWindow();
	CView::OnLButtonDown(nFlags, point);
}


void CamsdView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_MOUSE;
	event.Mouse.mouseaction = EMT_LEFT_UP;
	event.Mouse.x = point.x;
	event.Mouse.y = point.y;
	app->OnEvent(event);
	mouseDown--;
	updateViewWindow();
	CView::OnLButtonUp(nFlags, point);
}


void CamsdView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_MOUSE;
	event.Mouse.mouseaction = EMT_MIDDLE_DOWN;
	event.Mouse.x = point.x;
	event.Mouse.y = point.y;
	app->OnEvent(event);
	
	mouseDown++;
	updateViewWindow();
	CView::OnMButtonDown(nFlags, point);
}


void CamsdView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_MOUSE;
	event.Mouse.mouseaction = EMT_MIDDLE_UP;
	event.Mouse.x = point.x;
	event.Mouse.y = point.y;
	app->OnEvent(event);
	
	mouseDown--;
	updateViewWindow();
	CView::OnMButtonUp(nFlags, point);
}


void CamsdView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_MOUSE;
	event.Mouse.mouseaction = EMT_RIGHT_DOWN;
	event.Mouse.x = point.x;
	event.Mouse.y = point.y;
	app->OnEvent(event);
	
	mouseDown++;
	updateViewWindow();
	CView::OnRButtonDown(nFlags, point);
}


void CamsdView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_MOUSE;
	event.Mouse.mouseaction = EMT_MOUSE_MOVE;
	event.Mouse.x = point.x;
	event.Mouse.y = point.y;
	app->OnEvent(event);
	
	if(mouseDown!=0)
	{
		updateViewWindow();
	}
	CView::OnMouseMove(nFlags, point);
}


BOOL CamsdView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SEvent event;
	event.type = ET_MOUSE;
	event.Mouse.mouseaction = EMT_WHEEL;
	event.Mouse.x = pt.x;
	event.Mouse.y = pt.y;
	event.Mouse.wheel = zDelta;
	app->OnEvent(event);

	updateViewWindow();
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CamsdView::updateViewWindow()
{
	Invalidate(false);
}


void CamsdView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CView::OnPaint()

	wglMakeCurrent(m_hDC,m_hRC);
	//rendering
	RenderScene();
	//flush
	SwapBuffers(m_hDC);
}

//button click event
void CamsdView::OnObjA_LengthAccuracy()
{
	// TODO: 在此添加命令处理程序代码
	SEvent event;
	event.type = ET_USER;
	event.UserData.type = EUT_CALCULATION;
	event.UserData.calType = ECT_OBJ_LENGTH;//first type

	app->OnEvent(event);
	updateViewWindow();
}


void CamsdView::OnTrianglePathLength()
{
	// TODO: 在此添加命令处理程序代码
	SEvent event;
	event.type = ET_USER;
	event.UserData.type = EUT_CALCULATION;
	event.UserData.calType = ECT_TRIANGLE_PATH_LENGTH;

	app->OnEvent(event);
	updateViewWindow();
}


void CamsdView::OnNEQCalculation()
{
	// TODO: 在此添加命令处理程序代码
	SEvent event;
	event.type = ET_USER;
	event.UserData.type = EUT_CALCULATION;
	event.UserData.calType = ECT_NEQ_CALCULATION;

	app->OnEvent(event);
	updateViewWindow();
}


void CamsdView::OnMetalUniformity()
{
	// TODO: 在此添加命令处理程序代码
	SEvent event;
	event.type = ET_USER;
	event.UserData.type = EUT_CALCULATION;
	event.UserData.calType = ECT_METAL_UNIFORMITY;

	app->OnEvent(event);
	updateViewWindow();
}


void CamsdView::OnAMSD_SSP()
{
	// TODO: 在此添加命令处理程序代码
	SEvent event;
	event.type = ET_USER;
	event.UserData.type = EUT_CALCULATION;
	event.UserData.calType = ECT_SSP;

	app->OnEvent(event);
	updateViewWindow();
}




void CamsdView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// ThODO: 在此添加消息处理程序代码和/或调用默认值
	if(app)
		app->PickThreshold = 20;
	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CamsdView::Onsldpickthreshold()
{
	// TODO: 在此添加命令处理程序代码
	//get slider position
	auto pWnd=(CFrameWndEx *)AfxGetApp()->GetMainWnd();
	auto ribbonBar = pWnd->GetRibbonBar();
	
	CMFCRibbonSlider* pSlider = DYNAMIC_DOWNCAST(CMFCRibbonSlider, ribbonBar->FindByID(sldPickThreshold));
 
	f32 ratio = (pSlider->GetPos()-pSlider->GetRangeMin())/(f32)(pSlider->GetRangeMax()-pSlider->GetRangeMin());
	f32 threshold = 1.0f*ratio;
	if(app)
		app->PickThreshold = threshold;
}
