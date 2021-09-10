import tkinter as tk
import tkinter.ttk as ttk
from tkinter import filedialog
from tkinter import messagebox


from PIL import Image, ImageTk
import re
import os

import myimage
import constant
import myframe



class Application(tk.Frame):
    

    def __init__(self, master=None):
        super().__init__(master)
        #なんか作って
        self.master = master
        self.master.title('k2SLEditor')
        #とりあえずウィンドウはこれくらいで
        self.master.geometry("1350x650")
        #ウィンドウ最大可
        #self.master.state('zoomed')
        #色
        self.master.configure(bg=constant.WINDOW_COLOR)
        #これをしないとフレームがどうのこうので
        #placeしても表示されない
        self.pack(expand=1, fill=tk.BOTH)
        #self.pack()
        #キャンバスの初期化
        self.init_canvas()
        #色々初期化
        self.init_various()
        #メニューの初期化
        self.init_menu()
        #ラベルの初期化
        self.init_label()
        #インスペクターウィンドウ？の初期化
        self.init_inspector()
        #プロジェクトウィンドウ？の初期化
        self.init_project()

        #指定した色を透過する
        #self.master.wm_attributes("-transparentcolor", constant.WINDOW_COLOR)
        #読み込んだ画像のリスト
        self.myimage_list = {}

        self.myframe = myframe.MyFrame()

        #セレクトしている画像
        self.number_image = None

        #画像が押された
        self.is_pressed_image = False

        #画像の隅の四角が押された
        self.is_pressed_rect = False

        #pivotが押された
        self.is_pressed_pivot = False

        #どこの隅の画像を押したか
        self.number_rect = None

    #キャンバス座標をtkEngineの座標に変換する
    def convert_canvas_position_to_tk_position(self,position_x,position_y):
        position_x -= constant.ADD_CANVAS_SIZE
        position_y -= constant.ADD_CANVAS_SIZE
        position_x /= constant.CANVAS_SMALLER
        position_y /= constant.CANVAS_SMALLER
        position_x -= constant.CANVAS_WIDTH / 2
        position_y = -position_y
        position_y += constant.CANVAS_HEIGHT / 2
        return position_x,position_y

    #tkEngine座標をキャンバス座標に変換する
    def convert_tk_position_to_canvas_position(self,position_x,position_y):
        position_x += constant.CANVAS_WIDTH / 2
        position_y -= constant.CANVAS_HEIGHT / 2
        position_y = -position_y
        position_x *= constant.CANVAS_SMALLER
        position_y *= constant.CANVAS_SMALLER
        position_x += constant.ADD_CANVAS_SIZE
        position_y += constant.ADD_CANVAS_SIZE
        return position_x,position_y

    #画像の情報をインスペクターウィンドウに反映させる
    def reflect_information_inspector_window(self):
        if self.item_id == None:
            return
        myimg = self.myimage_list[self.item_id]
        #インスペクターウィンドウに情報を反映させる
        #名前
        self.inspector_image_name_entry.delete(0, tk.END)
        self.inspector_image_name_entry.insert(tk.END,myimg.name)

        #座標
        self.inspector_image_position_x_entry.delete(0, tk.END)
        self.inspector_image_position_y_entry.delete(0, tk.END)
        #position = myimg.get_position()
        position = myimg.get_pivot_position()
        #キャンバス座標をtk座標に変換
        position_x,position_y = self.convert_canvas_position_to_tk_position(position[0],position[1])
        self.inspector_image_position_x_entry.insert(tk.END,position_x)
        self.inspector_image_position_y_entry.insert(tk.END,position_y)

        #ピクセル数
        width=myimg.false_width
        height=myimg.false_height
        self.inspector_pixel_size_x_text.set('X : '+str(width))
        self.inspector_pixel_size_y_text.set('Y : '+str(height))

        #スケール
        self.inspector_image_scale_x_entry.delete(0, tk.END)
        self.inspector_image_scale_y_entry.delete(0, tk.END)
        scale=myimg.scale
        self.inspector_image_scale_x_entry.insert(tk.END,scale[0])
        self.inspector_image_scale_y_entry.insert(tk.END,scale[1])

        #レイヤー優先度
        self.inspector_layer_entry.delete(0,tk.END)
        self.inspector_layer_entry.insert(tk.END,myimg.number_layer)

        #ピボット
        pivot = myimg.get_pivot()
        self.inspector_pivot_x_entry.delete(0,tk.END)
        self.inspector_pivot_y_entry.delete(0,tk.END)

        self.inspector_pivot_x_entry.insert(tk.END,pivot[0])
        self.inspector_pivot_y_entry.insert(tk.END,pivot[1])

    #インスペクターウィンドウの情報を空にする
    def enpty_information_inspector_window(self):
        self.inspector_image_name_entry.delete(0, tk.END)

        self.inspector_image_position_x_entry.delete(0, tk.END)
        self.inspector_image_position_y_entry.delete(0, tk.END)

        self.inspector_pixel_size_x_text.set('')
        self.inspector_pixel_size_y_text.set('')

        self.inspector_image_scale_x_entry.delete(0, tk.END)
        self.inspector_image_scale_y_entry.delete(0, tk.END)

        self.inspector_layer_entry.delete(0, tk.END)

        self.inspector_pivot_x_entry.delete(0,tk.END)
        self.inspector_pivot_y_entry.delete(0,tk.END)

    #レイヤーの優先度順に画像を表示する
    def display_images_according_layer_priority(self):
        #レイヤー優先度のリストを作成する
        layer_list = []
        for i in self.myimage_list:
            layer_list.append(self.myimage_list[i].number_layer)
        #優先度を降順に並び変える
        layer_list = sorted(layer_list)
        #レイヤー回して
        for layer in layer_list:
            #イメージ回して
            for j in self.myimage_list:
                #レイヤー一緒だったらraiseする
                if layer == self.myimage_list[j].number_layer:
                    self.myimage_list[j].raise_image(self.canvas)

    #画像が選択されたときの処理
    def select_image(self):
        myimage = None
        try:
        #画像の座標を持ってくる
            self.image_position = self.myimage_list[self.item_id].get_position()
            #画像の座標と大きさを取得する
            image_position_x = self.image_position[0]
            image_position_y = self.image_position[1]
            myimage = self.myimage_list[self.item_id]
        except:
            print(11111)
            return
        #レイヤー優先度順に画像を表示させる
        self.display_images_according_layer_priority()
        #選択した画像を上に持ってくる、今は停止中
        #self.canvas.tag_raise(self.item_id)
        #枠を生成する
        #self.myframe.create_frame(self.canvas,image_position_x,image_position_y,myimage)
        if self.myframe.get_is_rect() == False: 
            self.myframe.create_frame(self.canvas,image_position_x,image_position_y,myimage)
            self.myimage_list[self.item_id].set_pivot_position(self.myframe.get_pivot_position())
        else:
            self.myframe.set_position(self.canvas,image_position_x,image_position_y,myimage)
            self.myframe.set_pivot(self.canvas,image_position_x,image_position_y,myimage)
            self.myimage_list[self.item_id].set_pivot_position(self.myframe.get_pivot_position())

        #枠を削除してから
        self.canvas.delete(self.canvas_rect)
        #720*1280の枠を作る
        self.canvas_rect = self.canvas.create_rectangle(
            constant.ADD_CANVAS_SIZE,
            constant.ADD_CANVAS_SIZE,
            constant.CANVAS_WIDTH * constant.CANVAS_SMALLER + constant.ADD_CANVAS_SIZE,
            constant.CANVAS_HEIGHT * constant.CANVAS_SMALLER + constant.ADD_CANVAS_SIZE,
            outline='blue'
        )

        #インスペクターウィンドウに情報を反映させる
        self.reflect_information_inspector_window()

    #リストボックスを選択したとき
    def select_listbox(self,event):
        number = self.project_list.curselection()
        #何も選択されてなかったら処理しない
        if len(number) == 0:
            return
        number2 = 0
        for i in self.myimage_list:
            if number[0] == number2:
                self.item_id = self.myimage_list[i].item_id
                break
            number2+=1
        self.select_image()
        self.number_image = number[0]
            
    #画像の隅にカーソルが乗ったら、マウスカーソルを変更する
    def enter_rect(self,event):
        #選択された画像を持ってくる
        item_id = self.canvas.find_closest(event.x, event.y)[0]
        item = None
        try:
            #IDからカーソルが乗った枠がどれかを判断する
            number_rect = self.myframe.determine_where_frame_pressed(item_id)
            item=constant.MOUSE_CURSOR_LIST[number_rect]
        except:
            #IDから判断出来なかった場合、座標から判断する
            number_rect = self.myframe.determine_where_frame_pressed_from_position(event.x,event.y)
            item=constant.MOUSE_CURSOR_LIST[number_rect]
    
        #マウスのカーソルを変更する
        self.master.configure(cursor=item)

    #枠からマウスカーソルが離れた時、画像のサイズを変更中でなければ
    #マウスカーソルを元に戻す
    def leave_rect(self,event):
        if self.is_pressed_rect == False:
            #マウスカーソルをデフォルトに戻す
            self.master.configure(cursor=constant.DEFAULT_MOUSE_CURSOR)
    
    #画像の隅に表示している枠をクリックしたときに処理
    def pressed_rect(self,event):
        #クリックした場所を保存
        self.pressed_x = event.x
        self.pressed_y = event.y
        self.is_pressed_rect = True
        #選択された画像を持ってくる
        item_id = self.canvas.find_closest(event.x, event.y)[0]
        self.number_rect = self.myframe.determine_where_frame_pressed(item_id)

    def enter_pivot(self,event):
        #マウスのカーソルを変更する
        self.master.configure(cursor=constant.MOUSE_CURSOR_PIVOT)

    def leave_pivot(self,event):
        if self.is_pressed_pivot == False:
            #マウスカーソルをデフォルトに戻す
            self.master.configure(cursor=constant.DEFAULT_MOUSE_CURSOR)

    def pressed_pivot(self,event):
        self.is_pressed_pivot = True
        #クリックした場所を保存
        self.pressed_x = event.x
        self.pressed_y = event.y

    #画像がクリックされたときの処理
    def pressed(self,event):
        #選択された画像を持ってくる
        self.item_id = self.canvas.find_closest(event.x, event.y)[0]
        #tag = self.canvas.gettags(self.item_id[0])[0]
        #print(item)
        #print(tag)
        #クリックした場所を保存
        self.pressed_x = event.x
        self.pressed_y = event.y
        self.select_image()
        self.project_list.selection_clear(0, tk.END)
        number = 0
        for i in self.myimage_list:
            if self.item_id == self.myimage_list[i].item_id:
                break
            number+=1
        #リストボックスを選択
        self.project_list.select_set(number)
        self.number_image = number
        self.is_pressed_image = True
    
    #スケールを変化させる
    #delta_xとdelta_yはマウスの移動量
    def change_scale(self,delta_x,delta_y):
        try:
            myimg = self.myimage_list[self.item_id]
            #今の画像の大きさ
            width = myimg.width
            height=myimg.height
            #画像の素の大きさ
            image_size=myimg.image_size

            scale_x,scale_y = self.myframe.calculate_scale_image(self.number_rect,myimg,delta_x,delta_y)
            myimg.set_scale([scale_x,scale_y])
            #リサイズするために一旦消してもう一回読み込む
            self.delete_image()
            self.load_image(myimg)
        except:
            pass

    #座標を変化させる
    #delta_xとdelta_yはマウスの移動量
    def change_position(self,delta_x,delta_y):
        img = self.myimage_list[self.item_id]
        position = img.get_position()
        #画像を動かす
        img.set_position(self.canvas,position[0]+delta_x,position[1]+delta_y)
        image_position = img.get_position()
        self.myframe.set_position(self.canvas,
        image_position[0],
        image_position[1],
        img)
        img.set_pivot_position(self.myframe.get_pivot_position())

    #ピボットの座標を変化させる
    #delta_xとdelta_yはマウスの移動量
    def change_pivot_position(self,delta_x,delta_y):
        self.myimage_list[self.item_id].set_pivot(self.myframe.move_pivot(self.canvas,delta_x,delta_y,self.myimage_list[self.item_id]))
        self.myimage_list[self.item_id].set_pivot_position(self.myframe.get_pivot_position())

    #画像がドラッグされたときの処理
    def dragged(self,event):
        #枠が表示されていなかったら、あるいは画像が押されていなかったら
        #あるいは画像の隅の四角形が押されていなかったら
        #画像を動かす処理をしない
        if self.myframe.get_is_rect() == False or (self.is_pressed_image == False and self.is_pressed_rect == False
        and self.is_pressed_pivot == False):
            return
        #tag = self.canvas.gettags(self.item_id[0])[0]
        #item = self.canvas.type(tag) # rectangle image
        #クリックした場所とドラッグした場所の差分を計算
        delta_x = event.x - self.pressed_x
        delta_y = event.y - self.pressed_y
        if self.is_pressed_rect == True:  
            #大きさ変更
            self.change_scale(event.x,event.y)
        elif self.is_pressed_image == True:
            #座標変更
            self.change_position(delta_x,delta_y)
        elif self.is_pressed_pivot == True:
            #ピボットの座標変更
            self.change_pivot_position(delta_x,delta_y)
        self.pressed_x = event.x
        self.pressed_y = event.y

        #インスペクターウィンドウに情報を反映させる
        self.reflect_information_inspector_window()

    #右クリックが終わったとき(ドラッグが終了したとき)
    def mouse_release(self,event):
        #各フラグをオフにする
        self.is_pressed_image = False
        self.is_pressed_rect = False
        self.is_pressed_pivot = False
        self.master.configure(cursor=constant.DEFAULT_MOUSE_CURSOR)

    #ファイル読み込みが選択されたときの処理
    def load_image(self,original_myimg=None):
        
        fn = None
        if original_myimg == None:
            #読み込むファイルの拡張子を指定
            typ = [('png画像','*.png'),
                ('jpg画像','*.jpg'),
                ('tga画像','*.tga'),
                ('dds画像','*.dds'),
                ('bmp画像','*.bmp'),
                ('jpeg画像','*.jpeg'),
                ('ppm画像','*.ppm')
            ]
            #ファイル選択ダイアログを表示
            fn = filedialog.askopenfilename(filetypes=typ)
            #ファイルが選択されてなかったら処理しない
            if len(fn) == 0:
                return
        else:
            fn=original_myimg.file_name
        #画像読み込み
        myimg = myimage.MyImage()
         #複製した画像に複製元の画像の情報をコピーする
        if original_myimg != None:
            myimg.copy_image_infromation(self.canvas,original_myimg)
        myimg.load_image(self.canvas,fn)
        #リストに追加
        self.myimage_list[myimg.item_id] = myimg
        #リストボックスに名前を追加
        self.project_list.insert(tk.END, myimg.name) 
        #レクタングルを設定して、リストボックスも選択する
        self.item_id = myimg.item_id
        self.select_image()
        self.project_list.selection_clear(0, tk.END)
        number = 0
        for i in self.myimage_list:
            if self.item_id == self.myimage_list[i].item_id:
                break
            number+=1
        #リストボックスを選択
        self.project_list.select_set(number)
        self.number_image = number

    def load_level(self):
        #読み込むファイルの拡張子を指定
        typ = [('レベル','*'+constant.FILE_EXTENSION)]
        #ファイル選択ダイアログを表示
        fn = filedialog.askopenfilename(filetypes=typ)
        #ファイルが選択されてなかったら処理しない
        if len(fn) == 0:
            return
        f = open(fn, 'rb')
        data = f.read()
        is_success = True
        #ファイルをオープンする、withでcloseをしなくていいらしい
        with open(fn,'rb') as file:

            #ファイルを読み込んで
            data = file.read()
            #bytes型からstr型に変換
            data = data.decode()
            #\nで分割する
            data = data.split('\n')
            self.delete_all_image(False)
            number = 0
            for image in data:
                number+=1
                #リストの0番目は無視する
                if number == 1:
                    continue
                if image == '': 
                    break
                try:
                    print(image)
                    #空白で分割する
                    image = image.split(',')
                    #インスタンス生成して
                    myimg = myimage.MyImage()
                    #名前入れて
                    myimg.name = image[1]
                    #ファイルパス設定
                    myimg.file_name = image[3]
                    #レイヤー優先度を設定して
                    myimg.number_layer = int(image[8])
                    #スケール設定して
                    scale = [float(image[9]),float(image[10])]
                    myimg.set_scale(scale)
                   
                    #ピボット設定して
                    pivot = [float(image[11]),float(image[12])]
                    myimg.set_pivot(pivot)
                    
                except:
                    messagebox.showerror('エラー', fn + 'の読み込みに失敗しました。')
                    is_success = False
                try:
                    self.load_image(myimg)
                    #座標は画像をロードして大きさが判明した際に、設定する。
                    position_x,position_y = self.convert_tk_position_to_canvas_position(float(image[4]),float(image[5]))
                    position_x,position_y = self.myimage_list[self.item_id].convert_pivot_position_to_image_position(position_x,position_y,pivot)
                    self.myimage_list[self.item_id].set_position(self.canvas,position_x,position_y)
                    self.select_image()
                except:
                    messagebox.showerror('エラー', image[1] + 'が読み込めませんでした、ファイルパスを確認してください。')
                    is_success = False
        if is_success == True:
            messagebox.showinfo('メッセージ', 'レベルの読み込みに成功しました！')
        #レイヤー優先度順に画像を表示させる
        self.display_images_according_layer_priority()
        self.select_image()
        

    #レベルデータを出力する
    def export_level(self):
        if len(self.myimage_list) == 0:
            messagebox.showerror('エラー','画像が配置されていません。')
            return
        #読み込むファイルの拡張子を指定
        typ = [('レベル','*'+constant.FILE_EXTENSION)]
        #ファイル選択ダイアログを表示
        fn = filedialog.asksaveasfilename(filetypes=typ)
        #保存先のファイルが何も選択されてない場合は以下の処理をしない
        if len(fn) == 0:
            return
        if fn.find(constant.FILE_EXTENSION) == -1:
            fn += constant.FILE_EXTENSION
        #ファイルをオープンする、withでcloseをしなくていいらしい
        with open(fn,'wb') as file:
            file.write(bytes((str(len(self.myimage_list)) + '\n').encode()))
            for i in self.myimage_list:
                myimg=self.myimage_list[i]
                #画像の座標を取得
                pivot_position = myimg.get_pivot_position()
                x,y = pivot_position[0],pivot_position[1]
                #画像の座標を書き出す
                #キャンバス座標をtk座標に変換する
                x,y=self.convert_canvas_position_to_tk_position(x,y)
                scale=myimg.scale
                width=myimg.false_width
                height=myimg.false_height
                layer=myimg.number_layer
                pivot=myimg.get_pivot()

                #画像の名前を書き出す
                file.write(bytes((str(len(myimg.name)) + ',').encode()))
                file.write(bytes((str(myimg.name) + ',').encode()))
                #画像のファイルパスを書き出す
                file.write(bytes((str(len(myimg.file_name)) + ',').encode()))
                file.write(bytes((str(myimg.file_name) + ',').encode()))
                #座標を書き出す
                file.write(bytes((str(x) + ',').encode()))
                file.write(bytes((str(y) + ',').encode()))
                #画像のピクセル数を書き出す
                file.write(bytes((str(width) + ',').encode()))
                file.write(bytes((str(height) + ',').encode()))
                #レイヤー優先度を書き出す
                file.write(bytes((str(layer) + ',').encode()))
                #画像のスケールを書き出す
                file.write(bytes((str(scale[0]) + ',').encode()))
                file.write(bytes((str(scale[1]) + ',').encode()))
                #ピボットを書き出す
                file.write(bytes((str(pivot[0]) + ',').encode()))
                file.write(bytes((str(pivot[1]) + ',').encode()))

                #ファイルパスを持ってきて
                #dds_file_path = fn
                #スラッシュで切る
                #slash_number = dds_file_path.rfind('/')
                #dds_file_path = dds_file_path[:slash_number]
                #asset_number = dds_file_path.find("Assets")
                #if asset_number != -1:
                    #dds_file_path = dds_file_path[asset_number:]

                file_name = myimg.file_name
                dot_number = file_name.rfind('.')
                slash_number = file_name.rfind('/')
                file_name = file_name[slash_number+1:dot_number]


                #.ddsを加える
                dds_file_path=file_name + '.dds'

                #.ddsの場合のファイルパスを書き出す
                file.write(bytes((str(len(dds_file_path)) + ',').encode()))
                file.write(bytes((str(dds_file_path) + '').encode()))

        
                file.write(bytes('\n'.encode()))
        
        cmd_file = "mk.bat"   #.batファイルへのパス
        number = fn.rfind('.')
        fn = fn[:number]
        slash_number = fn.rfind('/')
        fn = fn[:slash_number]
        for i in self.myimage_list:
            command = cmd_file
            command += " " + self.myimage_list[i].file_name.replace('/', '\\')
            command += " " + fn.replace('/', '\\')
            os.system(command)

        messagebox.showinfo('メッセージ', '書き出しに成功しました！')

    #マウスが動いた時の処理
    def motion(self,event):
        #マウス座標を取得する
        x,y=self.convert_canvas_position_to_tk_position(event.x,event.y)
        self.label['text'] = 'x : {:.2f}, y : {:.2f}'.format(x,y)
        #self.label['text'] = 'x : {}, y : {}'.format(event.x,event.y)

    #画像を複製する
    def duplicate_image(self):
        number = self.project_list.curselection()
        #何も選択されてなかったら処理をしない
        if len(number) == 0:
            return
        number2 = 0
        myimg = None
        for i in self.myimage_list:
            if number[0] == number2:
                myimg = self.myimage_list[i]
                break
            number2+=1
        self.load_image(myimg)
        
    #画像を削除する
    def delete_image(self,event=None):
        number = self.project_list.curselection()
        #何も選択されてなかったら処理をしない
        if len(number) == 0:
            return
        self.project_list.delete(number)
        number2 = 0
        fn = None
        for i in self.myimage_list:
            if number[0] == number2:
                #画像をキャンバスから削除
                self.canvas.delete(self.myimage_list[i].item_id)
                #リストから削除
                self.myimage_list.pop(i)
                break
            number2+=1
        #枠を削除する
        self.myframe.delete_frame(self.canvas)
        #インスペクターウィンドウの情報を空にする
        self.enpty_information_inspector_window()
        self.item_id = None
        self.number_image = None

    #全ての画像を削除する
    def delete_all_image(self,confirmation = True):
        if len(self.myimage_list) == 0:
            return
        if confirmation == True:
            confirmation = messagebox.askyesno('確認', '本当に画像を全て削除しますか？')
            if confirmation == False:
                return
        
        for i in self.myimage_list:
            #画像をキャンバスから削除
            self.canvas.delete(self.myimage_list[i].item_id)

        #リスト削除
        self.myimage_list.clear()
        #リストボックスを削除
        self.project_list.delete(0, tk.END)
        #インスペクターウィンドウの情報を空にする
        self.enpty_information_inspector_window()
        #枠を削除する
        self.myframe.delete_frame(self.canvas)
        #その他変数を初期化する
        self.item_id = None
        self.number_image = None


        
    #文字列検証関数、文字の入力を数字のみに制限させる
    #Falseで入力拒否
    def validation(self,before_word, after_word):
        if len(after_word) == 0:
            return True
        #elif (after_word.isdecimal()):
        #入力された文字が0～9の半角であれば
        elif re.match(re.compile('[0-9]+'), after_word) or re.match(re.compile('-([0-9]+)'), after_word):
            #self.apply_input_information()
            return True
        else:
            return False

    #文字列検証関数、文字の入力を正数のみに制限させる
    #Falseで入力拒否
    def validation_integer(self,before_word, after_word):
        if len(after_word) == 0:
            return True
        #elif (after_word.isdecimal()):
        #入力された文字が0～9の半角であれば
        elif re.match(re.compile('[0-9]'),after_word) and ('.' in after_word) == False:
            #self.apply_input_information()
            return True
        else:
            return False

    

    #文字列検証関数、文字の入力を0.0から1.0fに限定させる
    #Falseで入力拒否
    def validation_pivot(self,before_word, after_word):
        if len(after_word) == 0:
            return True
        #elif (after_word.isdecimal()):
        #入力された文字が0～9の半角であれば
        elif re.match(re.compile('[0-9]'),after_word):
            #self.apply_input_information()
            num = None
            try:
                num = float(after_word)
                if num < 0.0 and num > 1.0:
                    return False
                else:
                    return True
            except:
                return False
        else:
            return False

    #入力項目が空かどうか判断する
    def determine_input_empty(self,entry):
        if entry == '':
            messagebox.showerror('エラー', '無効な文字、もしくは空の項目があります。')
            return True
        else:
            return False

    #入力された情報を反映させる
    def apply_input_information(self):
        #何も選択されてなかったら処理しない
        if self.number_image == None:
            self.message()
            return
        self.project_list.delete(self.number_image)
        #入力項目が空かどうか判断する
        entry_list = []
        entry_list.append(self.inspector_image_name_entry.get())
        entry_list.append(self.inspector_image_position_x_entry.get())
        entry_list.append(self.inspector_image_position_y_entry.get())
        entry_list.append(self.inspector_layer_entry.get())
        entry_list.append(self.inspector_image_scale_x_entry.get())
        entry_list.append(self.inspector_image_scale_y_entry.get())
        entry_list.append(self.inspector_pivot_x_entry.get())
        entry_list.append(self.inspector_pivot_y_entry.get())
        for entry in entry_list:
            #入力が空であれば以下の処理をしない
            if self.determine_input_empty(entry) == True:
                return

        name = self.inspector_image_name_entry.get()
        self.myimage_list[self.item_id].name = name
        self.project_list.insert(self.number_image, self.myimage_list[self.item_id].name)

        
        #ウィンドウから情報を持ってくる。

        #ピボット
        pivot = [float(self.inspector_pivot_x_entry.get()),float(self.inspector_pivot_y_entry.get())]
        old_position = self.myframe.get_pivot_position()

        #if abs(pivot[0] - self.myframe.pivot[0]) > 0.001 or abs(pivot[1] - self.myframe.pivot[1]):
        self.myimage_list[self.item_id].set_pivot(pivot)
        image_position = self.myimage_list[self.item_id].get_position()
        self.myframe.set_pivot(self.canvas,image_position[0],image_position[1],self.myimage_list[self.item_id])
        self.myimage_list[self.item_id].set_pivot(pivot)
        #old_position = self.myframe.get_pivot_position()
        #self.myframe.set_pivot(self.canvas,position_x,position_y,self.myimage_list[self.item_id])

        #座標
        position_x = float(self.inspector_image_position_x_entry.get())
        position_y = float(self.inspector_image_position_y_entry.get())
        position_x,position_y = self.convert_tk_position_to_canvas_position(position_x,position_y)
        delta_x,delta_y = position_x-old_position[0], position_y - old_position[1]
        self.myframe.set_pivot(self.canvas,image_position[0]+delta_x,image_position[1]+delta_y,self.myimage_list[self.item_id])
        pivot_position = self.myframe.get_pivot_position()

        position_x,position_y = self.myimage_list[self.item_id].convert_pivot_position_to_image_position(pivot_position[0],pivot_position[1],pivot)
        self.myimage_list[self.item_id].set_position(self.canvas,
        position_x,
        position_y)
        
        self.myimage_list[self.item_id].set_scale([float(self.inspector_image_scale_x_entry.get()),
        float(self.inspector_image_scale_y_entry.get())])
        #レイヤー優先度
        self.myimage_list[self.item_id].number_layer = int(self.inspector_layer_entry.get())
        
        #リストボックスを選択
        self.project_list.select_set(self.number_image)
        self.select_image()
        myimg = self.myimage_list[self.item_id]
        #リサイズするために一旦消してもう一回読み込む
        self.delete_image()
        self.load_image(myimg)
        
    def message(self):
        messagebox.showerror('エラー', '画像を選択してください')

    #画像を左上に配置する
    def place_image_upper_left(self):
        if self.item_id == None:
            self.message()
            return
        myimg = self.myimage_list[self.item_id]
        width = myimg.get_width()
        height = myimg.get_height()

        position_x = width / 2 + constant.ADD_CANVAS_SIZE
        position_y = height / 2 + constant.ADD_CANVAS_SIZE
        self.place_image(position_x,position_y)
    
    #画像を左下に配置する
    def place_image_lower_left(self):
        if self.item_id == None:
            self.message()
            return
        myimg = self.myimage_list[self.item_id]
        width = myimg.get_width()
        height = myimg.get_height()

        position_x = width / 2 + constant.ADD_CANVAS_SIZE
        position_y = (constant.CANVAS_HEIGHT * constant.CANVAS_SMALLER) - height / 2 + constant.ADD_CANVAS_SIZE
        self.place_image(position_x,position_y)

    

    #画像を右上に配置する
    def place_image_upper_right(self):
        if self.item_id == None:
            self.message()
            return
        myimg = self.myimage_list[self.item_id]
        width = myimg.get_width()
        height = myimg.get_height()

        position_x = (constant.CANVAS_WIDTH * constant.CANVAS_SMALLER) - width / 2 + constant.ADD_CANVAS_SIZE
        position_y = height / 2 + constant.ADD_CANVAS_SIZE
        self.place_image(position_x,position_y)

    #画像を右下に配置する
    def place_image_lower_right(self):
        if self.item_id == None:
            self.message()
            return
        myimg = self.myimage_list[self.item_id]
        width = myimg.get_width()
        height = myimg.get_height()

        position_x = (constant.CANVAS_WIDTH * constant.CANVAS_SMALLER) - width / 2 + constant.ADD_CANVAS_SIZE
        position_y = (constant.CANVAS_HEIGHT * constant.CANVAS_SMALLER) - height / 2 + constant.ADD_CANVAS_SIZE
        self.place_image(position_x,position_y)

    #画像を中央に配置する
    def place_image_center(self):
        if self.item_id == None:
            self.message()
            return
        myimg = self.myimage_list[self.item_id]
        width = myimg.get_width()
        height = myimg.get_height()

        position_x = constant.CANVAS_WIDTH/2 * constant.CANVAS_SMALLER + constant.ADD_CANVAS_SIZE
        position_y = constant.CANVAS_HEIGHT/2 * constant.CANVAS_SMALLER + constant.ADD_CANVAS_SIZE
        self.place_image(position_x,position_y)

    #画像を上に移動させる
    def move_image_upper(self):
        if self.item_id == None:
            self.message()
            return
        self.move_image(0,-1)

    #画像を下に移動させる
    def move_image_lower(self):
        if self.item_id == None:
            self.message()
            return
        self.move_image(0,1)

    #画像を左に移動させる
    def move_image_left(self):
        if self.item_id == None:
            self.message()
            return
        self.move_image(-1,0)

    #画像を右に移動させる
    def move_image_right(self):
        if self.item_id == None:
            self.message()
            return
        self.move_image(1,0)

    #画像を移動させる
    def move_image(self,width,height):
        myimg = self.myimage_list[self.item_id]
        width = width * myimg.get_width()
        height = height * myimg.get_height()

        myimg.move_position(self.canvas,width,height)
        self.select_image()
    
    #画像を移動させる
    #座標はキャンバス座標
    def place_image(self,position_x,position_y):
        myimg = self.myimage_list[self.item_id]
        myimg.set_position(self.canvas,position_x,position_y)
        self.select_image()

    def change_bg_color_to_black(self):
        self.canvas['bg'] = 'black'

    def change_bg_color_to_gray(self):
        self.canvas['bg'] = 'gray'

    def change_bg_color_to_white(self):
        self.canvas['bg'] = 'white'

    def change_bg_color_to_red(self):
        self.canvas['bg'] = 'red'

    #今は使ってない
    #使うときが来るかもしれない
    def on_resize(self,event):
        # 右下のCanvasをリサイズに合わせて高さを自動調整
        #self.height = self.frame.winfo_height() - 30 # 30 == canvas.height
        width = event.width
        height = event.height
        xx=width-(constant.CANVAS_WIDTH+constant.ADD_CANVAS_SIZE*2)
        #self.canvas.place_forget()
        #self.canvas.place(x=xx,y=0)
        #self.canvas.pack()
        #self.canvas.place_forget()
        #self.canvas.pack(side=tk.LEFT,anchor=tk.NE)


    #キャンバスを初期化
    def init_canvas(self):
        #キャンバス作って
        self.canvas = tk.Canvas(self,
        width=constant.CANVAS_WIDTH*constant.CANVAS_SMALLER+constant.ADD_CANVAS_SIZE*2,
        height=constant.CANVAS_HEIGHT*constant.CANVAS_SMALLER+constant.ADD_CANVAS_SIZE*2,
        bg = constant.WINDOW_COLOR)
        self.canvas.place(x=0, y=0)
        
        #self.canvas.pack(side=tk.LEFT)

        #関数をバインドする
        self.canvas.tag_bind('img', '<ButtonPress-1>', self.pressed)
        self.canvas.tag_bind(constant.MYFRAME_IMAGE_TAG, '<ButtonPress-1>', self.pressed_rect)
        self.canvas.tag_bind(constant.MYFRAME_IMAGE_TAG, '<Enter>', self.enter_rect)
        self.canvas.tag_bind(constant.MYFRAME_IMAGE_TAG, '<Leave>', self.leave_rect)


        self.canvas.tag_bind(constant.MYFRAME_IMAGE_PIVOT_TAG, '<ButtonPress-1>', self.pressed_pivot)
        self.canvas.tag_bind(constant.MYFRAME_IMAGE_PIVOT_TAG, '<Enter>', self.enter_pivot)
        self.canvas.tag_bind(constant.MYFRAME_IMAGE_PIVOT_TAG, '<Leave>', self.leave_pivot)

        
        #マウスの座標を表示したい
        self.canvas.bind('<Motion>', self.motion)
        self.canvas.bind('<B1-Motion>', self.dragged)
        self.canvas.bind('<ButtonRelease-1>', self.mouse_release)

        #Deleteキー押したら削除
        self.canvas.bind_all('<Delete>', self.delete_image)
        
        	
        #720*1280の枠を作る
        self.canvas_rect = self.canvas.create_rectangle(
            constant.ADD_CANVAS_SIZE,
            constant.ADD_CANVAS_SIZE,
            constant.CANVAS_WIDTH * constant.CANVAS_SMALLER + constant.ADD_CANVAS_SIZE,
            constant.CANVAS_HEIGHT * constant.CANVAS_SMALLER + constant.ADD_CANVAS_SIZE,
            outline='blue'
        )
        #キャンバスの色をウィンドウの色と同じにする
        #self.canvas['bg'] = self.master['bg']
        self.canvas['bg'] = constant.CANVAS_COLOR
        

    #色々初期化
    def init_various(self):
        self.pressed_x = pressed_y = 0
        self.item_id = -1
        self.item_id_list = -1

    #メニューを初期化
    def init_menu(self):
        #メニューを生成
        self.mbar = tk.Menu()
        #メニューコマンドを生成
        self.mcom = tk.Menu(self.mbar,tearoff=0)
        self.mcom2 = tk.Menu(self.mbar,tearoff=0)
        self.mcom3 = tk.Menu(self.mbar,tearoff=0)
        self.mcom4 = tk.Menu(self.mbar,tearoff=0)
        #コマンドを追加
        self.mcom.add_command(label='画像読み込み',command=self.load_image)
        self.mcom.add_command(label='全削除',command=self.delete_all_image)
        self.mcom.add_command(label='レベル読み込み',command=self.load_level)
        self.mcom.add_command(label='レベル書き出し',command=self.export_level)
        self.mbar.add_cascade(label='ファイル',menu=self.mcom)
        self.mcom2.add_command(label='左上',command=self.place_image_upper_left)
        self.mcom2.add_command(label='左下',command=self.place_image_lower_left)
        self.mcom2.add_command(label='右上',command=self.place_image_upper_right)
        self.mcom2.add_command(label='右下',command=self.place_image_lower_right)
        self.mcom2.add_command(label='中央',command=self.place_image_center)
        self.mbar.add_cascade(label='配置',menu=self.mcom2)
        self.mcom3.add_command(label='上',command=self.move_image_upper)
        self.mcom3.add_command(label='下',command=self.move_image_lower)
        self.mcom3.add_command(label='左',command=self.move_image_left)
        self.mcom3.add_command(label='右',command=self.move_image_right)
        self.mbar.add_cascade(label='移動',menu=self.mcom3)
        self.mcom4.add_command(label='黒',command=self.change_bg_color_to_black)
        self.mcom4.add_command(label='白',command=self.change_bg_color_to_white)
        self.mcom4.add_command(label='グレー',command=self.change_bg_color_to_gray)
        self.mcom4.add_command(label='赤',command=self.change_bg_color_to_red)
        self.mbar.add_cascade(label='背景色',menu=self.mcom4)
        self.master['menu'] = self.mbar

    #ラベルを初期化
    def init_label(self):
        self.label = tk.Label(self.master,text='x : y :')
        self.label.place(relx=constant.LABEL_RELX, rely=constant.LABEL_RELY)

    #インスペクターウィンドウの名前のラベルを初期化する
    def init_inspector_name_label(self):
        #名前
        inspector_image_name = tk.Label(self.inspector,text='名前')
        inspector_image_name.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_NAME_Y)
        self.inspector_image_name_entry = tk.Entry(self.inspector,width=constant.INSPECTOR_NAME_ENTRY_SIZE_X)
        self.inspector_image_name_entry.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_NAME_ENTRY_Y)


    #インスペクターウィンドウの座標のラベルを初期化する
    def init_inspector_position_label(self):
        inspector_image_name = tk.Label(self.inspector,text='座標')
        inspector_image_name.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_POSITION_Y)

        #X
        inspector_image_position_x = tk.Label(self.inspector,text='x',font=("", "15", ""))
        inspector_image_position_x.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_POSITION_XY_Y)
        #entryを設定
        sv = tk.StringVar()
        self.inspector_image_position_x_entry = tk.Entry(self.inspector,width=constant.INSPECTOR_POSITION_ENTRY_SIZE_X,textvariable=sv)
        self.inspector_image_position_x_entry.place(x=constant.INSPECTOR_POSITION_X_ENTRY_X,y=constant.INSPECTOR_POSITION_ENTRY_Y)
        # %s は変更前文字列, %P は変更後文字列を引数で渡す
        vcmd1 = (self.inspector_image_position_x_entry.register(self.validation), '%s', '%P')
        #Validationコマンドを設定（'key'は文字が入力される毎にイベント発火）
        self.inspector_image_position_x_entry.configure(validate='key', vcmd=vcmd1)

        #y
        inspector_image_position_y = tk.Label(self.inspector,text='y',font=("", "15", ""))
        inspector_image_position_y.place(x=constant.INSPECTOR_POSITION_Y_X,y=constant.INSPECTOR_POSITION_XY_Y)
        #entryを設定
        sv = tk.StringVar()
        self.inspector_image_position_y_entry = tk.Entry(self.inspector,width=constant.INSPECTOR_POSITION_ENTRY_SIZE_X,textvariable=sv)
        self.inspector_image_position_y_entry.place(x=constant.INSPECTOR_POSITION_Y_ENTRY_X,y=constant.INSPECTOR_POSITION_ENTRY_Y)
        # %s は変更前文字列, %P は変更後文字列を引数で渡す
        vcmd2 = (self.inspector_image_position_y_entry.register(self.validation), '%s', '%P')
        #Validationコマンドを設定（'key'は文字が入力される毎にイベント発火）
        self.inspector_image_position_y_entry.configure(validate='key', vcmd=vcmd2)

    #インスペクターウィンドウのスケールのラベルを初期化する
    def init_inspector_scale_label(self):
        inspector_image_scale = tk.Label(self.inspector,text='スケール')
        inspector_image_scale.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_SCALE_Y)

        #X
        inspector_image_scale_x = tk.Label(self.inspector,text='x',font=("", "15", ""))
        inspector_image_scale_x.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_SCALE_XY_Y)
        #entryを設定
        sv = tk.StringVar()
        self.inspector_image_scale_x_entry = tk.Entry(self.inspector,width=constant.INSPECTOR_SCALE_ENTRY_SIZE_X,textvariable=sv)
        self.inspector_image_scale_x_entry.place(x=constant.INSPECTOR_SCALE_X_ENTRY_X,y=constant.INSPECTOR_SCALE_ENTRY_Y)
        # %s は変更前文字列, %P は変更後文字列を引数で渡す
        vcmd1 = (self.inspector_image_scale_x_entry.register(self.validation), '%s', '%P')
        #Validationコマンドを設定（'key'は文字が入力される毎にイベント発火）
        self.inspector_image_scale_x_entry.configure(validate='key', vcmd=vcmd1)

        #y
        inspector_image_scale_y = tk.Label(self.inspector,text='y',font=("", "15", ""))
        inspector_image_scale_y.place(x=constant.INSPECTOR_SCALE_Y_X,y=constant.INSPECTOR_SCALE_XY_Y)
        #entryを設定
        sv = tk.StringVar()
        self.inspector_image_scale_y_entry = tk.Entry(self.inspector,width=constant.INSPECTOR_SCALE_ENTRY_SIZE_X,textvariable=sv)
        self.inspector_image_scale_y_entry.place(x=constant.INSPECTOR_SCALE_Y_ENTRY_X,y=constant.INSPECTOR_SCALE_ENTRY_Y)
        # %s は変更前文字列, %P は変更後文字列を引数で渡す
        vcmd2 = (self.inspector_image_scale_y_entry.register(self.validation), '%s', '%P')
        #Validationコマンドを設定（'key'は文字が入力される毎にイベント発火）
        self.inspector_image_scale_y_entry.configure(validate='key', vcmd=vcmd2)


    #インスペクターウィンドウのピクセルのラベルを初期化する
    def init_inspector_pixel_label(self):
        inspector_pixel_size = tk.Label(self.inspector,text='ピクセル')
        inspector_pixel_size.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_PIXEL_Y)
        self.inspector_pixel_size_x_text = tk.StringVar()
        self.inspector_pixel_size_x_text.set('X : ')
        self.inspector_pixel_size_x = tk.Label(self.inspector,textvariable=self.inspector_pixel_size_x_text,font=('','10'))
        self.inspector_pixel_size_x.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_PIXEL_XY_Y)
        self.inspector_pixel_size_y_text = tk.StringVar()
        self.inspector_pixel_size_y_text.set('Y : ')
        self.inspector_pixel_size_y = tk.Label(self.inspector,textvariable=self.inspector_pixel_size_y_text,font=('','10'))
        self.inspector_pixel_size_y.place(x=constant.INSPECTOR_PIXEL_Y_X,y=constant.INSPECTOR_PIXEL_XY_Y)

    #インスペクターウィンドウのレイヤー優先度のラベルを初期化する
    def init_inspector_layer_label(self):
        inspector_layer = tk.Label(self.inspector,text='レイヤー表示優先度')
        inspector_layer.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_LAYER_Y)
        #entryを設定
        sv = tk.StringVar()
        self.inspector_layer_entry = tk.Entry(self.inspector,width=constant.INSPECTOR_SCALE_ENTRY_SIZE_X,textvariable=sv)
        self.inspector_layer_entry.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_LAYER_ENTRY_Y)
         # %s は変更前文字列, %P は変更後文字列を引数で渡す
        vcmd = (self.inspector_layer_entry.register(self.validation_integer), '%s', '%P')
        #Validationコマンドを設定（'key'は文字が入力される毎にイベント発火）
        self.inspector_layer_entry.configure(validate='key', vcmd=vcmd)

    def init_inspector_pivot_label(self):
        inspector_pivot = tk.Label(self.inspector,text='ピボット')
        inspector_pivot.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_PIVOT_Y)

        #X
        inspector_pivot_x = tk.Label(self.inspector,text='x',font=("", "15", ""))
        inspector_pivot_x.place(x=constant.INSPECTOR_STANDARS_POSITION_X,y=constant.INSPECTOR_PIVOT_XY_Y)
        #entryを設定
        sv = tk.StringVar()
        self.inspector_pivot_x_entry = tk.Entry(self.inspector,width=constant.INSPECTOR_PIVOT_ENTRY_SIZE_X,textvariable=sv)
        self.inspector_pivot_x_entry.place(x=constant.INSPECTOR_PIVOT_X_ENTRY_X,y=constant.INSPECTOR_PIVOT_ENTRY_Y)
        # %s は変更前文字列, %P は変更後文字列を引数で渡す
        vcmd1 = (self.inspector_pivot_x_entry.register(self.validation_pivot), '%s', '%P')
        #Validationコマンドを設定（'key'は文字が入力される毎にイベント発火）
        self.inspector_pivot_x_entry.configure(validate='key', vcmd=vcmd1)

        #y
        inspector_pivot_y = tk.Label(self.inspector,text='y',font=("", "15", ""))
        inspector_pivot_y.place(x=constant.INSPECTOR_PIVOT_Y_X,y=constant.INSPECTOR_PIVOT_XY_Y)
        #entryを設定
        sv = tk.StringVar()
        self.inspector_pivot_y_entry = tk.Entry(self.inspector,width=constant.INSPECTOR_PIVOT_ENTRY_SIZE_X,textvariable=sv)
        self.inspector_pivot_y_entry.place(x=constant.INSPECTOR_PIVOT_Y_ENTRY_X,y=constant.INSPECTOR_PIVOT_ENTRY_Y)
        # %s は変更前文字列, %P は変更後文字列を引数で渡す
        vcmd2 = (self.inspector_pivot_y_entry.register(self.validation_pivot), '%s', '%P')
        #Validationコマンドを設定（'key'は文字が入力される毎にイベント発火）
        self.inspector_pivot_y_entry.configure(validate='key', vcmd=vcmd2)
        return


    #インスペクターウィンドウ？の初期化
    def init_inspector(self):
        #ラベル配置
        self.inspector = tk.Frame(self.master,width=constant.INSPECTOR_WIDTH,height=constant.INSPECTOR_HEIGHT)
        #self.inspector['bg'] = 'white'
        self.inspector.place(relx=constant.INSPECTOR_RELX,rely=constant.INSPECTOR_RELY)

        #名前項目の初期化
        self.init_inspector_name_label()
        
        #座標項目の初期化
        self.init_inspector_position_label()

        #スケール項目の初期化
        self.init_inspector_scale_label()

        #ピクセル項目の初期化
        self.init_inspector_pixel_label()

        #レイヤー表示優先度項目の初期化
        self.init_inspector_layer_label()

        #ピボット項目の初期化
        self.init_inspector_pivot_label()

        self.inspector_button = tk.Button(self.inspector,text='入力項目を反映させる',command=self.apply_input_information,fg='red')
        self.inspector_button.place(x=constant.INSPECTOR_BUTTON_X,y=constant.INSPECTOR_BUTTON_Y)
    
    #プロジェクトウィンドウ？の初期化
    def init_project(self):
        self.project = tk.Frame(self.master)
        self.project_list = tk.Listbox(self.project,
        listvariable=None,
        selectmode='single',
        width=constant.PROJECT_WIDTH,
        height=constant.PROJECT_HEIGHT)
        self.project_list.bind('<<ListboxSelect>>', self.select_listbox)
        self.project.place(relx=constant.INSPECTOR_RELX,
        rely=constant.PROJECT_RELY)


        #フレームを親にスクロールバーを生成
        self.project_bar_y = tk.Scrollbar(self.project,orient=tk.VERTICAL,command=self.project_list.yview)
        #スクロールバーを配置
        self.project_bar_y.pack(side=tk.RIGHT, fill=tk.Y)
        #キャンバスとスクロールバーを紐づける
        self.project_list.config(yscrollcommand=self.project_bar_y.set)
        #なんかよくわからんがスクロールバーが自動で調整される
        self.project_list.see('end')
        #リストを設置
        self.project_list.pack()
        
        #画像の複製ボタンを生成
        self.duplicate_button = ttk.Button(
        self.project,
        text='複製',
        command=self.duplicate_image)
        self.duplicate_button.pack(side=tk.LEFT)

        #画像の削除ボタンを生成
        self.delete_button = ttk.Button(
        self.project,
        text='削除',
        command=self.delete_image)
        self.delete_button.pack(side=tk.LEFT)

        

    
root = tk.Tk()
app = Application(master=root)
app.mainloop()
