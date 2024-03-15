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
StringVars = {
    "speed": ctk.DoubleVar(value=0),
    "cadence": ctk.DoubleVar(value=0),
    "gear": ctk.IntVar(value=1)
}


async def listener():
    uri = "ws://192.168.4.1"
    async with websockets.connect(uri) as websocket:
        while True:
            if buffer := await websocket.recv():
                for k, v in json.loads(buffer).items():
                    StringVars[k].set(v)


if __name__ == '__main__':
    # start listener thread
    listener_thread = threading.Thread(target=asyncio.run, args=(listener(),))
    listener_thread.start()

    # setup ctk
    label_font = ctk.CTkFont('Helvetica', 32)
    for i, (k, v) in enumerate(StringVars.items()):
        ctk.CTkLabel(root, text=f"{k}: ", font=label_font).grid(row=i, column=0, sticky=ctk.E)
        ctk.CTkLabel(root, textvariable=v, font=label_font).grid(row=i, column=1, sticky=ctk.W)

    # start ctk
    root.mainloop()
