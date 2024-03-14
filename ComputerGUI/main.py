import threading

import customtkinter as ctk

import asyncio
import websockets

import json

# initialize ctk root
root = ctk.CTk()

# configure window
root.title("Smart Cycle Monitor")
root.geometry(f"{1100}x{580}")
# root.resizable(width=False, height=False)

# create listener message variable
rand1 = ctk.StringVar()
rand2 = ctk.StringVar()


async def listener():
    uri = "ws://192.168.4.1"
    async with websockets.connect(uri) as websocket:
        while True:
            temp = await websocket.recv()
            if temp:
                data_dict = json.loads(temp)
                rand1.set(data_dict["rand1"])
                rand2.set(data_dict["rand2"])


if __name__ == '__main__':
    # start listener thread
    listener_thread = threading.Thread(target=asyncio.run, args=(listener(),))
    listener_thread.start()

    # setup ctk
    label_font = ctk.CTkFont('Helvetica', 32)
    rand1_label = ctk.CTkLabel(root, text="rand1: ", font=label_font).grid(row=0, column=0)
    rand2_label = ctk.CTkLabel(root, text="rand2: ", font=label_font).grid(row=1, column=0)
    ctk.CTkLabel(root, textvariable=rand1, font=label_font).grid(row=0, column=1)
    ctk.CTkLabel(root, textvariable=rand2, font=label_font).grid(row=1, column=1)

    # start ctk
    root.mainloop()
