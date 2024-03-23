import json
import threading

import customtkinter as ctk
import websocket, rel
from tkdial import Meter


class SmartCycleMonitor(ctk.CTk):
    def __init__(self):
        super().__init__()

        url = "ws://192.168.4.1"
        self.ws_app = websocket.WebSocketApp(
            url,
            on_data=self.on_data,
        )

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
        connect_button = ctk.CTkButton(header_frame, text="Connect", font=label_font,
                                       command=threading.Thread(target=self.ws_app.run_forever).start)
        connect_button.grid(row=0, column=1, pady=10, padx=10)

        # Meter Frame
        meter_frame = ctk.CTkFrame(self)
        meter_frame.grid(row=1, column=0, sticky="EW")

        # Speedometer Frame
        speedometer_frame = ctk.CTkFrame(meter_frame)
        speedometer_frame.grid(row=0, column=0, padx=(20, 10), pady=20)

        # Speedometer Label
        ctk.CTkLabel(speedometer_frame, text="Speed", font=label_font).grid(row=0, column=0, pady=5)

        # Speedometer Dial
        self.speedometer = Meter(speedometer_frame, border_width=0, fg="#1f6aa5", text_color="white",
                                 text_font="DS-Digital 30", scale_color="white", needle_color="red",
                                 integer=True, end=60)
        self.speedometer.grid(row=1, column=0, padx=20, pady=(5, 10))

        # Cadence Frame
        cadence_frame = ctk.CTkFrame(meter_frame)
        cadence_frame.grid(row=0, column=1, padx=10, pady=20)

        # Cadence Label
        ctk.CTkLabel(cadence_frame, text="Cadence", font=label_font).grid(row=0, column=0, pady=5)

        # Cadence Dial
        self.cadence = Meter(cadence_frame, border_width=0, fg="#1f6aa5", text_color="white", end=120,
                             text_font="DS-Digital 30", scale_color="white", needle_color="red",
                             integer=True)
        self.cadence.grid(row=1, column=0, padx=20, pady=(5, 10))

        # Gear Frame
        gear_frame = ctk.CTkFrame(meter_frame)
        gear_frame.grid(row=0, column=2, padx=(10, 20), pady=20, sticky="NS")
        gear_frame.rowconfigure((1, 2, 3, 4, 5, 6), weight=1)
        gear_frame.rowconfigure(0, weight=0)

        # Gear Label
        ctk.CTkLabel(gear_frame, text="Gear", font=label_font).grid(row=0, column=0, columnspan=3, pady=5, padx=20,
                                                                    sticky="N")

        # Gear Slider
        self.target_gear = ctk.CTkSlider(gear_frame, from_=1, to=6, orientation=ctk.VERTICAL, state=ctk.DISABLED,
                                         width=25, height=200, button_corner_radius=6, number_of_steps=5,
                                         button_length=15, progress_color="transparent")
        self.target_gear.grid(row=1, column=0, padx=(20, 0), pady=5, sticky="NS", rowspan=6)
        self.current_gear = ctk.CTkSlider(gear_frame, from_=1, to=6, orientation=ctk.VERTICAL, state=ctk.DISABLED,
                                          width=25, height=200, button_corner_radius=6, number_of_steps=5,
                                          button_length=15, progress_color="transparent")
        self.current_gear.grid(row=1, column=1, padx=(5, 0), pady=5, sticky="NS", rowspan=6)
        self.current_gear.set(1)
        self.target_gear.set(1)

        # Slider Indices
        for i in range(6):
            label = ctk.CTkLabel(gear_frame, text=str(6 - i), font=text_font)
            label.grid(row=i + 1, column=2, sticky="W", padx=(7.5, 20))

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
        self.raw_data_text = ctk.CTkLabel(raw_data_text_frame, text="Waiting to connect...", font=text_font,
                                          justify="left")
        self.raw_data_text.grid(row=0, column=1, pady=10, padx=(10, 20))

    def on_data(self, ws, packet: str, *_):
        j_obj = json.loads(packet)
        self.raw_data_text.configure(text=json.dumps(j_obj, indent=4))
        for k, v in j_obj.items():
            match k:
                case "speed":
                    self.speedometer.set(v)
                case "cadence":
                    self.cadence.set(v)
                case "target gear":
                    self.target_gear.set(v)
                case "current gear":
                    self.current_gear.set(v)
                case "state":
                    pass
                case _:
                    pass


if __name__ == '__main__':
    SmartCycleMonitor().mainloop()
