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


def open_input_dialog_event():
    dialog = ctk.CTkInputDialog(text="Type in a number:", title="CTkInputDialog")
    print("CTkInputDialog:", dialog.get_input())


def change_appearance_mode_event(new_appearance_mode: str):
    ctk.set_appearance_mode(new_appearance_mode)


def change_scaling_event(new_scaling: str):
    new_scaling_float = int(new_scaling.replace("%", "")) / 100
    ctk.set_widget_scaling(new_scaling_float)


def sidebar_button_event():
    print("sidebar_button click")


if __name__ == '__main__':
    # start listener thread
    listener_thread = threading.Thread(target=asyncio.run, args=(listener(),))
    listener_thread.start()

    # configure grid layout (4x4)
    root.grid_columnconfigure(1, weight=1)
    root.grid_columnconfigure(3, weight=1)
    root.grid_rowconfigure((0, 1, 2), weight=1)

    # create sidebar frame with widgets
    sidebar_frame = ctk.CTkFrame(root, width=140, corner_radius=0)
    sidebar_frame.grid(row=0, column=0, rowspan=4, sticky="nsew")
    sidebar_frame.grid_rowconfigure(4, weight=1)
    logo_label = ctk.CTkLabel(sidebar_frame, text="SmartCycle", font=ctk.CTkFont(size=20, weight="bold"))
    logo_label.grid(row=0, column=0, padx=20, pady=(20, 10))
    appearance_mode_label = ctk.CTkLabel(sidebar_frame, text="Appearance Mode:", anchor="w")
    appearance_mode_label.grid(row=5, column=0, padx=20, pady=(10, 0))
    appearance_mode_optionemenu = ctk.CTkOptionMenu(sidebar_frame, values=["Light", "Dark", "System"],
                                                    command=change_appearance_mode_event)
    appearance_mode_optionemenu.grid(row=6, column=0, padx=20, pady=(10, 10))
    scaling_label = ctk.CTkLabel(sidebar_frame, text="UI Scaling:", anchor="w")
    scaling_label.grid(row=7, column=0, padx=20, pady=(10, 0))
    scaling_optionemenu = ctk.CTkOptionMenu(sidebar_frame, values=["80%", "90%", "100%", "110%", "120%"],
                                            command=change_scaling_event)
    scaling_optionemenu.grid(row=8, column=0, padx=20, pady=(10, 20))

    appearance_mode_optionemenu.set("Dark")
    scaling_optionemenu.set("100%")

    label_font = ctk.CTkFont('Helvetica', 32)
    for i, (k, v) in enumerate(StringVars.items()):
        ctk.CTkLabel(root, text=f"{k}: ", font=label_font).grid(row=i, column=1, sticky=ctk.E, padx=10)
        ctk.CTkLabel(root, textvariable=v, font=label_font).grid(row=i, column=2, sticky=ctk.W, padx=10)

    # start ctk
    root.mainloop()
