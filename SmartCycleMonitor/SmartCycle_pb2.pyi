from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar, Mapping, Optional, Union

DESCRIPTOR: _descriptor.FileDescriptor

class Message(_message.Message):
    __slots__ = ["telemetry", "tuning"]
    TELEMETRY_FIELD_NUMBER: ClassVar[int]
    TUNING_FIELD_NUMBER: ClassVar[int]
    telemetry: Telemetry
    tuning: Tuning
    def __init__(self, telemetry: Optional[Union[Telemetry, Mapping]] = ..., tuning: Optional[Union[Tuning, Mapping]] = ...) -> None: ...

class Telemetry(_message.Message):
    __slots__ = ["cadence", "current_gear", "down_shift_button", "speed", "state", "target_gear", "up_shift_button"]
    class ButtonState(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    class State(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = []
    ASLEEP: Telemetry.State
    BIKING: Telemetry.State
    CADENCE_FIELD_NUMBER: ClassVar[int]
    CURRENT_GEAR_FIELD_NUMBER: ClassVar[int]
    DOWN_SHIFT_BUTTON_FIELD_NUMBER: ClassVar[int]
    PRESSED: Telemetry.ButtonState
    RELEASED: Telemetry.ButtonState
    SPEED_FIELD_NUMBER: ClassVar[int]
    STATE_FIELD_NUMBER: ClassVar[int]
    STOPPED: Telemetry.State
    TARGET_GEAR_FIELD_NUMBER: ClassVar[int]
    UP_SHIFT_BUTTON_FIELD_NUMBER: ClassVar[int]
    cadence: float
    current_gear: int
    down_shift_button: Telemetry.ButtonState
    speed: float
    state: Telemetry.State
    target_gear: int
    up_shift_button: Telemetry.ButtonState
    def __init__(self, speed: Optional[float] = ..., cadence: Optional[float] = ..., target_gear: Optional[int] = ..., current_gear: Optional[int] = ..., state: Optional[Union[Telemetry.State, str]] = ..., up_shift_button: Optional[Union[Telemetry.ButtonState, str]] = ..., down_shift_button: Optional[Union[Telemetry.ButtonState, str]] = ...) -> None: ...

class Tuning(_message.Message):
    __slots__ = ["desired_cadence_high", "desired_cadence_low", "nominal_gear_encoder_value_1", "nominal_gear_encoder_value_2", "nominal_gear_encoder_value_3", "nominal_gear_encoder_value_4", "nominal_gear_encoder_value_5", "nominal_gear_encoder_value_6"]
    DESIRED_CADENCE_HIGH_FIELD_NUMBER: ClassVar[int]
    DESIRED_CADENCE_LOW_FIELD_NUMBER: ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_1_FIELD_NUMBER: ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_2_FIELD_NUMBER: ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_3_FIELD_NUMBER: ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_4_FIELD_NUMBER: ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_5_FIELD_NUMBER: ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_6_FIELD_NUMBER: ClassVar[int]
    desired_cadence_high: float
    desired_cadence_low: float
    nominal_gear_encoder_value_1: int
    nominal_gear_encoder_value_2: int
    nominal_gear_encoder_value_3: int
    nominal_gear_encoder_value_4: int
    nominal_gear_encoder_value_5: int
    nominal_gear_encoder_value_6: int
    def __init__(self, nominal_gear_encoder_value_1: Optional[int] = ..., nominal_gear_encoder_value_2: Optional[int] = ..., nominal_gear_encoder_value_3: Optional[int] = ..., nominal_gear_encoder_value_4: Optional[int] = ..., nominal_gear_encoder_value_5: Optional[int] = ..., nominal_gear_encoder_value_6: Optional[int] = ..., desired_cadence_high: Optional[float] = ..., desired_cadence_low: Optional[float] = ...) -> None: ...
