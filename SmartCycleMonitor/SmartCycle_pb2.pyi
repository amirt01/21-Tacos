from google.protobuf.internal import enum_type_wrapper as _enum_type_wrapper
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class ServerStatus(_message.Message):
    __slots__ = ("speed", "cadence", "target_gear", "current_gear", "state", "up_shift_button", "down_shift_button")
    class State(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        BIKING: _ClassVar[ServerStatus.State]
        STOPPED: _ClassVar[ServerStatus.State]
        ASLEEP: _ClassVar[ServerStatus.State]
    BIKING: ServerStatus.State
    STOPPED: ServerStatus.State
    ASLEEP: ServerStatus.State
    class ButtonState(int, metaclass=_enum_type_wrapper.EnumTypeWrapper):
        __slots__ = ()
        RELEASED: _ClassVar[ServerStatus.ButtonState]
        PRESSED: _ClassVar[ServerStatus.ButtonState]
    RELEASED: ServerStatus.ButtonState
    PRESSED: ServerStatus.ButtonState
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
    state: ServerStatus.State
    up_shift_button: ServerStatus.ButtonState
    down_shift_button: ServerStatus.ButtonState
    def __init__(self, speed: _Optional[float] = ..., cadence: _Optional[float] = ..., target_gear: _Optional[int] = ..., current_gear: _Optional[int] = ..., state: _Optional[_Union[ServerStatus.State, str]] = ..., up_shift_button: _Optional[_Union[ServerStatus.ButtonState, str]] = ..., down_shift_button: _Optional[_Union[ServerStatus.ButtonState, str]] = ...) -> None: ...
