
Add-Type -Assembly System.Windows.Forms



function CopyAndCreateShortCut($maxVersion)
{
    # maxスクリプトをコピー。
    #コピー元のフォルダのパス
    $copySrcFolder = "3dsMaxScripts\*"
    #コピー先のフォルダのパス
    $copyDstFolder = $copyDstFolderPrefix + "\" + $maxVersion + " - 64bit\JPN\scripts"
    

    if( Test-path $copyDstFolder){
        #コピー先のフォルダがあれば。
        #フォルダの内容をコピー。
        Copy-Item $copySrcFolder -Destination $copyDstFolder -Recurse -Force
       
        #デスクトップにショートカットを作る。
        $shell = New-Object -ComObject WScript.shell

        #ショートカットへのオブジェクトを作成
        $lnk = $shell.CreateShortcut($desktop + "\tkExporter For 3dsMax " + $maxVersion + ".lnk")
 
        #リンク先パス設定
        $lnk.TargetPath = $copyDstFolder + "\tkExporter.ms"
        
        $lnk.WorkingDirectory = $desktopFolder
 
        #ショートカットを保存
        $lnk.Save()
    }

    # maxプラグインをコピー。
    $copySrcFolder = "3dsMaxPlugin\*"


    #コピー先のフォルダのパス
    $copyDstFolderPrefix = "C:\Program Files (x86)\Autodesk\3ds Max "
    $copyDstFolder = $copyDstFolderPrefix + $maxVersion + "\Plugins"
    if( Test-path $copyDstFolder){
        #コピー先のフォルダがあれば。
        #フォルダの内容をコピー。
        Copy-Item $copySrcFolder -Destination $copyDstFolder -Recurse -Force
    }

    $copyDstFolderPrefix = "C:\Autodesk\3ds Max "
    $copyDstFolder = $copyDstFolderPrefix + $maxVersion + "\Plugins"
    if( Test-path $copyDstFolder){
        #コピー先のフォルダがあれば。
        #フォルダの内容をコピー。
        Copy-Item $copySrcFolder -Destination $copyDstFolder -Recurse -Force
    }
}

Start-Process -FilePath make-3.81.exe -Wait



#アプリケーションデータのフォルダを取得。
$appData = [Environment]::GetFolderPath("ApplicationData")
#Roamingを置き変えてLocalに変更。
$appData = $appData.Replace("Roaming", "Local")
#コピー先のフォルダを作成。
$copyDstFolderPrefix = $appData + "\Autodesk\3dsMax"

#デスクトップフォルダを取得
$desktop = [Environment]::GetFolderPath("Desktop")

CopyAndCreateShortCut("2017")
CopyAndCreateShortCut("2018")
CopyAndCreateShortCut("2019")
CopyAndCreateShortCut("2020")
CopyAndCreateShortCut("2021")
CopyAndCreateShortCut("2022")
CopyAndCreateShortCut("2023")
CopyAndCreateShortCut("2024")

# 続いてシェーダーのショートカットをデスクトップに作成。
$shell = New-Object -ComObject WScript.shell
$lnk = $shell.CreateShortcut($desktop + "\k2EngineShader For 3dsMax " + ".lnk")
$currentDir = Get-Location 

$lnk.TargetPath = $currentDir.Path + "\3dsMaxShader" + "\k2EngineShader.fx"

#ショートカットを保存
$lnk.Save()

#ユーザーパスにmake.exeまでのパスを追加する。
$oldUserPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
$oldUserPath += ";C:\Program Files (x86)\GnuWin32\bin"
[System.Environment]::SetEnvironmentVariable("Path", $oldUserPath, "User")

[System.Windows.Forms.MessageBox]::Show('セットアップ完了。PCを再起動してください。')


