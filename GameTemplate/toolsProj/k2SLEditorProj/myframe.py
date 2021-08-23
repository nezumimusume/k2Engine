from pathlib import WindowsPath
import tkinter as tk
import tkinter.ttk as ttk
from tkinter import filedialog
from tkinter import messagebox
from PIL import Image, ImageTk
import re
import glob
import sys

import myimage
import constant

#画像を覆うように枠を表示するクラス
class MyFrame():
    def __init__(self):
        #枠
        self.rect = None
        self.rect_list = {}
        #最初から、上　下　左　右　左上　右下　右上　左下
        self.myimage_position_list = {
            0 : [0.0,-1/2, 0,-1],
            1 : [0.0,1/2 ,0,1],
            2 : [-1/2,0.0, -1,0],
            3 : [1/2,0.0, 1,0],
            4 : [-1/2,-1/2, -1,-1],
            5 : [1/2,1/2, 1,1],
            6 : [-1/2,1/2, -1,1],
            7 : [1/2,-1/2, 1,-1]
        }
        self.myimage_id_list = None
        self.position = [0.0,0.0]
        self.position1_x = None
        self.position1_y = None
        self.position2_x = None
        self.position2_y = None
        self.color_move = 'red'
        self.color_scale = 'blue'


        self.is_move = False

        self.pivot_image = None

        self.pivot = [0.5,0.5]

        self.pivot_position = [0.0,0.0]

    #8つの枠のうち、どこの枠をクリックしたのか判断する
    def determine_where_frame_pressed(self,item_id):
        for i in self.rect_list:
            if self.rect_list[i].item_id == item_id:
                return i 
    
    #座標からどこの枠が選択されたのか判断する
    #座標はキャンバス座標
    def determine_where_frame_pressed_from_position(self,position_x,position_y):
        number = 0
        #float型の最大値を代入する
        min_diff = sys.float_info.max   
        for i in self.rect_list:
            rect_position = self.rect_list[i].get_position()
            #マウスの座標と、各枠の座標の距離を求める
            diff = abs(rect_position[0] - position_x) + abs(rect_position[1] - position_y)
            #距離が一番小さい枠の番号を求める
            if min_diff > diff:
                min_diff = diff
                number =i
        return number

    def calculate_scale_image(self,number_rect,myimage,delta_x,delta_y):
        
        #delta_x /= constant.CANVAS_SMALLER
        #delta_y /= constant.CANVAS_SMALLER

        #現在の大きさ(実際の大きさ)
        width = myimage.get_width()
        height = myimage.get_height()
        #画像の元の大きさ
        size_x = myimage.image_size[0]
        size_y = myimage.image_size[1]
        size_x = float(size_x)
        size_y = float(size_y)
        size_x *= constant.CANVAS_SMALLER
        size_y *= constant.CANVAS_SMALLER

        size_x = int(size_x)
        size_y = int(size_y)

        #上下左右の枠が押されたいた時
        if number_rect < len(self.rect_list) / 2:
            scale_x = myimage.scale[0]
            scale_y = myimage.scale[1]
            position = myimage.get_position()
            if self.myimage_position_list[number_rect][2] != 0:
                x = delta_x - position[0]
                x *= 2.0
                x *= self.myimage_position_list[number_rect][2]
                scale_x = x / size_x 
            if self.myimage_position_list[number_rect][3] != 0:
                y = delta_y - position[1]
                y *= 2.0
                y *= self.myimage_position_list[number_rect][3]
                scale_y = y / size_y
            return scale_x, scale_y
        else:
            scale_x = myimage.scale[0]
            scale_y = myimage.scale[1]
            position = myimage.get_position()
            x = delta_x - position[0]
            x *= 2.0
            x *= self.myimage_position_list[number_rect][2]
            scale_x = x / size_x 
            y = delta_y - position[1]
            y *= 2.0
            y *= self.myimage_position_list[number_rect][3]
            scale_y = y / size_y
            scale = (scale_x + scale_y) / 2
            return scale, scale

    #座標を取得
    #それぞれ左上と右下のxとy
    def get_position(self):
        return self.position1_x,self.position1_y,self.position2_x,self.position2_y

    
    def get_pivot_position(self):
        return self.pivot_position

    #枠を動かす
    def set_position(self,canvas,position_x,position_y,myimg):
        if self.rect == None:
            return
        
        width = float(myimg.get_width())
        height = float(myimg.get_height())
        self.position1_x = position_x - width / 2 - constant.ADD_FRAME_SIZE
        self.position1_y = position_y + height / 2 + constant.ADD_FRAME_SIZE
        self.position2_x = position_x + width / 2 + constant.ADD_FRAME_SIZE
        self.position2_y = position_y - height / 2 - constant.ADD_FRAME_SIZE
        canvas.coords(self.rect,
        self.position1_x,
        self.position1_y,
        self.position2_x,
        self.position2_y,)
        canvas.tag_raise(self.rect)

        self.move_rect(canvas,position_x,position_y,myimg)

    def get_is_rect(self):
        return self.rect != None

    def move_rect(self,canvas,position_x,position_y,myimg):
        width = float(myimg.get_width())
        height = float(myimg.get_height())

        for i in self.rect_list:
            rect = self.rect_list[i]
            pos_x = position_x + width * self.myimage_position_list[i][0]
            pos_y = position_y + height * self.myimage_position_list[i][1]
            #枠は画像よりちょっと大き目なのでそれを考慮した座標を指定する
            rect.set_position(canvas,
            pos_x + constant.ADD_FRAME_SIZE * self.myimage_position_list[i][2],
            pos_y + constant.ADD_FRAME_SIZE * self.myimage_position_list[i][3]
            )
            canvas.tag_raise(rect.item_id)

        self.set_pivot(canvas,position_x,position_y,myimg)
        canvas.tag_raise(self.pivot_image.item_id)


    def set_pivot(self,canvas,image_position_x,image_position_y,myimg):
     
        #現在選択している画像の、座標、横幅、縦幅を取得する。
        position_x,position_y = image_position_x,image_position_y
        width = float(myimg.get_width())
        height = float(myimg.get_height())
        
        left = position_x - width / 2

        top = position_y + height / 2

        pivot = myimg.get_pivot()

        pivot_position_x = width * pivot[0] + left
        pivot_position_y = -height * pivot[1] + top

        self.pivot_image.set_position(canvas,pivot_position_x,pivot_position_y)
        self.pivot_position = [pivot_position_x,pivot_position_y]
        self.pivot = pivot

    def move_pivot(self,canvas,delta_x,delta_y,myimg):
      
        #現在選択している画像の、座標、横幅、縦幅を取得する。
        position_x,position_y = myimg.get_position()
        width = float(myimg.get_width())
        height = float(myimg.get_height())

        #ピボット用の画像を移動させる
        self.pivot_image.move_position(canvas,delta_x,delta_y)

        pivot_position_x,pivot_position_y = self.pivot_image.get_position()
        self.pivot_position = [pivot_position_x,pivot_position_y]
        right = position_x + width / 2
        left = position_x - width / 2

        top = position_y + height / 2
        bottom = position_y - height / 2

        if pivot_position_x > right:
            pivot_position_x = right
        elif pivot_position_x < left:
            pivot_position_x = left

        if pivot_position_y > top:
            pivot_position_y = top
        elif pivot_position_y < bottom:
            pivot_position_y = bottom

        self.pivot_image.set_position(canvas,pivot_position_x,pivot_position_y)
        self.pivot_position = [pivot_position_x,pivot_position_y]
        pivot_x = (pivot_position_x - left) / width
        pivot_y = abs((pivot_position_y - top)) / height

        self.pivot[0] = pivot_x
        self.pivot[1] = pivot_y

        return self.pivot

        

    def create_image(self,canvas,position_x,position_y,myimg):
        fn = glob.glob('Assets/sprite/rect.png')
        widht = float(myimg.get_width())
        height = float(myimg.get_height())
        for i in range(constant.NUMBER_IMAGE):
            rect = myimage.MyImage()
            rect.load_image(canvas,fn[0],constant.MYFRAME_IMAGE_TAG)
            
            pos_x = position_x + widht * self.myimage_position_list[i][0]
            pos_y = position_y + height * self.myimage_position_list[i][1]
            #枠は画像よりちょっと大き目なのでそれを考慮した座標を指定する
            rect.set_position(canvas,
            pos_x + constant.ADD_FRAME_SIZE * self.myimage_position_list[i][2],
            pos_y + constant.ADD_FRAME_SIZE * self.myimage_position_list[i][3]
            )
            self.rect_list[i] = rect

        fn = glob.glob('Assets/sprite/pivot.png')

        self.pivot_image = myimage.MyImage()
        self.pivot_image.load_image(canvas,fn[0],constant.MYFRAME_IMAGE_PIVOT_TAG)
        self.set_pivot(canvas,position_x,position_y,myimg)



    #座標と画像の大きさ
    def create_frame(self,canvas,position_x,position_y,myimg):
        #枠が存在していたら削除する
        if self.rect != None:
            self.delete_frame(canvas)
        self.position = [position_x,position_y]
        widht = float(myimg.get_width())
        height = float(myimg.get_height())
        self.position1_x = position_x - widht / 2 - constant.ADD_FRAME_SIZE
        self.position1_y = position_y + height / 2 + constant.ADD_FRAME_SIZE
        self.position2_x = position_x + widht / 2 + constant.ADD_FRAME_SIZE
        self.position2_y = position_y - height / 2 - constant.ADD_FRAME_SIZE
        #枠を生成する、引数の順番は、
        #左上のx座標、左上のy座標
        #右下のx座標、右下のy座標
        self.rect = canvas.create_rectangle(
        self.position1_x,
        self.position1_y,
        self.position2_x,
        self.position2_y,
        outline='red') 

       
        self.create_image(canvas,position_x,position_y,myimg)


    def delete_frame(self,canvas):
        #枠を削除する
        canvas.delete(self.rect)
        self.rect = None
        for i in self.rect_list:
            canvas.delete(self.rect_list[i].item_id)
        self.rect_list.clear()

        canvas.delete(self.pivot_image.item_id)
    