#pragma once

class GraphicsEngine;
extern HWND			g_hWnd ;				//ウィンドウハンドル。

//ゲームの初期化。
void InitGame(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow, const TCHAR* appName);
//ウィンドウメッセージをディスパッチ。falseが返ってきたら、ゲーム終了。
bool DispatchWindowMessage();
// k2Engineの初期化
void InitK2Engine(bool isSoftShadow);
// k2Engineの処理を実行。
void ExecuteK2Engine();
// k2Engineの終了処理を実行。
void FinalK2Engine();