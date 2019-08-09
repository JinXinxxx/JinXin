
// OpenFileView.cpp: COpenFileView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "OpenFile.h"
#endif

#include "OpenFileDoc.h"
#include "OpenFileView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenFileView

IMPLEMENT_DYNCREATE(COpenFileView, CView)

BEGIN_MESSAGE_MAP(COpenFileView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// COpenFileView 构造/析构

COpenFileView::COpenFileView() noexcept
{
	// TODO: 在此处添加构造代码

}

COpenFileView::~COpenFileView()
{
}

BOOL COpenFileView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// COpenFileView 绘图

void COpenFileView::OnDraw(CDC* /*pDC*/)
{
	COpenFileDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// COpenFileView 打印

BOOL COpenFileView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void COpenFileView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void COpenFileView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// COpenFileView 诊断

#ifdef _DEBUG
void COpenFileView::AssertValid() const
{
	CView::AssertValid();
}

void COpenFileView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenFileDoc* COpenFileView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenFileDoc)));
	return (COpenFileDoc*)m_pDocument;
}
#endif //_DEBUG


// COpenFileView 消息处理程序
