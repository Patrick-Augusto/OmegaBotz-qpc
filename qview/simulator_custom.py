from ast import Global
import os
from tkinter import *
from tkinter.ttk import * # override the basic Tk widgets with Ttk widgets
from tkinter.simpledialog import *
from struct import pack
from pynput import keyboard
from inputs import get_gamepad

import threading


from PIL import Image, ImageTk
import math

from robot import *
from led_stripe import *

USE_PS3_CONTROLLER = False

ROBOT_SIZE_PIXELS = 60
ARENA_RADIUS_PIXELS = 225


global qview_base
global image_dict
global canvas_dict
global sumo_robot
global key_pressed_dict
global line_sensors


QS_USER_DATA_ID = {
    "QS_LED_ID":    0,
    "QS_BUZZER_ID": 1,
    "QS_MOTOR_ID":  2,
    "QS_LED_STRIPE_ID":  3,
}

def custom_menu_command():
    command_name = "RESET POSITION"
    command_function =  reset_position

    return (command_name, command_function)

def custom_on_dettach():
    # print("Custom action on dettach")
    return
    

def custom_qview_init(qview):

    global qview_base
    qview_base = qview

    if (qview_base == None):
        return

    global HOME_DIR
    global image_dict, canvas_dict
    global last_sensor_active, line_sensors, action_counter
    global sumo_robot
    global key_pressed_dict, gamepad_dict
    global last_gamepad
    global last_line_sensor_state

    last_gamepad = (0,0,0,0)
    last_line_sensor_state = (False, False, False, False)



    HOME_DIR = os.path.dirname(__file__)

    sumo_robot = Robot(300, 120)
    action_counter = 0
    last_sensor_active = 0

    qview_base._view_canvas.set(1)
    qview_base.canvas.configure(width=600, height=600)

    key_pressed_dict = {
        "up": False,
        "down": False,
        "left": False,
        "right": False,
        "three": False,
        "four": False,
    }

    line_sensors = {
        "FL" : {
            "angle": 45,
            "active": False
        },
        "FR" : {
            "angle": -45,
            "active": False
        },
        "BL" : {
            "angle": 135,
            "active": False
        },
        "BR" : {
            "angle": -135,
            "active": False
        }
    }

    gamepad_dict = {
        "ch1": 127,
        "ch2": 127,
        "ch3": 127,
        "ch4": 127,
    }


    image_dict = {
        "arena": PhotoImage(file=HOME_DIR + "/img/arena.png"),
        "sumo": PhotoImage(file=HOME_DIR + "/img/raiju.png"),
        "num_0" : PhotoImage(file=HOME_DIR + "/img/num0.png"),
        "num_1" : PhotoImage(file=HOME_DIR + "/img/num1.png"),
        "num_2" : PhotoImage(file=HOME_DIR + "/img/num2.png"),
        "start" : PhotoImage(file=HOME_DIR + "/img/button_start.png"),
        "radio_ev_1_button" : PhotoImage(file=HOME_DIR + "/img/button_r-ev1.png"),
        "radio_ev_2_button" : PhotoImage(file=HOME_DIR + "/img/button_r-ev2.png"),
        "idle_button" : PhotoImage(file=HOME_DIR + "/img/button_idle.png"),
        "stop_button" : PhotoImage(file=HOME_DIR + "/img/button_stop.png"),
    }

    canvas_dict = {
        "arena" : qview_base.canvas.create_image(300,  300, image=image_dict["arena"]), 
        "led" : qview_base.canvas.create_rectangle(10, 5, 50, 45, outline = "black", fill = '#000000', width = 1), 
        "sumo" : qview_base.canvas.create_image(sumo_robot.get_position()[0],  sumo_robot.get_position()[1], image=image_dict["sumo"]), 
        "num_0" : qview_base.canvas.create_image(50, 570, image=image_dict["num_0"]), 
        "num_1" : qview_base.canvas.create_image(100, 570, image=image_dict["num_1"]), 
        "num_2" : qview_base.canvas.create_image(150, 570, image=image_dict["num_2"]), 
        "start" : qview_base.canvas.create_image(350, 570, image=image_dict["start"]), 
        "radio_ev_1_button" : qview_base.canvas.create_image(450, 570, image=image_dict["radio_ev_1_button"]), 
        "radio_ev_2_button" : qview_base.canvas.create_image(550, 570, image=image_dict["radio_ev_2_button"]), 
        "stop_button" : qview_base.canvas.create_image(550, 30, image=image_dict["stop_button"]), 
    }

    # Buttons
    qview_base.canvas.tag_bind(canvas_dict["num_0"], "<ButtonPress>",  lambda strategy: change_strategy(0))
    qview_base.canvas.tag_bind(canvas_dict["num_1"], "<ButtonPress>", lambda strategy: change_strategy(1))
    qview_base.canvas.tag_bind(canvas_dict["num_2"], "<ButtonPress>", lambda strategy: change_strategy(2))
    qview_base.canvas.tag_bind(canvas_dict["start"], "<ButtonPress>", start_command)
    qview_base.canvas.tag_bind(canvas_dict["radio_ev_1_button"], "<ButtonPress>", radio_evt1_command)
    qview_base.canvas.tag_bind(canvas_dict["radio_ev_2_button"], "<ButtonPress>", radio_evt2_command)
    qview_base.canvas.tag_bind(canvas_dict["stop_button"], "<ButtonPress>", stop_command)

    led_stripe_init(qview_base)

    if (USE_PS3_CONTROLLER):
        x = threading.Thread(target=gamepad_thread)
        x.daemon = True
        x.start()



def custom_user_00_packet(packet):
    data = qview_base.qunpack("xxTxb", packet)    
    data_id = data[1]     

    if (data_id == QS_USER_DATA_ID['QS_LED_ID']):
        process_led_id_packet(packet)
    elif (data_id == QS_USER_DATA_ID['QS_MOTOR_ID']):
        process_motor_id_packet(packet)
    elif (data_id == QS_USER_DATA_ID["QS_BUZZER_ID"]):
        process_buzzer_id_packet(packet)
    elif (data_id == QS_USER_DATA_ID["QS_LED_STRIPE_ID"]):
        process_led_stripe_id_packet(packet)
    else:
        qview_base.print_text("Timestamp = %d; ID = %d"%(data[0], data[1]))  


def process_led_id_packet(packet):
    data = qview_base.qunpack("xxTxbxb", packet)    
    led_stat = data[2]
    if (led_stat == 1):
        qview_base.canvas.itemconfig(canvas_dict["led"], fill = '#FFFFFF')
    else:
        qview_base.canvas.itemconfig(canvas_dict["led"], fill = '#000000')
    qview_base.print_text("Timestamp = %d; LED = %d"%(data[0], data[2]))  
    
def process_motor_id_packet(packet):
    data = qview_base.qunpack("xxTxbxbxb", packet)    
    sumo_robot.set_motors(data[2], data[3])
    mot_esq, mot_dir = sumo_robot.get_motors()
    qview_base.print_text("Timestamp = %d; MOT_ESQ = %d; MOT_DIR = %d"%(data[0], mot_esq, mot_dir))  

def process_buzzer_id_packet(packet):
    data = qview_base.qunpack("xxTxbxb", packet)    
    qview_base.print_text("Timestamp = %d; Buzzer = %d"%(data[0], data[2]))  

def process_led_stripe_id_packet(packet):
    data = qview_base.qunpack("xxTxBxBxBxBxB", packet)    
    led_idx = data[2]
    r = data[3]
    g = data[4]
    b = data[5]
    qview_base.print_text("Timestamp = %d; Led Strip Num %d RGB = %x,%x,%x"%(data[0], led_idx, r, g, b)) 
    if (led_idx == 255):
        led_stripe_set_all(qview_base, r, g, b)
    elif (led_idx == 254):
        led_stripe_set_half(qview_base, True, r, g, b)
    elif (led_idx == 253):
        led_stripe_set_half(qview_base, False, r, g, b)
    else:
        led_stripe_set(qview_base, led_idx, r, g, b)



def custom_user_01_packet(packet):
    return

def custom_on_poll():
    global action_counter
    action_counter += 1

    if (action_counter % 4 == 0):
        global sumo_rotated_canvas
        global tk_sumo_rotated
        global last_sensor_active

        mot_esq, mot_dir = sumo_robot.get_motors()
        angle = sumo_robot.get_angle()
        
        rotation_vel = (mot_dir - mot_esq) / 2
        angle += (rotation_vel / 10)
        sumo_robot.set_angle(angle % 360)

        vel = ((mot_esq + mot_dir) / 2)
        vel_x = int((vel * math.sin(angle * math.pi/180))/10)
        vel_y = int((vel * math.cos(angle * math.pi/180))/10)

        sumo_robot.offset_position(vel_x, vel_y)

        sumo_rotated_canvas = Image.open(HOME_DIR + "/img/raiju.png")
        tk_sumo_rotated = ImageTk.PhotoImage(sumo_rotated_canvas.rotate(angle))
        qview_base.canvas.itemconfig(canvas_dict["sumo"], image=tk_sumo_rotated)
        qview_base.canvas.move(canvas_dict["sumo"], vel_x, vel_y)

        robot_pos_x, robot_pos_y = sumo_robot.get_position()

        # Line Sensor simulator
        update_line_sensor(robot_pos_x, robot_pos_y,angle)
        if (line_sensor_changed()):
            line_command()

        # Distance Sensor simulator:
        sensor_active = is_mouse_direction(robot_pos_x, robot_pos_y, angle)
        if (sensor_active != last_sensor_active):
            sensor_command(sensor_active)

        last_sensor_active = sensor_active

        # radio Simulator
        if (USE_PS3_CONTROLLER):
            if (action_counter % 12 == 0):
                send_game_pad()
        else:
            if (action_counter % 20 == 0):
                send_keyboard()

def start_command(*args):
    qview_base.command(0, 0)

def stop_command(*args):
    qview_base.command(1, 0)

def change_strategy(strategy):
    qview_base.command(2, strategy)

def line_command():
    qview_base.command(3, 0)

def sensor_command(sensor):
    qview_base.command(4, sensor)

def radio_evt1_command(*args):
    qview_base.command(5, 0)

def radio_evt2_command(*args):
    qview_base.command(6, 0)

def send_radio_command_ch1_ch2(ch1, ch2):
    qview_base.command(7, ch1, ch2)

def send_radio_command_ch3_ch4(ch3, ch4):
    qview_base.command(8, ch3, ch4)

def send_game_pad():

    global last_gamepad

    # x coordinate
    ch1 = gamepad_dict["ch1"]
    
    # y coordinate
    ch2 = 255 - gamepad_dict["ch2"]

    ch3 = gamepad_dict["ch3"]

    ch4 = gamepad_dict["ch4"]

    # Send only when there is change so that the simulator does not get slow
    if (last_gamepad[0] != ch1 or last_gamepad[1] != ch2 or last_gamepad[2] != ch3 or last_gamepad[3] != ch3):
        send_radio_command_ch1_ch2(ch1, ch2)
        send_radio_command_ch3_ch4(ch3, ch4)
        
    last_gamepad = (ch1, ch2, ch3, ch4)

def send_keyboard():

    # x coordinate
    x_keyboard = 0
    if (key_pressed_dict["right"]):
        x_keyboard = 255
    elif (key_pressed_dict["left"]):
        x_keyboard = 0
    else:
        x_keyboard = 127

    # y coordinate
    y_keyboard = 0
    if (key_pressed_dict["up"]):
        y_keyboard = 255
    elif (key_pressed_dict["down"]):
        y_keyboard = 0
    else:
        y_keyboard = 127

    
    send_radio_command_ch1_ch2(x_keyboard, y_keyboard)  
    send_radio_command_ch3_ch4(key_pressed_dict["three"], key_pressed_dict["four"])



def reset_position():
    global image_dict, canvas_dict
    sumo_robot.set_angle(0)
    sumo_robot.set_position(300, 100)
    canvas_dict["sumo"] = qview_base.canvas.create_image(sumo_robot.get_position()[0],  sumo_robot.get_position()[1], image=image_dict["sumo"])

def line_sensor_changed():
    global last_line_sensor_state
    changed = False
    current_line_active = (line_sensors["FL"]["active"], line_sensors["FR"]["active"], line_sensors["BL"]["active"], line_sensors["BR"]["active"])

    for i in range(len(current_line_active)):
        if (current_line_active[i] != last_line_sensor_state[i]):
            changed = True

    last_line_sensor_state = current_line_active
    return changed

def update_line_sensor(posx, posy, robot_angle):

    robot_angle %= 360
    if (robot_angle > 180):
        robot_angle -= 360
    elif (robot_angle < -180):
        robot_angle += 360
    
    line_sensors_dist_center = (ROBOT_SIZE_PIXELS/2) * math.sqrt(2)

    for key, sensor in line_sensors.items():
        relative_angle = math.radians(robot_angle + sensor["angle"])
        sensor_x = posx + (line_sensors_dist_center * math.sin(relative_angle)) 
        sensor_y = posy + (line_sensors_dist_center * math.cos(relative_angle))

        if (((sensor_x - 300 ) ** 2 + (sensor_y - 300 ) ** 2) > ARENA_RADIUS_PIXELS ** 2):
            sensor["active"] = True
        else:
            sensor["active"] = False

def is_mouse_direction(posx, posy, robot_angle):
    mouse_pos_x_abs = qview_base.canvas.winfo_pointerx() - qview_base.canvas.winfo_rootx()
    mouse_pos_y_abs = qview_base.canvas.winfo_pointery() - qview_base.canvas.winfo_rooty()

    mouse_pos_x_rel = mouse_pos_x_abs - posx
    mouse_pos_y_rel = mouse_pos_y_abs - posy

    mouse_angle_rel = math.degrees(math.atan2(mouse_pos_x_rel, mouse_pos_y_rel))
    mouse_robot_distance = math.sqrt(mouse_pos_x_rel ** 2 + mouse_pos_y_rel ** 2)

    # Limit robot angle range
    robot_angle %= 360
    if (robot_angle > 180):
        robot_angle -= 360
    elif (robot_angle < -180):
        robot_angle += 360
    
    anglediff = mouse_angle_rel - robot_angle

    if (mouse_robot_distance < 150):
        if (anglediff > -15 and anglediff < 15):
            return 3
        elif (anglediff > 15 and anglediff < 40):
            return 4
        elif (anglediff > 40 and anglediff < 60):
            return 5
        elif (anglediff > -40 and anglediff < -15):
            return 2
        elif (anglediff > -60 and anglediff < -40):
            return 1
    
    return 0


# Keyboard data simulating radio data
def on_press(key):
    global key_pressed_dict
    try:
        if (key == keyboard.Key.up):
            key_pressed_dict["up"] = True
        elif (key == keyboard.Key.down):
            key_pressed_dict["down"] = True
        elif (key == keyboard.Key.right):
            key_pressed_dict["right"] = True
        elif (key == keyboard.Key.left):
            key_pressed_dict["left"] = True
        elif (key.char == '3'):
            key_pressed_dict["three"] = True
        elif (key.char == '4'):
            key_pressed_dict["four"] = True
        
    except AttributeError:
        # print('special key {0} pressed'.format(key))
        return




def on_release(key):
    global key_pressed_dict
    try:
        if (key == keyboard.Key.up):
            key_pressed_dict["up"] = False
        elif (key == keyboard.Key.down):
            key_pressed_dict["down"] = False
        elif (key == keyboard.Key.right):
            key_pressed_dict["right"] = False
        elif (key == keyboard.Key.left):
            key_pressed_dict["left"] = False
        elif (key.char == '3'):
            key_pressed_dict["three"] = False
        elif (key.char == '4'):
            key_pressed_dict["four"] = False
    
    except AttributeError:
        return
        # print('special key {0} released'.format(key))
    
# ...or, in a non-blocking fashion:
listener = keyboard.Listener(
    on_press=on_press,
    on_release=on_release)
listener.start()


def gamepad_thread():

    global gamepad_dict
    while(True):
        event = get_gamepad()
        if (event[0].code == "ABS_RX"):
            gamepad_dict["ch1"] = event[0].state
        elif (event[0].code == "ABS_RY"):
            gamepad_dict["ch2"] = event[0].state
        elif (event[0].code == "BTN_DPAD_RIGHT"):
            gamepad_dict["ch3"] = event[0].state
        elif (event[0].code == "BTN_DPAD_UP"):
            gamepad_dict["ch4"] = event[0].state