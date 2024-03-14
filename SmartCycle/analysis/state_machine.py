from statemachine import StateMachine, State 

class SmartCycle_StateDiagram(StateMachine):
    Asleep = State(initial=True)           # Low power mode
    Stopped = State()           # Waiting to bike again
    Shifting_Up = State()       # Shifting to higher gear
    Shifting_Down = State()     # Shifting to lower gear
    Biking = State()

    pedal_start = (
        Asleep.to(Stopped) |
        Asleep.to(Biking)    
    )

    button_wake = (Asleep.to(Stopped))
    coasting_move = (Stopped.to(Biking))
    after_one_minute_idle = (Stopped.to(Asleep))
    shift_down = (Biking.to(Shifting_Down))
    shift_up = (Biking.to(Shifting_Up))
    resume_biking = (Shifting_Up.to(Biking)
                    | Shifting_Down.to(Biking))
    stopping = (Biking.to(Stopped))
    done = (Biking.to(Asleep))

sc = SmartCycle_StateDiagram()
img_path = "state_machine.png"

sc._graph().write_png(img_path)