import SmartCycle_pb2 as SmartCycle
from google.protobuf.message import DecodeError
from google.protobuf.json_format import MessageToJson

import threading

import customtkinter as ctk
import websocket
from tkdial import Meter


class SmartCycleMonitor(ctk.CTk):
    def __init__(self):
        super().__init__()

        url = "ws://192.168.4.1"
        self.data = b''
        self.message = SmartCycle.Message()
        self.ws_app = websocket.WebSocketApp(
            url,
            on_open=lambda *_: self.raw_data_text.configure(text="Connected!"),
            on_data=lambda ws, msg, *_: self.on_data(ws, msg)
        )

        # configure window
        self.title("Smart Cycle Monitor")
        self.config(padx=20)

        # configure 3 row layout
        self.rowconfigure((0, 1, 2), pad=20)
        self.columnconfigure(0, pad=20, weight=1)
        self.columnconfigure(1, pad=20, weight=0)

        # Fonts
        title_font = ctk.CTkFont("ComincSans", 42, "bold")
        label_font = ctk.CTkFont("ComicSans", 28)
        text_font = ctk.CTkFont("DS-Digital", 18)

        # Title
        header_frame = ctk.CTkFrame(self)
        header_frame.grid(row=0, sticky="EW", padx=(0, 5))
        header_frame.columnconfigure(0, weight=1)
        header_frame.columnconfigure(1, weight=0)
        ctk.CTkLabel(header_frame, text="SmartCycle", font=title_font).grid(row=0, column=0, sticky="EW")
        connect_button = ctk.CTkButton(header_frame, text="Connect", font=label_font,
                                       command=threading.Thread(target=self.ws_app.run_forever).start)
        connect_button.grid(row=0, column=1, pady=10, padx=10)

        # Meter Frame
        meter_frame = ctk.CTkFrame(self)
        meter_frame.grid(row=1, column=0, sticky="EW", padx=(0, 5))
        meter_frame.columnconfigure(2, weight=1)
        meter_frame.columnconfigure((0, 1), weight=0)

        # Speedometer Frame
        speedometer_frame = ctk.CTkFrame(meter_frame)
        speedometer_frame.grid(row=0, column=0, padx=(10, 5), pady=10, sticky="NSEW")
        speedometer_frame.rowconfigure(1, weight=1)
        speedometer_frame.columnconfigure(0, weight=1)

        # Speedometer Label
        ctk.CTkLabel(speedometer_frame, text="Speed", font=label_font).grid(row=0, column=0, pady=5)

        # Speedometer Dial
        self.speedometer = Meter(speedometer_frame, border_width=0, fg="#1f6aa5", text_color="white",
                                 text_font="DS-Digital 30", scale_color="white", needle_color="red",
                                 integer=True, end=60, end_angle=-300, state=ctk.DISABLED, radius=500)
        self.speedometer.grid(row=1, column=0, padx=10, pady=(5, 10), sticky="NSEW")

        # Cadence Frame
        cadence_frame = ctk.CTkFrame(meter_frame)
        cadence_frame.grid(row=0, column=1, padx=5, pady=10, sticky="NSEW")
        cadence_frame.rowconfigure(1, weight=1)
        cadence_frame.columnconfigure(0, weight=1)

        # Cadence Label
        ctk.CTkLabel(cadence_frame, text="Cadence", font=label_font).grid(row=0, column=0, pady=5)

        # Cadence Dial
        self.cadence = Meter(cadence_frame, border_width=0, fg="#1f6aa5", text_color="white", end=120,
                             text_font="DS-Digital 30", scale_color="white", needle_color="red",
                             integer=True, end_angle=-300, state=ctk.DISABLED, radius=500)
        self.cadence.grid(row=1, column=0, padx=10, pady=(5, 10), sticky="NSEW")

        # Gear Frame
        gear_frame = ctk.CTkFrame(meter_frame)
        gear_frame.grid(row=0, column=2, padx=(5, 10), pady=10, sticky="NSEW")
        gear_frame.rowconfigure((1, 2, 3, 4, 5, 6), weight=1)
        gear_frame.rowconfigure(0, weight=0)
        gear_frame.columnconfigure((0, 1), weight=1)
        gear_frame.columnconfigure((2), weight=0)

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

        # Gear Slider Labels
        target_label = ctk.CTkLabel(gear_frame, text="Target", font=text_font)
        target_label.grid(row=7, column=0, pady=(5, 5), sticky="S")

        current_label = ctk.CTkLabel(gear_frame, text="Current", font=text_font)
        current_label.grid(row=7, column=1, pady=(5, 5), sticky="S")

        # Raw Data Frame
        raw_data_frame = ctk.CTkFrame(self)
        raw_data_frame.grid(row=2, sticky="EW", padx=(0, 5))
        raw_data_frame.columnconfigure(0, weight=0)
        raw_data_frame.columnconfigure(1, weight=1)

        # Raw Data Label
        ctk.CTkLabel(raw_data_frame, text="Raw Data: ", font=label_font).grid(row=0, column=0, sticky="E", padx=20)

        # Raw Data Text Output
        raw_data_text_frame = ctk.CTkScrollableFrame(raw_data_frame)
        raw_data_text_frame.grid(row=0, column=1, pady=10, padx=10, sticky="EW")
        self.raw_data_text = ctk.CTkLabel(raw_data_text_frame, text="Waiting to connect...", font=text_font,
                                          justify="left")
        self.raw_data_text.grid(row=0, column=1, pady=10, padx=(10, 20))

        # Tuning Panel
        '''
        config_frame = ctk.CTkFrame(self, width=420)
        config_frame.grid(row=0, column=1, rowspan=3, sticky="NSEW", padx=(5, 0), pady=10)

        config_label = ctk.CTkLabel(config_frame, text="Tuning", font=label_font)
        config_label.grid(row=0, column=0, sticky="EWN", padx=10, pady=10)
        '''
        # Tuning Panel
        config_frame = ctk.CTkFrame(self)
        config_frame.grid(row=0, column=1, rowspan=3, sticky="NSEW", padx=(10, 0), pady=10)

        # Create and place "Tuning" label centered in the frame
        config_label = ctk.CTkLabel(config_frame, text="Tuning", font=label_font)
        config_label.grid(row=0, column=0, columnspan=3, sticky="NSEW", padx=10, pady=10)

        # Create and place horizontal sliders for adjusting gear variables
        self.tuning_variables = [ctk.DoubleVar() for _ in SmartCycle.Tuning.DESCRIPTOR.fields_by_name]
        for i in range(6):
            var_label = ctk.CTkLabel(config_frame, text=f"Gear {i + 1}", font=label_font)
            var_label.grid(row=i + 1, column=0, sticky="E", padx=10, pady=5)

            # Create and configure horizontal slider
            tuning_slider = ctk.CTkSlider(config_frame, from_=0, to=1000, variable=self.tuning_variables[i])
            tuning_slider.grid(row=i + 1, column=1, sticky="W", padx=10, pady=5)

            # Create label to display slider value
            tuning_label = ctk.CTkLabel(config_frame, text="0", font=label_font, textvariable=self.tuning_variables[i])
            tuning_label.grid(row=i + 1, column=2, sticky="W", padx=(0, 10), pady=5)

        # Add space between "Gear 6" and "Desired Cadence Range"
        empty_row = ctk.CTkFrame(config_frame, height=10)
        empty_row.grid(row=7, column=0, columnspan=3)

        # Create and place "Desired Cadence Range" label centered in the frame
        var_label = ctk.CTkLabel(config_frame, text=f"Desired Cadence Range", font=label_font)
        var_label.grid(row=8, column=0, columnspan=3, sticky="NSEW", padx=10, pady=10)

        # Create and place sliders for adjusting cadence range
        var_label_max = ctk.CTkLabel(config_frame, text=f"Max", font=label_font)
        var_label_max.grid(row=9, column=0, sticky="E", padx=10, pady=5)
        slider_max = ctk.CTkSlider(config_frame, from_=0, to=120, variable=self.tuning_variables[6])
        slider_max.grid(row=9, column=1, sticky="W", padx=10, pady=5)

        # Create label to display max slider value
        max_value_label = ctk.CTkLabel(config_frame, text="0", font=label_font, textvariable=self.tuning_variables[6])
        max_value_label.grid(row=9, column=2, sticky="W", padx=(0, 10), pady=5)

        self.tuning_variables.append(ctk.DoubleVar())
        var_label_min = ctk.CTkLabel(config_frame, text=f"Min", font=label_font)
        var_label_min.grid(row=10, column=0, sticky="E", padx=10, pady=5)
        slider_min = ctk.CTkSlider(config_frame, from_=0, to=120, variable=self.tuning_variables[7])
        slider_min.grid(row=10, column=1, sticky="W", padx=10, pady=5)

        # Create label to display min slider value
        min_value_label = ctk.CTkLabel(config_frame, text="0", font=label_font, textvariable=self.tuning_variables[7])
        min_value_label.grid(row=10, column=2, sticky="W", padx=(0, 10), pady=5)

    def on_data(self, ws, msg):
        # record all messages other than the null terminator
        if msg != b'\x00':
            self.data += msg
            return

        # decode the complete message and handle any error
        try:
            self.message.ParseFromString(self.data)
        except DecodeError:
            print("Could not decode:", self.data)
        self.data = b''

        # store the newly decoded data
        self.raw_data_text.configure(text=MessageToJson(self.message))
        match self.message.WhichOneof("packet"):
            case "telemetry":
                self.speedometer.set(self.message.telemetry.speed)
                self.cadence.set(self.message.telemetry.cadence)
                self.target_gear.set(self.message.telemetry.target_gear)
                self.current_gear.set(self.message.telemetry.current_gear)
            case "tuning":
                for i in range(6):
                    self.tuning_variables[i].set(getattr(self.message.tuning, f"nominal_gear_encoder_value_{i + 1}"))
                self.tuning_variables[6].set(getattr(self.message.tuning, f"desired_cadence_high"))
                self.tuning_variables[7].set(getattr(self.message.tuning, f"desired_cadence_low"))


if __name__ == '__main__':
    SmartCycleMonitor().mainloop()
