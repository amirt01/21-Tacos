from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class Telemetry(_message.Message):
    __slots__ = ("speed", "cadence", "target_gear", "current_gear", "state", "up_shift_button", "down_shift_button")
    class State(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        BIKING: _ClassVar[Telemetry.State]
        STOPPED: _ClassVar[Telemetry.State]
        ASLEEP: _ClassVar[Telemetry.State]
    BIKING: Telemetry.State
    STOPPED: Telemetry.State
    ASLEEP: Telemetry.State
    class ButtonState(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        RELEASED: _ClassVar[Telemetry.ButtonState]
        PRESSED: _ClassVar[Telemetry.ButtonState]
    RELEASED: Telemetry.ButtonState
    PRESSED: Telemetry.ButtonState
    SPEED_FIELD_NUMBER: _ClassVar[int]
    CADENCE_FIELD_NUMBER: _ClassVar[int]
    TARGET_GEAR_FIELD_NUMBER: _ClassVar[int]
    CURRENT_GEAR_FIELD_NUMBER: _ClassVar[int]
    STATE_FIELD_NUMBER: _ClassVar[int]
    UP_SHIFT_BUTTON_FIELD_NUMBER: _ClassVar[int]
    DOWN_SHIFT_BUTTON_FIELD_NUMBER: _ClassVar[int]
    speed: float
    cadence: float
    target_gear: int
    current_gear: int
    state: Telemetry.State
    up_shift_button: Telemetry.ButtonState
    down_shift_button: Telemetry.ButtonState
    def __init__(self, speed: _Optional[float] = ..., cadence: _Optional[float] = ..., target_gear: _Optional[int] = ..., current_gear: _Optional[int] = ..., state: _Optional[_Union[Telemetry.State, str]] = ..., up_shift_button: _Optional[_Union[Telemetry.ButtonState, str]] = ..., down_shift_button: _Optional[_Union[Telemetry.ButtonState, str]] = ...) -> None: ...

class Tuning(_message.Message):
    __slots__ = ("nominal_gear_encoder_value_1", "nominal_gear_encoder_value_2", "nominal_gear_encoder_value_3", "nominal_gear_encoder_value_4", "nominal_gear_encoder_value_5", "nominal_gear_encoder_value_6", "desired_cadence_high", "desired_cadence_low")
    NOMINAL_GEAR_ENCODER_VALUE_1_FIELD_NUMBER: _ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_2_FIELD_NUMBER: _ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_3_FIELD_NUMBER: _ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_4_FIELD_NUMBER: _ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_5_FIELD_NUMBER: _ClassVar[int]
    NOMINAL_GEAR_ENCODER_VALUE_6_FIELD_NUMBER: _ClassVar[int]
    DESIRED_CADENCE_HIGH_FIELD_NUMBER: _ClassVar[int]
    DESIRED_CADENCE_LOW_FIELD_NUMBER: _ClassVar[int]
    nominal_gear_encoder_value_1: float
    nominal_gear_encoder_value_2: float
    nominal_gear_encoder_value_3: float
    nominal_gear_encoder_value_4: float
    nominal_gear_encoder_value_5: float
    nominal_gear_encoder_value_6: float
    desired_cadence_high: float
    desired_cadence_low: float
    def __init__(self, nominal_gear_encoder_value_1: _Optional[float] = ..., nominal_gear_encoder_value_2: _Optional[float] = ..., nominal_gear_encoder_value_3: _Optional[float] = ..., nominal_gear_encoder_value_4: _Optional[float] = ..., nominal_gear_encoder_value_5: _Optional[float] = ..., nominal_gear_encoder_value_6: _Optional[float] = ..., desired_cadence_high: _Optional[float] = ..., desired_cadence_low: _Optional[float] = ...) -> None: ...

class Message(_message.Message):
    __slots__ = ("telemetry", "tuning")
    TELEMETRY_FIELD_NUMBER: _ClassVar[int]
    TUNING_FIELD_NUMBER: _ClassVar[int]
    telemetry: Telemetry
    tuning: Tuning
    def __init__(self, telemetry: _Optional[_Union[Telemetry, _Mapping]] = ..., tuning: _Optional[_Union[Tuning, _Mapping]] = ...) -> None: ...
