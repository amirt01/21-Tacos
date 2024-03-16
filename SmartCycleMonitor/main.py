import sys
import threading
import asyncio
from websockets.sync import client
import json

import customtkinter as ctk
from tkdial import Meter, Dial


class SmartCycleMonitor(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.websocket = None

        # configure window
        self.title("Smart Cycle Monitor")
        self.config(padx=20)

        # configure 3 row layout
        self.rowconfigure((0, 1, 2), pad=20)

        # Title
        ctk.CTkLabel(self, text="SmartCycle", font=("ComincSans", 42)).grid(row=0, sticky="ew")

        # Meter Frame
        self.meter_frame = ctk.CTkFrame(self)
        self.meter_frame.grid(row=1, column=0, sticky="ew")

        # Speedometer Frame
        self.speedometer_frame = ctk.CTkFrame(self.meter_frame)
        self.speedometer_frame.grid(row=0, column=0, padx=20, pady=20)

        # Speedometer Label
        ctk.CTkLabel(self.speedometer_frame, text="Speed", font=("ComincSans", 24)).grid(row=0, column=0)

        # Speedometer Dial
        self.speedometer = Meter(self.speedometer_frame, border_width=0, fg="#1f6aa5", text_color="white",
                                 text_font="DS-Digital 30", scale_color="white", needle_color="red")
        self.speedometer.grid(row=1, column=0, padx=20, pady=20)

        # Cadence Frame
        self.cadence_frame = ctk.CTkFrame(self.meter_frame)
        self.cadence_frame.grid(row=0, column=1, padx=20, pady=20)

        # Cadence Label
        ctk.CTkLabel(self.cadence_frame, text="Cadence", font=("ComincSans", 24)).grid(row=0, column=0)

        # Cadence Dial
        self.cadence = Meter(self.cadence_frame, border_width=0, fg="#1f6aa5", text_color="white", end=120,
                             text_font="DS-Digital 30", scale_color="white", needle_color="red")
        self.cadence.grid(row=1, column=0, padx=20, pady=20)

        # Gear Frmae
        self.gear_frame = ctk.CTkFrame(self.meter_frame)
        self.gear_frame.grid(row=0, column=2, padx=20, pady=20)

        # Gear Label
        ctk.CTkLabel(self.gear_frame, text="Gear", font=("ComincSans", 24)).grid(row=0, column=0)

        # Gear Meter
        self.gear = Meter(self.gear_frame, border_width=0, fg="#1f6aa5", text_color="white", start=1, end=6,
                          text_font="DS-Digital 30", scale_color="white", needle_color="red")
        self.gear.grid(row=1, column=0, padx=20, pady=20)

        self.raw_data_frame = ctk.CTkFrame(self)
        self.raw_data_frame.grid(row=2, sticky="ew", padx=20, pady=20)
        self.data_text = ctk.StringVar(value="waiting for data...")
        data_label = ctk.CTkLabel(self.raw_data_frame, textvariable=self.data_text, font=("ComicSans", 28))
        data_label.grid(padx=20, pady=20)

    def connect(self, uri):
        try:
            self.websocket = client.connect(uri)
        except TimeoutError:
            print("Connection Timeout! Make sure you are connected to the AP and the URI is correct.")

    async def listen(self):
        while True:
            if self.websocket:
                if buffer := self.websocket.recv():
                    self.data_text.set(buffer)
                    for k, v in json.loads(buffer).items():
                        match k:
                            case "speed":
                                self.speedometer.set(v)
                            case "cadence":
                                self.cadence.set(v)
                            case "gear":
                                self.gear.set(v)
                            case _:
                                print("Unrecognized Data")


if __name__ == '__main__':
    App = SmartCycleMonitor()
    App.connect("ws://192.168.4.1")
    threading.Thread(target=asyncio.run, args=(App.listen(),)).start()
    App.mainloop()
