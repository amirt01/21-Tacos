import sys
import threading
import asyncio
from websockets.sync import client
import json

import customtkinter as ctk
from tkdial import Meter, Dial

uri = "ws://192.168.4.1"


class SmartCycleMonitor(ctk.CTk):
    def __init__(self):
        super().__init__()

        self.websocket = None

        # configure window
        self.title("Smart Cycle Monitor")
        self.config(padx=20)

        # configure 3 row layout
        self.rowconfigure((0, 1, 2), pad=20)

        # Fonts
        label_font = ctk.CTkFont("ComicSans", 28)
        text_font = ctk.CTkFont("DS-Digital", 18)

        # Title
        header_frame = ctk.CTkFrame(self)
        header_frame.grid(row=0, sticky="EW")
        header_frame.columnconfigure(0, weight=1)
        header_frame.columnconfigure(1, weight=0)
        ctk.CTkLabel(header_frame, text="SmartCycle", font=("ComincSans", 42)).grid(row=0, column=0, sticky="EW")
        ctk.CTkButton(header_frame, text="Connect", font=label_font,
                      command=lambda: self.connect()).grid(row=0, column=1, pady=10, padx=10)

        # Meter Frame
        meter_frame = ctk.CTkFrame(self)
        meter_frame.grid(row=1, column=0, sticky="EW")

        # Speedometer Frame
        speedometer_frame = ctk.CTkFrame(meter_frame)
        speedometer_frame.grid(row=0, column=0, padx=20, pady=20)

        # Speedometer Label
        ctk.CTkLabel(speedometer_frame, text="Speed", font=label_font).grid(row=0, column=0, pady=5)

        # Speedometer Dial
        self.speedometer = Meter(speedometer_frame, border_width=0, fg="#1f6aa5", text_color="white",
                                 text_font="DS-Digital 30", scale_color="white", needle_color="red")
        self.speedometer.grid(row=1, column=0, padx=20, pady=(5, 10))

        # Cadence Frame
        cadence_frame = ctk.CTkFrame(meter_frame)
        cadence_frame.grid(row=0, column=1, padx=20, pady=20)

        # Cadence Label
        ctk.CTkLabel(cadence_frame, text="Cadence", font=label_font).grid(row=0, column=0, pady=5)

        # Cadence Dial
        self.cadence = Meter(cadence_frame, border_width=0, fg="#1f6aa5", text_color="white", end=120,
                             text_font="DS-Digital 30", scale_color="white", needle_color="red")
        self.cadence.grid(row=1, column=0, padx=20, pady=(5, 10))

        # Gear Frame
        gear_frame = ctk.CTkFrame(meter_frame)
        gear_frame.grid(row=0, column=2, padx=20, pady=5)

        # Gear Label
        ctk.CTkLabel(gear_frame, text="Gear", font=label_font).grid(row=0, column=0, pady=5)

        # Gear Meter
        self.gear = Meter(gear_frame, border_width=0, fg="#1f6aa5", text_color="white", start=1, end=6,
                          text_font="DS-Digital 30", scale_color="white", needle_color="red")
        self.gear.grid(row=1, column=0, padx=20, pady=(5, 10))

        # Raw Data Frame
        raw_data_frame = ctk.CTkFrame(self)
        raw_data_frame.grid(row=2, sticky="EW")
        raw_data_frame.columnconfigure(0, weight=0)
        raw_data_frame.columnconfigure(1, weight=1)

        # Raw Data Label
        ctk.CTkLabel(raw_data_frame, text="Raw Data: ", font=label_font).grid(row=0, column=0, sticky="E", padx=20)

        # Raw Data Text Output
        raw_data_text_frame = ctk.CTkFrame(raw_data_frame)
        raw_data_text_frame.grid(row=0, column=1, pady=10, padx=10, sticky="EW")
        self.data_text = ctk.StringVar(value="Waiting to connect...")
        ctk.CTkLabel(raw_data_text_frame, textvariable=self.data_text, font=text_font).grid(row=0, column=1, pady=10,
                                                                                            padx=20)

    def connect(self):
        self.data_text.set("Connecting...")
        threading.Thread(target=self.__connect, args=()).start()

    def __connect(self):
        try:
            self.websocket = client.connect(uri)
            self.listen()
        except TimeoutError:
            self.data_text.set("Waiting to connect...")
            print("Connection Timeout! Make sure you are connected to the AP and the URI is correct.")

    def listen(self):
        self.data_text.set("Listening...")
        threading.Thread(target=self.__listen).start()

    def __listen(self):
        while True:
            if self.websocket and (buffer := self.websocket.recv()):
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
    SmartCycleMonitor().mainloop()
