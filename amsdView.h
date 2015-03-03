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

// amsdView.h : CamsdView 类的接口
//

#pragma once

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glu32.lib")

#include <GL/glew.h>
#include "gl/gl.h"
#include "gl/glu.h"
//driver
#include "Src/core/core.h"
#include "Src/Driver/COpenGLDriver.h"
//application
#include "App.h"

class CamsdView : public CView
{
protected: // 仅从序列化创建
	CamsdView();
	DECLARE_DYNCREATE(CamsdView)

// 特性
public:
	CamsdDoc* GetDocument() const;

// 操作
public:
		//draw object with opengl
	virtual void RenderScene();

private:
	//create opengl device
	void glInit();
	//remove all opengl variables
	void glRelease();
	//change viewport
	void glResize(int x,int y);
	//set up gl state
	void setupRC();

	//update view window
	void updateViewWindow();
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CamsdView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HGLRC m_hRC;
	HDC m_hDC;

	IApp* app;
	//for key
	bool keyShift,keyControl;
	int keyDown,mouseDown;
	bool updateViewInMouseMove;
// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
	afx_msg void OnObjA_LengthAccuracy();
};

#ifndef _DEBUG  // amsdView.cpp 中的调试版本
inline CamsdDoc* CamsdView::GetDocument() const
   { return reinterpret_cast<CamsdDoc*>(m_pDocument); }
#endif

