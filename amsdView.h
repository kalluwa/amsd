// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// amsdView.h : CamsdView ��Ľӿ�
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
protected: // �������л�����
	CamsdView();
	DECLARE_DYNCREATE(CamsdView)

// ����
public:
	CamsdDoc* GetDocument() const;

// ����
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
// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// ʵ��
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
// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // amsdView.cpp �еĵ��԰汾
inline CamsdDoc* CamsdView::GetDocument() const
   { return reinterpret_cast<CamsdDoc*>(m_pDocument); }
#endif

