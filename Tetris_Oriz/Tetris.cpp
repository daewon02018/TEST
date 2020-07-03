#include "Tetris.h"
#include <stdio.h>
#define REG_NOERROR 1
#define REG_ERROR 0


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MENU);
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
		  CW_USEDEFAULT,CW_USEDEFAULT,416,484,NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);
	
	while(GetMessage(&Message,0,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch(iMessage) {
	case WM_CREATE:
		hDC=GetDC(hWnd);
		CenterWindow(hWnd); //â�� ȭ�� �߾����� ��ġ

		Bitmap = CreateCompatibleBitmap(hDC, 410, 440);
		MemDC = CreateCompatibleDC(hDC);	
		
		MainMemDC=CreateCompatibleDC(hDC);
		BackMemDC=CreateCompatibleDC(hDC);
		NextBackMemDC=CreateCompatibleDC(hDC);
		for(i = 0 ; i < 8 ; i++) 
			BMemDC[i]=CreateCompatibleDC(hDC);
		ReleaseDC(hWnd,hDC);

		MainImg=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_MAIN));
		BackImg=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BACK));
		NextBackImg=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_NEXTBACK));
		BlockImg[0]=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_0));
		BlockImg[1]=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_1));
		BlockImg[2]=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_2));
		BlockImg[3]=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_3));
		BlockImg[4]=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_4));
		BlockImg[5]=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_5));
		BlockImg[6]=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_6));
		BlockImg[7]=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_7));

		SelectObject(MainMemDC,MainImg);
		SelectObject(BackMemDC,BackImg);
		SelectObject(NextBackMemDC,NextBackImg);
		SelectObject(BMemDC[0],BlockImg[0]);
		SelectObject(BMemDC[1],BlockImg[1]);
		SelectObject(BMemDC[2],BlockImg[2]);
		SelectObject(BMemDC[3],BlockImg[3]);
		SelectObject(BMemDC[4],BlockImg[4]);
		SelectObject(BMemDC[5],BlockImg[5]);
		SelectObject(BMemDC[6],BlockImg[6]);
		SelectObject(BMemDC[7],BlockImg[7]);

		IsStart = FALSE;
		IsEnd = FALSE;
		Stage = 0;
		Score = 0;
		Line = 0;

		return 0;
	case WM_COMMAND:
		switch(wParam) {
		case ID_START:
			InitStage();	//�������� �ʱ�ȭ
			PlaySound((LPCSTR) IDR_DROP, g_hInst, SND_ASYNC | SND_RESOURCE);	//�Ҹ� ���
			SetTimer(hWnd,1,Speed,NULL);	//Ÿ�̸�
			randomize();
			NextBlock = random(7);
			NextBlock2 = random(7);
			IsStart = TRUE;
			IsEnd = FALSE;
			DownCheck = FALSE;
			DrawScreen();
			NewBlock();

			EnableMenuItem(GetMenu(hWnd),ID_START,MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
			InvalidateRect(hWnd,NULL,FALSE);
			UpdateWindow(hWnd);
			break;
		case ID_EXIT:
			DeleteDC(MainMemDC);
			DeleteDC(BackMemDC);
			DeleteObject(MainImg);
			DeleteObject(BackImg);
			DeleteObject(NextBackImg);
			for(i = 0 ; i < 8 ; i++){
				DeleteDC(BMemDC[i]);
				DeleteObject(BlockImg[i]);
			}
			DeleteObject(Bitmap);
			DeleteDC(MemDC); 
			KillTimer(hWnd,1);
			PostQuitMessage(0);
			break;
		case ID_About:
			DialogBox(g_hInst,MAKEINTRESOURCE(IDD_ABOUTDIALOG),hWnd,AboutDlgProc);
			break;
		}
		return 0;
	case WM_KEYDOWN:
		if(IsStart){
			switch(wParam) {
			case VK_LEFT:
				MoveLeft();
				break;
			case VK_RIGHT:
				MoveRight();
				break;
			case VK_UP:
				RotateBlock();
				break;
			case VK_DOWN:
				MoveDown();
				break;
			case VK_SPACE:
				MoveBottom();
				
				break;
			}
		}
		return 0;
	case WM_TIMER:
		if(DownCheck) 
			MoveDown();
		else
			NewBlock();
		return 0;
	case WM_PAINT:
		hDC=BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		DrawScreen();
		DrawScore();
		if(IsStart && DownCheck){
			DrawBack();
			DrawNextBlock();
			DrawBlock();
		}
		if(IsEnd){
			DrawBack();
			DrawNextBlock();
		}
		return 0;
	case WM_DESTROY:
		DeleteDC(MainMemDC);
		DeleteDC(BackMemDC);
		DeleteObject(MainImg);
		DeleteObject(BackImg);
		DeleteObject(NextBackImg);
		for(i = 0 ; i < 8 ; i++){
			DeleteDC(BMemDC[i]);
			DeleteObject(BlockImg[i]);
		}
		DeleteObject(Bitmap);
		DeleteDC(MemDC); 
		KillTimer(hWnd,1);
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

BOOL CALLBACK AboutDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
		case WM_INITDIALOG:
			return TRUE;
		case WM_COMMAND:
			switch (wParam)
			{
			case IDOK:
				EndDialog(hDlg,0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

void CenterWindow(HWND hWnd)
{
	RECT rcTemp1, rcTemp2 ;
	LONG iX, iY, iWidth, iHeight ;

	GetWindowRect(GetDesktopWindow(),&rcTemp1) ;
	GetWindowRect(hWnd,&rcTemp2) ;
	iWidth = rcTemp2.right - rcTemp2.left ;
	iHeight = rcTemp2.bottom - rcTemp2.top ;
	iX = LONG((rcTemp1.right - iWidth) / 2) ;
	iY = LONG((rcTemp1.bottom - iHeight) / 2) ;
	MoveWindow(hWnd,iX,iY,iWidth,iHeight,TRUE) ;
}
//�������� �ʱ�ȭ
void InitStage(){
	Stage = 0;
	Score = 0;
	Line = LineLimit[Stage];	//�� ������������ �����ؾ� �ϴ� ���� ��
	Speed = StageSpeed[Stage];	//�� ������������ ��� �������� �ӵ�

	//��ü ��� �迭 �ʱ�ȭ
	for(i = 0 ; i < Rows ; i++){
		for(j = 0 ; j < Cols ; j++){
			//���� ���� ���� ������ ��, ������ ���� �迭�� 9�� ���� ȭ�鿡 ������ �ʴ� ���� �ǹ�
			if(j == 0 || j == Cols - 1 || i == Rows - 1)
				Back[i][j] = 9;
			//�������� 8�� ���� �� ����� �ǹ�
			else
				Back[i][j] = 8;
		}
	}

	//�������� �⺻ ��� ����
	for(i = 0 ; i < Rows - 1 ; i++)
		for(j = 0 ; j < Cols - 2 ; j++)
			Back[i][j + 1] = TBack[Stage][i][j];
}

//��� �׸���
void DrawBlock(){
	hDC=GetDC(hWnd);
	OldBitmap=(HBITMAP)SelectObject(MemDC, Bitmap); 

	for(i = 0 ; i < 8 ; i += 2)
		DrawPiece(Piece[Block][i] + cx, Piece[Block][i+1] + cy);

	BitBlt(hDC,0,0,410,440,MemDC,0,0,SRCCOPY);
	SelectObject(MemDC, OldBitmap); 
	ReleaseDC(hWnd, hDC);
}

void do_tainted_buffer_access(){
    int c, j;
	char buf[10];

    c = getchar(); if (c == EOF) return;
    buf[c*c] = 'a'; /* Tainted Buffer Access warning issued here */

	for(j=0; j<10; j++)
		buf[j+1] = buf[j]/c;
		buf[j+1] = buf[j]/j;
} 

//��� ���� �׸���
void DrawPiece(int x, int y){
	BitBlt(MemDC,20 * x + 150, 20 * y + 20,20,20,BMemDC[Color],0,0,SRCCOPY);
}

//��� �̹����� ���� �׸���
void DrawScreen(){
	hDC=GetDC(hWnd);
	OldBitmap=(HBITMAP)SelectObject(MemDC, Bitmap); 
	BitBlt(MemDC,0,0,410,440,MainMemDC,0,0,SRCCOPY);
	DrawScore();
	BitBlt(hDC,0,0,410,440,MemDC,0,0,SRCCOPY);
	SelectObject(MemDC, OldBitmap); 
	ReleaseDC(hWnd, hDC);
}

//�� ��� �����
void NewBlock(){
	cx = 5; cy = 0;
	Block = NextBlock;		//���� ����� �����Ͽ� ����
	NextBlock = NextBlock2;	//�ٴ��� ����� ���� ��Ͽ� ����
	NextBlock2 = random(7);	//�ٴ��� ��Ͽ� ���� �� ����
	Color = Block;			//���� ����� ������ �ǹ�
	
	DrawBack();				//��� �׸���
	DrawNextBlock();		//���� ��� �׸���
	DrawBlock();			//��� �׸���
	DownCheck = TRUE;		//�Ʒ��� �̵����� ����
}

//��� �׸���
void DrawBack(){
	hDC=GetDC(hWnd);
	OldBitmap=(HBITMAP)SelectObject(MemDC, Bitmap); 
	BitBlt(MemDC,150,20,240,400,BackMemDC,0,0,SRCCOPY);
	DrawAllBlock();
	BitBlt(hDC,0,0,410,440,MemDC,0,0,SRCCOPY);
	SelectObject(MemDC, OldBitmap); 
	ReleaseDC(hWnd, hDC);
}

//��� ����� ��� �׸���
void DrawAllBlock(){
	for(i = 0; i < Rows - 1; i++){
		for(j = 1; j < Cols - 1; j++){
			if(Back[i][j] < 8)
				BitBlt(MemDC,20 * j + 130, 20 * i + 20,20,20,BMemDC[Back[i][j]],0,0,SRCCOPY);
		}
	}
}

//�Ʒ��� �̵�
void MoveDown(){
	//üũ�ؼ� �����ϴٸ� +1 �Ͽ� �̵��Ѵ�
	if (CheckBlock(cx, cy + 1) == 1){
		cy += 1;
		DrawBack();		//���׸���
		DrawBlock();	//��� �׸���
	}
	else{
		SaveBlock();	//��� ����
		DeleteBlock();	//��� ����
		FullCheck();	//������ ������ üũ
		if(DownCheck) NewBlock();	//�Ʒ��� �̵� �������� üũ �Ͽ� �� ����� �׸���.
	}
}

//�ٴ����� �̵�
void MoveBottom(){
	//1�� ���ϸ鼭 üũ�ϸ鼭 �׸��� �� �Ʒ� �ٷ� �̵��Ѵ�.
	while(CheckBlock(cx, cy + 1) == 1){
		cy += 1;
		DrawBack();
		DrawBlock();
		Sleep(10);
	}
	SaveBlock();	//��� ����
	DeleteBlock();	//��� ����
	FullCheck();	//������ ������ üũ
	if(DownCheck) NewBlock();	//�Ʒ��� �̵� �������� üũ �Ͽ� �� ����� �׸���.
}

//�������� �̵�
void MoveLeft(){
	if (CheckBlock(cx - 1, cy) == 1){
		cx -= 1;
		DrawBack();
		DrawBlock();
	}
}

//���������� �̵�
void MoveRight(){
	if (CheckBlock(cx + 1, cy) == 1){
		cx += 1;
		DrawBack();
		DrawBlock();
	}
}

//������ ������ üũ
void FullCheck(){
	//���� ���� ���� 7�� �迭�� ���� ������� ���� ��� ������ ���������� ����
	if(Back[0][7] < 8){
		IsStart = FALSE;
		IsEnd = TRUE;
		DownCheck = FALSE;
		KillTimer(hWnd,1);

		hDC=GetDC(hWnd);
		OldBitmap=(HBITMAP)SelectObject(MemDC, Bitmap); 

		//������ ������ �Ʒ������� ���� ���� ����� ��ϵ��� ������ �������� ȸ��(7)���� ��ȭ��Ų��
		for(i = Rows - 1 ; i >= 0 ; i--){
			for(j = 1; j < Cols - 1; j++){
				if(Back[i][j] < 8){
					Back[i][j] = 7;
					BitBlt(MemDC,20 * j + 130, 20 * i + 20,20,20,BMemDC[Back[i][j]],0,0,SRCCOPY);
				}
			}
			BitBlt(hDC,0,0,410,440,MemDC,0,0,SRCCOPY);
			Sleep(40);
		}
		SelectObject(MemDC, OldBitmap); 
		ReleaseDC(hWnd, hDC);

		DrawBack();
		DrawNextBlock();

		if (MessageBox(hWnd,"The Game is Over!!!        \n\nRestart Now?", "TETRIS", MB_YESNO | MB_ICONQUESTION)==IDYES)
			SendMessage(hWnd,WM_COMMAND,ID_START,NULL);
		else 
			EnableMenuItem(GetMenu(hWnd),ID_START,MF_BYCOMMAND | MF_ENABLED);
	}
}

//��� ����
void SaveBlock(){
	//���� ��ũ ������ ��� project > settings > link tab > winmm.lib �߰�
	//PlaySound((LPCSTR) IDR_ROTATE, g_hInst, SND_ASYNC | SND_RESOURCE);
	for(i = 0 ; i < 8 ; i += 2)
		Back[cy + Piece[Block][i+1]][cx + Piece[Block][i] + 1] = Color;
}

//���� �׸���
void DrawScore(){
	HFONT Font, OldFont;
	Font=CreateFont(20,0,0,0,0,0,0,0,ANSI_CHARSET,0,0,0,0,"Times New Roman");
	OldFont=(HFONT)SelectObject(MemDC,Font);
	SetTextColor(MemDC,RGB(255,255,255));
	SetBkColor(MemDC,RGB(12,12,12));
	wsprintf(StageStr,"Stage : %d  ", Stage + 1);
	TextOut(MemDC, 22,360,StageStr,strlen(StageStr));
	wsprintf(LineStr,"Line : %d  ", Line);
	TextOut(MemDC, 22,380,LineStr,strlen(LineStr));
	wsprintf(ScoreStr,"Score : %d  ", Score);
	TextOut(MemDC, 22,400,ScoreStr,strlen(ScoreStr));
	SelectObject(MemDC,OldFont);
	DeleteObject(Font);
}

//��� ����
void DeleteBlock(){
	Score += 5;	//���� �߰�
	hDC=GetDC(hWnd);
	OldBitmap=(HBITMAP)SelectObject(MemDC, Bitmap); 
	BitBlt(MemDC,150,20,240,400,BackMemDC,0,0,SRCCOPY);
	DrawAllBlock();
	
	PlaySound((LPCSTR) IDR_DROP, g_hInst, SND_ASYNC | SND_RESOURCE); //�Ҹ� ���

	int Count;
	int LineCount = 0;
	//���� ����� ��ġ�� �������� 4�ٸ� �Ųٷ� �Ʒ������� ���� ����Ͽ� ��� �迭�� ����
	for(i = 3 ; i >= 0 ; i--){
		Count = 0;
		//�� �࿡ ����� �ִ��� üũ
		for(j = 1 ; j < Cols - 1 ; j++){
			if(Back[cy + Piece[Block][i * 2 + 1]][j] < 8){
				Count++;
			}
		}
		
		//12���� ��� á���� ��� �迭�� 7�� ����(ȸ��)
		if(Count == 12){
			for(j = 1 ; j < Cols - 1 ; j++){
				if(Back[cy + Piece[Block][i * 2 + 1]][j] < 8){
					Back[cy + Piece[Block][i * 2 + 1]][j] = 7;
					BitBlt(MemDC,20 * j + 130, 20 * (cy + Piece[Block][i * 2 + 1]) + 20,20,20,BMemDC[Back[cy + Piece[Block][i * 2 + 1]][j]],0,0,SRCCOPY);
				}
			}
			BitBlt(hDC,0,0,410,440,MemDC,0,0,SRCCOPY);
			Sleep(50);
		}
	}

	//���� ����� ��ġ�� �������� ����� �����Ͽ� �Ʒ��� ���������� �Ѵ�
	for(i = 0 ; i < 4 ; i++){
		Count = 0;
		//�� �࿡ ����� �ִ��� üũ
		for(j = 1 ; j < Cols - 1 ; j++){
			if(Back[cy + Piece[Block][i * 2 + 1]][j] < 8){
				Count++;
			}
		}
		//12���� ��� á���� 
		if(Count == 12){
			//����� �ӽ� ����
			for(j = 0 ; j < Rows - 1 ; j++)
				for(k = 1 ; k < Cols - 1; k++)
					TempBack[j][k] = Back[j][k];
			//���� ����� ���� ��� ��ϵ��� �Ʒ��� ������.
			//���� ���ӿ��� ����� ���ŵǾ� �Ʒ��� �������� �κ�
			for(j = 0 ; j < cy + Piece[Block][i * 2 + 1]; j++)
				for(k = 1 ; k < Cols - 1 ; k++)
					Back[j + 1][k] = TempBack[j][k];

			//���� ���� ���� 8�� �ʱ�ȭ, ������ ���ذ� �ȵ����� �Ƹ��� ���װ� �־ �־��������� ������ ��
			for(j = 1; j < Cols - 1; j++)
				Back[0][j] = 8;		

			Line--;
			if(Line < 0) Line = 0;
			Score += 100;
			LineCount += 1;
		}
	}
	
	DrawScore();
	BitBlt(hDC,0,0,410,440,MemDC,0,0,SRCCOPY);
	SelectObject(MemDC, OldBitmap); 
	ReleaseDC(hWnd, hDC);

	StageCheck(LineCount);
}

//���������� �������� üũ
void StageCheck(int LineCount){
	//3���� ������ ��� �߰�����
	if(LineCount == 3){
		Score += 100;
	}
	//4���� ������ ��� �߰�����
	else if(LineCount == 4){
		Score += 200;
	}

	if(Line <= 0){
		DrawBack();
		//11�� ���������� ������
		if(Stage == 11){
			IsStart = FALSE;
			IsEnd = TRUE;
			DownCheck = FALSE;
			KillTimer(hWnd,1);

			if (MessageBox(hWnd,"�١� Congratulation !!! �ڡ�        \n\nAll Stages Cleared.\n\nRestart Again???", "TETRIS", MB_YESNO | MB_ICONQUESTION)==IDYES)
				SendMessage(hWnd,WM_COMMAND,ID_START,NULL);
			else 
				EnableMenuItem(GetMenu(hWnd),ID_START,MF_BYCOMMAND | MF_ENABLED);
		}
		//������ ���������� �ƴϸ�
		else{
			Stage++;					//�������� ����
			Speed = StageSpeed[Stage];	//�ӵ� ����
			Line = LineLimit[Stage];	//�����ؾ��� ���� ����

			//���� �������� ����
			for(i = 0 ; i < Rows - 1 ; i++)
				for(j = 0 ; j < Cols - 2 ; j++)
					Back[i][j + 1] = TBack[Stage][i][j];

			StageClear();	//�������� Ŭ����
		}
	}
}

//�������� Ŭ����
void StageClear(){
	KillTimer(hWnd,1);

	if(Stage != 11 && MessageBox(hWnd,"Stage Clear!!!", "TETRIS", MB_OK)){
		SetTimer(hWnd,1,Speed,NULL);
		DrawScreen();
	}
}

//���� ��� �׸���
void DrawNextBlock(){
	hDC=GetDC(hWnd);
	OldBitmap=(HBITMAP)SelectObject(MemDC, Bitmap); 
	BitBlt(MemDC,20,20,110,80,NextBackMemDC,0,0,SRCCOPY);
	BitBlt(MemDC,20,120,110,80,NextBackMemDC,0,0,SRCCOPY);

	//���� ���
	for(i = 0 ; i < 8 ; i += 2)
			DrawNextPiece(Piece[NextBlock][i], Piece[NextBlock][i+1], 40, 1);
	
	//�ٴ��� ���
	for(i = 0 ; i < 8 ; i += 2)
			DrawNextPiece(Piece[NextBlock2][i], Piece[NextBlock2][i+1], 140, 2);
	
	BitBlt(hDC,0,0,410,440,MemDC,0,0,SRCCOPY);
	SelectObject(MemDC, OldBitmap); 
	ReleaseDC(hWnd, hDC);
}

//���� ��� ���� �׸���
void DrawNextPiece(int x, int y, int sy, int nextblock){
	int px, py;

	//���� ����� 1�ϰ�� ���� ��ġ ����
	if(nextblock == 1) {
		if(NextBlock == 0) {
			px = 35; py = sy + 10;
		}
		else if(NextBlock == 6){
			px = 55; py = sy;
		}
		else{
			px = 45; py = sy;
		}
		BitBlt(MemDC,20 * x + px, 20 * y + py,20,20,BMemDC[NextBlock],0,0,SRCCOPY);
	}
	//������ ��ϵ��� ��ġ ����
	else {
		if(NextBlock2 == 0) {
			px = 35; py = sy + 10;
		}
		else if(NextBlock2 == 6){
			px = 55; py = sy;
		}
		else{
			px = 45; py = sy;
		}
		BitBlt(MemDC,20 * x + px, 20 * y + py,20,20,BMemDC[7],0,0,SRCCOPY);
	}
}

//��� üũ, �̵��� x y ���� �޾Ƽ� �̵� �������� üũ�Ѵ�
int CheckBlock(int x, int y){
	Cnt = 0;
	for(i = 0 ; i < 8 ; i += 2){
		Chk = Back[y + Piece[Block][i+1]][x + Piece[Block][i] + 1];
		
		if (Chk != 8)
			Cnt++;
	}

	if (Cnt > 0)
		return 0;
	else
		return 1;
}

//��� ȸ��
void RotateBlock(){
	Temp = Block;	//��� �ӽ� ����
	Temp_cx = cx;	//���� x�� ����
	Temp_cy = cy;	//���� y�� ����
	Block = CheckRotate(Block);	//ȸ�� üũ�Ͽ� ȸ�� �� ������ ����� �����Ѵ�

	//ȸ���� ������ ��ġ �� ��� �׸���
	if(CheckBlock(cx, cy) == 1){
		DrawBack();
		DrawBlock();
	}
	else{
		//ȸ���� �������� �ʴٸ� �����س��� ��ġ�� �̵�
		cx = Temp_cx;
		cy = Temp_cy;
		Block = Temp;
	}
}

//ȸ�� üũ
int CheckRotate(int Block){
	if(Block == 0){
		if(cy < 17){
			Block = 7;
			if(cx == 8) cx = 11;
		}
	}
	else if(Block == 1){
		Block = 8;
	}
	else if(Block == 2){
		Block = 11;
	}
	else if(Block == 3){
		Block = 12;
	}
	else if(Block == 4){
		Block = 13;
	}
	else if(Block == 5){
		Block = 16;
	}
	else if(Block == 6){
		Block = 6;
	}
	else if(Block == 7){
		Block = 0;
		if(cx == 11 || cx == 10 || cx == 9) cx = 8;
	}
	else if(Block == 8){
		Block = 9;
		if(cx == 10) cx = 9;
	}
	else if(Block == 9){
		Block = 10;
	}
	else if(Block == 10){
		Block = 1;
	}
	else if(Block == 11){
		Block = 2;
		if(cx == 10) cx = 9;
	}
	else if(Block == 12){
		Block = 3;
		if(cx == 10) cx = 9;
	}
	else if(Block == 13){
		Block = 14;
		if(cx == 10) cx = 9;
	}
	else if(Block == 14){
		Block = 15;
	}
	else if(Block == 15){
		Block = 4;
		if(cx == 10) cx = 9;
	}
	else if(Block == 16){
		Block = 17;
		if(cx == 10) cx = 9;
	}
	else if(Block == 17){
		Block = 18;
	}
	else if(Block == 18){
		Block = 5;
		if(cx == 10) cx = 9;
	}

	return(Block);
}

void copy_node(char* rest)
{
	char buf[10];
	int length;

	if(rest != NULL)
		length = strlen(rest);

	if(length < 10)
		strncpy(buf, rest, length);

}

static void recursion(int* depth)
{
	float advance;
	*depth = *depth + 1 ;
	advance = 1.0f /(float)(*depth);

	if(*depth < 50)
		recursion(depth);
}

char* acquire_state(int *err, int nodes)
{
	char* node = NULL;
	if(nodes == 0)
	{
		*err = REG_NOERROR;
		return NULL;
	}
	else
	{
		node = (char*)malloc((sizeof(char)*nodes)); 
		if(node != NULL)
			*err = REG_NOERROR;
		else
			*err = REG_ERROR;

		return node;
	}
}


void my_main()
{
	int init = 0;
	int i=0;
	float x=0.0, div=0.0;
	while (i<10) {
		if (init) {
			x+=x/div;
		}
		else {
			init = 1;
			x = 1.0;
			div = 2.0;
		}
	i++;
	}
}

char add_src_nodes(int nodes, int b)
{
	//int* a;
	//a = (int*)malloc(4);
//	int acquire_err = REG_NOERROR;
	int acquire_err = 300;
	char* value = (char*)malloc(sizeof(char));
	char* state = acquire_state(&acquire_err, nodes);

	if(acquire_err != REG_NOERROR)
	{
		printf("REG_ERROR_OCCURED");
		return acquire_err;
	}

	if(*state !=NULL)
		memcpy(value, state, sizeof(state));


	unsigned long u32r = 100;
	unsigned long u32a = 500;
	
	my_main();
	printf("Before : u32r = %l, u32a = %l", u32r, u32a);

	u32r = -u32a;

	printf("After : u32r = %l, u32a = %l", u32r, u32a);

	return acquire_err;

}

