// tkmOptimizer.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "k2EngineLowPreCompile.h"
#include <iostream>

using namespace nsK2EngineLow;

// コマンドライン引数
enum Arg {
    Arg_Exefilename,                // 実行ファイル名。
    Arg_InputTkmFilePath,           // 入力tkmファイルパス。
    Arg_OutputTkmFilePath,          // 出力tkmファイルパス。
    CommonArg_Num,                  // 共通引数の数。
    Arg_Options = CommonArg_Num,    // ここからオプション。
};
int main( int argc, char* argv[] )
{
    if (argc < CommonArg_Num) {
        printf("tkmファイルの最適化ツール\n");
        printf("Usage : tkmOptimizer <入力tkmファイルパス> <出力tkmファイルパス> [option]\n");
        printf("    <入力tkmファイルパス>    最適化前のtkmファイルのパス\n");
        printf("    <出力tkmファイルパス>    最適化後のtkmファイルの出力パス\n");
        return 0 ;
    }   
    // k2Engineを初期化する。
    K2EngineLow engine;
    engine.Init(nullptr, 0, 0);
    // コマンドライン引数を解析
    std::string inputTkmFilePath, outputTkmFilePath;
    // 入力ファイルパスと出力ファイルパスを取得。
    inputTkmFilePath = argv[Arg_InputTkmFilePath];
    outputTkmFilePath = argv[Arg_OutputTkmFilePath];
    
    // tkmファイルをロード。
    TkmFile tkmFile;
    if (tkmFile.Load(inputTkmFilePath.c_str(), true, false, true) == false) {
        // エラー。
        return 1;
    }
    // 最適化されたtkmファイルを保存する。
    if (tkmFile.Save(outputTkmFilePath.c_str()) == false) {
        // エラー。
        return 1;
    }
    return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
